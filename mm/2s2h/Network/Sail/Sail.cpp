#include "Sail.h"
#include <libultraship/libultraship.h>
#include <nlohmann/json.hpp>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/GameInteractor/Actions/Actions.h"

#include <functional>
#include <string>
#include <vector>

// Sail lets something outside the game make it do things. Every message carries an `id` that comes
// back on the reply.
//
//   -> { "id":1, "type":"command", "command":"reset" }
//   <- { "id":1, "type":"result", "outcome":"ok" }
//
//   -> { "id":2, "type":"action.apply", "name":"scaleLink",
//        "params":{"scale":0.4}, "duration":600, "expiresAfter":600, "lifetime":"session" }
//   <- { "id":2, "type":"result", "outcome":"applied" }
//   <- { "id":2, "type":"action.ended", "outcome":"finished" }   // later, unsolicited
//
//   -> { "id":3, "type":"action.remove", "name":"scaleLink" }
//   <- { "id":3, "type":"result", "outcome":"ok", "cancelled":1 }
//
//   -> { "id":4, "type":"action.list" }
//   <- { "id":4, "type":"result", "outcome":"ok", "protocol":2, "actions":[...] }
//
//   -> { "id":5, "type":"action.status" }
//   <- { "id":5, "type":"result", "outcome":"ok", "ready":true, "pending":0, "active":1 }
//
//   -> { "id":6, "type":"hook.list" }
//   <- { "id":6, "type":"result", "outcome":"ok", "hooks":[{"name":"OnItemGive","idFilter":true}] }
//
//   -> { "id":7, "type":"subscribe", "hookName":"OnItemGive", "hookIdFilter":10 }
//   <- { "id":7, "type":"result", "outcome":"ok" }
//   <- { "id":*, "type":"hook", "hook":{...} }                   // whenever it fires
//
// `outcome` is the only status field, and it's the same word whatever went wrong:
//
//   ok         a query or command did what was asked
//   applied    the action happened
//   finished   a timed action's duration ran out
//   cancelled  it was stopped, or dropped when a save loaded
//   expired    the game never became ready in time -- nothing happened, ask again later
//   impossible it can never apply as asked -- nothing happened, don't ask again
//   invalid    the message itself was wrong; `reason` says how
//
// An action.apply gets no immediate reply: the answer comes when the action actually lands, so
// "applied" means it happened rather than "the packet parsed". Only the first word back is a
// `result`; anything after it is an unsolicited message, so an id is never answered twice.
//
// Sail is where JSON stops. Everything below the boundary speaks GIParams, so GameInteractor never
// learns a wire format and anything else driving it can pick its own protocol.

// Frames an action waits for the game to become ready before giving up, when the sender doesn't
// say. Something is always waiting on the answer, so the default is finite rather than forever --
// and twice the longest default duration, so a request queued behind a full-length effect in its
// exclusion group outlives the wait instead of expiring one frame short of its turn.
static constexpr uint32_t DEFAULT_EXPIRES_AFTER = 20 * 60; // 60 seconds

// Bumped when the shape of what Sail sends or accepts changes incompatibly. Reported by
// "action.list" so a server can refuse to talk to a client it doesn't understand.
static constexpr int SAIL_PROTOCOL_VERSION = 2;

static bool ParamsFromJson(const nlohmann::json& source, GIParams& params, std::string& error) {
    if (!source.is_object()) {
        error = "params must be an object";
        return false;
    }

    for (auto& [key, value] : source.items()) {
        if (value.is_boolean()) {
            params.Set(key, value.get<bool>());
        } else if (value.is_number_integer()) {
            params.Set(key, value.get<int32_t>());
        } else if (value.is_number_float()) {
            params.Set(key, value.get<float>());
        } else if (value.is_string()) {
            params.Set(key, value.get<std::string>());
        } else {
            error = "param '" + key + "' is not a bool, number, or string";
            return false;
        }
    }

    return true;
}

// MARK: - Hooks

static void SendHook(const char* type, nlohmann::json fields) {
    // Hook messages are unsolicited, so the id just has to exist; a counter keeps them distinct
    // without pretending anyone asked.
    static uint32_t nextMessageId = 1;
    nlohmann::json payload;
    payload["id"] = nextMessageId++;
    payload["type"] = "hook";
    payload["hook"] = std::move(fields);
    payload["hook"]["type"] = type;
    Sail::Instance->QueueOutgoingPacket(std::move(payload));
}

// Argument types must match GameInteractor_HookTable.h exactly. They're implicitly convertible if
// they don't, so a narrower one compiles and silently truncates on the wire.
static void OnSceneInitHandler(s8 sceneId, s8 spawnNum) {
    SendHook("OnSceneInit", { { "sceneId", sceneId } });
}

static void OnItemGiveHandler(u8 item) {
    SendHook("OnItemGive", { { "itemId", item } });
}

static void OnActorInitHandler(Actor* actor) {
    SendHook("OnActorInit", { { "actorId", actor->id }, { "params", actor->params } });
}

static void OnFlagSetHandler(FlagType flagType, u32 flag) {
    SendHook("OnFlagSet", { { "flagType", (int32_t)flagType }, { "flag", flag } });
}

static void OnFlagUnsetHandler(FlagType flagType, u32 flag) {
    SendHook("OnFlagUnset", { { "flagType", (int32_t)flagType }, { "flag", flag } });
}

static void OnSceneFlagSetHandler(s16 sceneId, FlagType flagType, u32 flag) {
    SendHook("OnSceneFlagSet", { { "sceneId", sceneId }, { "flagType", (int32_t)flagType }, { "flag", flag } });
}

static void OnSceneFlagUnsetHandler(s16 sceneId, FlagType flagType, u32 flag) {
    SendHook("OnSceneFlagUnset", { { "sceneId", sceneId }, { "flagType", (int32_t)flagType }, { "flag", flag } });
}

// One row per hook Sail can forward. The subscribe and unsubscribe thunks close over the hook's
// type, so it can't be lost between registering and tearing down.
struct HookBinding {
    const char* name;
    // Whether GameInteractor dispatches this hook by id, and so whether `hookIdFilter` means
    // anything for it. That lives in GameInteractor.cpp's ExecuteHooksForID call sites and has no
    // representation in the hook table, so it has to be mirrored here by hand.
    bool idFilter;
    std::function<HOOK_ID()> subscribe;
    std::function<HOOK_ID(int32_t)> subscribeForId;
    std::function<void(HOOK_ID)> unsubscribe;
    std::function<void(HOOK_ID)> unsubscribeForId;
};

template <typename H> static HookBinding Bind(const char* name, bool idFilter, typename H::fn handler) {
    return {
        name,
        idFilter,
        [handler] { return GameInteractor::Instance->RegisterGameHook<H>(handler); },
        [handler](int32_t id) { return GameInteractor::Instance->RegisterGameHookForID<H>(id, handler); },
        [](HOOK_ID hookId) { GameInteractor::Instance->UnregisterGameHook<H>(hookId); },
        [](HOOK_ID hookId) { GameInteractor::Instance->UnregisterGameHookForID<H>(hookId); },
    };
}

static const std::vector<HookBinding>& HookBindings() {
    static const std::vector<HookBinding> bindings = {
        Bind<GameInteractor::OnSceneInit>("OnSceneInit", true, OnSceneInitHandler),
        Bind<GameInteractor::OnItemGive>("OnItemGive", true, OnItemGiveHandler),
        Bind<GameInteractor::OnActorInit>("OnActorInit", true, OnActorInitHandler),
        Bind<GameInteractor::OnFlagSet>("OnFlagSet", false, OnFlagSetHandler),
        Bind<GameInteractor::OnFlagUnset>("OnFlagUnset", false, OnFlagUnsetHandler),
        Bind<GameInteractor::OnSceneFlagSet>("OnSceneFlagSet", false, OnSceneFlagSetHandler),
        Bind<GameInteractor::OnSceneFlagUnset>("OnSceneFlagUnset", false, OnSceneFlagUnsetHandler),
    };
    return bindings;
}

static const HookBinding* FindHookBinding(const std::string& name) {
    for (const auto& binding : HookBindings()) {
        if (name == binding.name) {
            return &binding;
        }
    }
    return nullptr;
}

// Live subscriptions, keyed the way the sender addressed them.
static std::unordered_map<std::string, HOOK_ID> hookIds;
static std::unordered_map<std::string, std::unordered_map<int32_t, HOOK_ID>> filteredHookIds;

// MARK: - Actions

static const char* OutcomeName(GIActionStatus status) {
    switch (status) {
        case GI_STATUS_APPLIED:
            return "applied";
        case GI_STATUS_FINISHED:
            return "finished";
        case GI_STATUS_CANCELLED:
            return "cancelled";
        case GI_STATUS_EXPIRED:
            return "expired";
        case GI_STATUS_IMPOSSIBLE:
            return "impossible";
    }
    return "unknown";
}

static nlohmann::json DescribeAction(const GIActions::Definition& definition) {
    nlohmann::json entry;
    entry["name"] = definition.name;
    entry["displayName"] = definition.displayName;
    entry["timed"] = definition.IsTimed();
    entry["defaultDuration"] = definition.defaultDuration;
    entry["stacking"] = definition.stacking == GI_STACK_REFRESH ? "refresh" : "queue";
    entry["valence"] = definition.valence == GI_VALENCE_POSITIVE   ? "positive"
                       : definition.valence == GI_VALENCE_NEGATIVE ? "negative"
                                                                   : "neutral";
    entry["params"] = nlohmann::json::array();
    for (const auto& spec : definition.schema) {
        nlohmann::json param;
        param["name"] = spec.name;
        param["type"] = GIParamTypeName(spec.type);
        param["required"] = spec.required;
        if (spec.min.has_value()) {
            param["min"] = *spec.min;
        }
        if (spec.max.has_value()) {
            param["max"] = *spec.max;
        }
        if (spec.defaultValue.has_value()) {
            std::visit([&param](auto&& value) { param["default"] = value; }, *spec.defaultValue);
        }
        entry["params"].push_back(param);
    }
    return entry;
}

// Queues an action and wires its answers back to the sender. Answers nothing itself: the reply
// comes from onComplete once the action lands, or `fail` if it never got that far.
template <typename Fail>
static void ApplyAction(const nlohmann::json& payload, const GIActions::Definition& definition, Fail&& fail) {
    GIParams params;
    std::string error;
    if (payload.contains("params") && !ParamsFromJson(payload["params"], params, error)) {
        fail("bad params for '" + std::string(definition.name) + "': " + error, "invalid");
        return;
    }

    auto action = definition.Build(std::move(params), &error);
    if (!action.has_value()) {
        fail(error, "invalid");
        return;
    }

    if (payload.contains("duration")) {
        // Only a timed action can take one, and it has to stay timed: overriding to 0 would run
        // onStart -- which may install hooks -- and then never run onEnd, leaving the effect stuck
        // on with nothing for action.remove to find.
        int64_t duration = payload["duration"].get<int64_t>();
        if (!definition.IsTimed() || duration <= 0 || duration > UINT32_MAX) {
            fail("bad duration for '" + std::string(definition.name) +
                     "': must be a positive frame count on a timed action",
                 "invalid");
            return;
        }
        action->duration = (uint32_t)duration;
    }

    int64_t expiresAfter = payload.value("expiresAfter", (int64_t)DEFAULT_EXPIRES_AFTER);
    if (expiresAfter < 0 || expiresAfter > UINT32_MAX) {
        fail("bad expiresAfter for '" + std::string(definition.name) + "': must be a frame count (0 waits forever)",
             "invalid");
        return;
    }
    action->expiresAfter = (uint32_t)expiresAfter;
    // Session by default, since a Sail session outlives any one save -- but a sender tying a
    // action to the file it was meant for can say so.
    action->lifetime =
        payload.value("lifetime", std::string("session")) == "save" ? GI_LIFETIME_SAVE : GI_LIFETIME_SESSION;

    nlohmann::json id = payload["id"];
    // The first word back answers the action, whatever it turned out to be -- applied, or expired
    // without ever running. Only a timed action speaks twice, and its second word is unsolicited,
    // so it's an unsolicited message rather than a second reply to the same id. A request merged
    // into a running instance by GI_STACK_REFRESH only ever hears "applied": the instance keeps
    // its original onComplete, so its action.ended goes to the id that started it.
    action->onComplete = [id, replied = false](GIActionStatus status) mutable {
        nlohmann::json message;
        message["id"] = id;
        message["type"] = replied ? "action.ended" : "result";
        message["outcome"] = OutcomeName(status);
        replied = true;
        Sail::Instance->QueueOutgoingPacket(std::move(message));
    };

    GameInteractor::Instance->Queue(std::move(*action));
}

// Drops every game-hook subscription Sail opened for a connected server. Must run on the game
// thread: unsubscribing mutates GameInteractor's hook registry, which the game thread walks every
// frame.
static void TearDownSubscriptions() {
    for (auto& [hookName, hookId] : hookIds) {
        if (const HookBinding* binding = FindHookBinding(hookName)) {
            binding->unsubscribe(hookId);
        }
    }
    hookIds.clear();

    for (auto& [hookName, filtered] : filteredHookIds) {
        const HookBinding* binding = FindHookBinding(hookName);
        if (binding == nullptr) {
            continue;
        }
        for (auto& [hookIdFilter, hookId] : filtered) {
            binding->unsubscribeForId(hookId);
        }
    }
    filteredHookIds.clear();
}

// The game-thread pump, registered for as long as Sail is enabled. Draining remote messages and
// tearing subscriptions down when the link drops both mutate GameInteractor state, so they run here
// rather than from the network thread's connect/disconnect callbacks, which would race the game
// thread's own use of that state.
static HOOK_ID sPumpHookId = 0;
static bool sWasConnected = false;

bool Sail::Enable() {
    if (isEnabled) {
        return true;
    }

    // Nothing enabled (bad host): no pump either, or it would sit registered with no thread behind
    // it and a second Enable would stack another.
    if (!Network::Enable(CVarGetString("gNetwork.Sail.Host", "127.0.0.1"),
                         CVarGetInteger("gNetwork.Sail.Port", 43384))) {
        return false;
    }

    // Registered on the game thread (Enable is only ever called from it). Packets the network
    // thread queues before this line just wait in the queue for the pump's first run. The pump is
    // a member-scope lambda so it can still reach the private OnIncomingJson.
    sWasConnected = false;
    sPumpHookId = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameStateMainStart>([]() {
        // A dropped connection means the server is gone, so drop its subscriptions. Edge-triggered
        // off isConnected (set by the network thread) so it fires exactly once per disconnect.
        bool connectedNow = Sail::Instance->isConnected;
        if (sWasConnected && !connectedNow) {
            TearDownSubscriptions();
        }
        sWasConnected = connectedNow;

        std::queue<nlohmann::json> packetQueue;
        Sail::Instance->SwapIncomingPacketQueue(packetQueue);
        while (!packetQueue.empty()) {
            Sail::Instance->OnIncomingJson(std::move(packetQueue.front()));
            packetQueue.pop();
        }
    });

    return true;
}

void Sail::Disable() {
    if (!isEnabled) {
        return;
    }

    // Stop the network thread first; then, still on the game thread, unregister the pump and drop
    // any subscriptions it left behind.
    Network::Disable();
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnGameStateMainStart>(sPumpHookId);
    sPumpHookId = 0;
    TearDownSubscriptions();
    sWasConnected = false;
}

void Sail::OnIncomingJson(nlohmann::json payload) {
    nlohmann::json responsePayload;
    responsePayload["type"] = "result";

    // Every rejection logs the same reason it reports, so the sender is never left with a bare
    // failure it can't act on.
    auto fail = [&](const std::string& reason, const char* outcome = "invalid") {
        SPDLOG_ERROR("[Sail] {}", reason);
        responsePayload["outcome"] = outcome;
        responsePayload["reason"] = reason;
        QueueOutgoingPacket(responsePayload);
    };
    auto ok = [&]() {
        responsePayload["outcome"] = "ok";
        QueueOutgoingPacket(responsePayload);
    };

    try {
        if (!payload.contains("id")) {
            fail("received payload without ID");
            return;
        }

        responsePayload["id"] = payload["id"];

        if (!payload.contains("type")) {
            fail("received payload without type");
            return;
        }

        std::string payloadType = payload["type"].get<std::string>();

        // A raw escape hatch into the debug console, and the only thing here that skips the queue
        // entirely -- no readiness gate, no lifetime, and `ok` means "the string was dispatched"
        // rather than "it worked". Some console commands (give_item, spawn, entrance) reach the
        // game world just as directly as an action does; the action form of those is the one that
        // waits its turn, so prefer it where it exists.
        if (payloadType == "command") {
            if (!payload.contains("command")) {
                fail("received command payload without command");
                return;
            }

            std::reinterpret_pointer_cast<Ship::ConsoleWindow>(
                Ship::Context::GetRawInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))
                ->Dispatch(payload["command"].get<std::string>());
            ok();
            return;
        } else if (payloadType == "action.apply" || payloadType == "action.remove") {
            if (!payload.contains("name")) {
                fail("received " + payloadType + " without a name");
                return;
            }

            std::string name = payload["name"].get<std::string>();
            const GIActions::Definition* definition = GIActions::FindByName(name);
            if (definition == nullptr) {
                fail("unknown action '" + name + "'", "impossible");
                return;
            }

            // Ends every running instance of the action, firing its onEnd so nothing is left
            // half-applied. `cancelled` says whether there was anything to stop.
            if (payloadType == "action.remove") {
                responsePayload["cancelled"] = GameInteractor::Instance->CancelAction(definition->id);
                ok();
                return;
            }

            ApplyAction(payload, *definition, fail);
            return;
        } else if (payloadType == "action.list") {
            responsePayload["protocol"] = SAIL_PROTOCOL_VERSION;
            responsePayload["actions"] = nlohmann::json::array();
            for (const auto& definition : GIActions::All()) {
                responsePayload["actions"].push_back(DescribeAction(definition));
            }
            ok();
            return;
        } else if (payloadType == "action.status") {
            // Lets a sender hold something back until the player can actually receive it, rather
            // than firing and waiting for an `expired`.
            responsePayload["ready"] = GameInteractor::Instance->CanProcessActions() == GI_AVAILABILITY_READY;
            responsePayload["pending"] = GameInteractor::Instance->PendingActions().size();
            responsePayload["active"] = GameInteractor::Instance->ActiveActions().size();
            ok();
            return;
        } else if (payloadType == "hook.list") {
            responsePayload["hooks"] = nlohmann::json::array();
            for (const auto& binding : HookBindings()) {
                responsePayload["hooks"].push_back({ { "name", binding.name }, { "idFilter", binding.idFilter } });
            }
            ok();
            return;
        } else if (payloadType == "subscribe" || payloadType == "unsubscribe") {
            if (!payload.contains("hookName")) {
                fail("received " + payloadType + " without hookName");
                return;
            }

            std::string hookName = payload["hookName"].get<std::string>();
            const HookBinding* binding = FindHookBinding(hookName);
            if (binding == nullptr) {
                fail("unknown hookName '" + hookName + "'", "impossible");
                return;
            }

            bool byId = payload.contains("hookIdFilter");
            if (byId && !binding->idFilter) {
                // Registering it filtered would attach to a hook nothing dispatches by id, so it
                // would just never fire. Say so rather than going quiet.
                fail("hookName '" + hookName + "' does not support hookIdFilter", "impossible");
                return;
            }
            int32_t hookIdFilter = byId ? payload["hookIdFilter"].get<int32_t>() : 0;

            if (payloadType == "subscribe") {
                if (byId) {
                    if (!filteredHookIds[hookName].contains(hookIdFilter)) {
                        filteredHookIds[hookName][hookIdFilter] = binding->subscribeForId(hookIdFilter);
                    }
                } else if (!hookIds.contains(hookName)) {
                    hookIds[hookName] = binding->subscribe();
                }
            } else if (byId) {
                auto it = filteredHookIds[hookName].find(hookIdFilter);
                if (it != filteredHookIds[hookName].end()) {
                    binding->unsubscribeForId(it->second);
                    filteredHookIds[hookName].erase(it);
                }
            } else {
                auto it = hookIds.find(hookName);
                if (it != hookIds.end()) {
                    binding->unsubscribe(it->second);
                    hookIds.erase(it);
                }
            }

            ok();
            return;
        } else {
            fail("unknown payload type '" + payloadType + "'", "impossible");
            return;
        }
    } catch (const std::exception& e) {
        // Usually a field of the wrong JSON type. The sender is still owed an answer for this id;
        // going quiet here would leave it waiting forever.
        fail(std::string("exception handling message: ") + e.what());
    } catch (...) { fail("unknown exception handling message"); }
}
