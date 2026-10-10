#ifndef GAME_INTERACTOR_ACTIONS_H
#define GAME_INTERACTOR_ACTIONS_H

#ifdef __cplusplus

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "2s2h/GameInteractor/GameInteractorAction.h"

extern "C" {
#include "z64.h"
}

namespace GIActions {

struct Definition {
    GIActionId id;
    // Stable identifier remotes address actions by; renaming one is a protocol break.
    const char* name;
    const char* displayName;
    GIValence valence = GI_VALENCE_NEUTRAL;
    uint32_t defaultDuration = 0; // Frames. 0 means instant.
    GIStacking stacking = GI_STACK_QUEUE;
    GIExclusionGroup exclusionGroup = GI_EXCLUSION_NONE;
    std::vector<GIParamSpec> schema = {};

    GIActionGate canApply = nullptr;
    GIActionFunc onStart = nullptr;
    GIActionFunc onTick = nullptr;
    GIActionFunc onEnd = nullptr;

    bool IsTimed() const {
        return defaultDuration > 0 || onTick != nullptr || onEnd != nullptr;
    }

    // Validates `params` against the schema; on failure returns nullopt and fills `error`.
    std::optional<GIAction> Build(GIParams params, std::string* error = nullptr) const;
};

struct Register {
    explicit Register(Definition definition);
};

const std::vector<Definition>& All();
const Definition* Get(GIActionId id);
const Definition* FindByName(std::string_view name);

namespace Gates {
GIActionAvailability NotOnHorse(const GIAction& action);
} // namespace Gates

Player* PlayerOrNull();

namespace Setting {
// Snapshot/Restore for a cvar an action borrows; ABSENT means there was no value to put back.
inline constexpr int32_t ABSENT = INT32_MIN;

int32_t Snapshot(const char* cvar);
void Restore(const char* cvar, int32_t previous);
} // namespace Setting

namespace CameraRoll {
// Adds `offset` to the main camera's roll for this frame. Call from an AfterCameraUpdate hook.
void Apply(Camera* camera, int16_t offset);
} // namespace CameraRoll

namespace PlayerScale {
// The scale Player_Init gives the player actor.
inline constexpr float BASE = 0.01f;

// Re-apply every tick: scene transitions and save loads rebuild the player actor.
void Set(float x, float y, float z);
void Reset();
} // namespace PlayerScale

// MARK: - Typed builders for internal C++ callers

struct GiveItemArgs {
    // Whether to show the get item cutscene. If true and the player is in the air, the player is
    // frozen for a few seconds instead. If this is true you _must_ call
    // CustomMessage::SetActiveCustomMessage in giveItem, otherwise you'll just see a blank message.
    bool showGetItemCutscene;
    // Arbitrary s16 readable from the give/draw functions via CUSTOM_ITEM_PARAM.
    s16 param;
    // Run in the context of an item00 actor. Not usually important, but useful in some cases.
    ActorFunc giveItem;
    ActorFunc drawItem;
};

// Spawns a custom item on the player and delivers it once they pick it up, holding the queue for
// as long as that takes.
GIAction GiveItem(GiveItemArgs args);

struct TransitionArgs {
    u16 entrance;
    u16 cutsceneIndex = 0;
    s8 transitionTrigger = TRANS_TRIGGER_START;
    u8 transitionType = TRANS_TYPE_FADE_BLACK;
};
GIAction Transition(TransitionArgs args);

} // namespace GIActions

#endif // __cplusplus

#endif // GAME_INTERACTOR_ACTIONS_H
