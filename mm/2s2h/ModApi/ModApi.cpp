#include "ModApi.h"

#include <spdlog/spdlog.h>
#include <string>
#include <unordered_map>
#include <libultraship/bridge/consolevariablebridge.h>
#include <ship/Context.h>
#include <ship/debug/Console.h>

#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/BenGui/Notification.h"
#include "2s2h/ShipInit.hpp"

#include "variables.h"
#include "functions.h"

static std::unordered_map<std::string, void (*)(void*)> BuildHookEntries() {
    std::unordered_map<std::string, void (*)(void*)> entries;

#define DEFINE_HOOK(name, args)                                           \
    entries[#name] = [](void* callback) {                                 \
        GameInteractor::Instance->RegisterGameHook<GameInteractor::name>( \
            (S2HCb_##name)callback);                                      \
    };
#include "2s2h/GameInteractor/GameInteractor_HookTable.h"
#undef DEFINE_HOOK

    return entries;
}

static bool RegisterHookByName(const char* name, void* callback) {
    if (name == nullptr || callback == nullptr) {
        return false;
    }

    static const std::unordered_map<std::string, void (*)(void*)> entries = BuildHookEntries();

    auto entry = entries.find(name);
    if (entry == entries.end()) {
        SPDLOG_ERROR("[ModApi] A mod asked for hook '{}', which this build does not have", name);
        return false;
    }

    entry->second(callback);
    return true;
}

static bool RegisterVB(GIVanillaBehavior flag, S2HVbCallback callback) {
    if (callback == nullptr) {
        return false;
    }

    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(
        flag, [callback](GIVanillaBehavior, bool* should, va_list originalArgs) {
            va_list args;
            va_copy(args, originalArgs);
            callback(should, args);
            va_end(args);
        });

    return true;
}

static void S2H_Log(const char* message) {
    SPDLOG_INFO("{}", message);
}

static void S2H_Notify(const char* message) {
    Notification::Emit({ .message = message });
}

static int32_t S2H_RunCommand(const char* command) {
    return Ship::Context::GetRawInstance()->GetConsole()->Run(command, nullptr);
}

static void S2H_CVarApply(const char* name) {
    ShipInit::Init(name);
}

static std::unordered_map<std::string, void*> BuildSymbolEntries() {
    std::unordered_map<std::string, void*> entries;

#define S2H_FUNCTION(name) entries[#name] = (void*)name;
#define S2H_GLOBAL(name) entries[#name] = (void*)&name;

    S2H_FUNCTION(S2H_Log)
    S2H_FUNCTION(S2H_Notify)
    S2H_FUNCTION(S2H_CVarApply)
    S2H_FUNCTION(S2H_RunCommand)

    S2H_FUNCTION(CVarGetInteger)
    S2H_FUNCTION(CVarGetFloat)
    S2H_FUNCTION(CVarGetString)
    S2H_FUNCTION(CVarGetColor)
    S2H_FUNCTION(CVarGetColor24)
    S2H_FUNCTION(CVarSetInteger)
    S2H_FUNCTION(CVarSetFloat)
    S2H_FUNCTION(CVarSetString)
    S2H_FUNCTION(CVarSetColor)
    S2H_FUNCTION(CVarSetColor24)
    S2H_FUNCTION(CVarRegisterInteger)
    S2H_FUNCTION(CVarRegisterFloat)
    S2H_FUNCTION(CVarRegisterString)
    S2H_FUNCTION(CVarRegisterColor)
    S2H_FUNCTION(CVarRegisterColor24)
    S2H_FUNCTION(CVarClear)
    S2H_FUNCTION(CVarClearBlock)
    S2H_FUNCTION(CVarCopy)
    S2H_FUNCTION(CVarLoad)
    S2H_FUNCTION(CVarSave)

    S2H_FUNCTION(Flags_GetSwitch)
    S2H_FUNCTION(Flags_SetSwitch)
    S2H_FUNCTION(Flags_UnsetSwitch)
    S2H_FUNCTION(Flags_GetTreasure)
    S2H_FUNCTION(Flags_SetTreasure)
    S2H_FUNCTION(Flags_GetAllTreasure)
    S2H_FUNCTION(Flags_SetAllTreasure)
    S2H_FUNCTION(Flags_GetCollectible)
    S2H_FUNCTION(Flags_SetCollectible)
    S2H_FUNCTION(Flags_GetClear)
    S2H_FUNCTION(Flags_SetClear)
    S2H_FUNCTION(Flags_UnsetClear)
    S2H_FUNCTION(Flags_GetClearTemp)
    S2H_FUNCTION(Flags_SetClearTemp)
    S2H_FUNCTION(Flags_UnsetClearTemp)
    S2H_FUNCTION(Flags_GetEventChkInf)
    S2H_FUNCTION(Flags_SetEventChkInf)
    S2H_FUNCTION(Flags_GetInfTable)
    S2H_FUNCTION(Flags_SetInfTable)
    S2H_FUNCTION(Flags_SetWeekEventReg)
    S2H_FUNCTION(Flags_ClearWeekEventReg)
    S2H_FUNCTION(Flags_SetWeekEventRegHorseRace)
    S2H_FUNCTION(Flags_SetEventInf)
    S2H_FUNCTION(Flags_ClearEventInf)
    S2H_FUNCTION(Flags_GetRandoInf)
    S2H_FUNCTION(Flags_SetRandoInf)
    S2H_FUNCTION(Flags_ClearRandoInf)

    S2H_GLOBAL(gPlayState)
    S2H_GLOBAL(gSaveContext)
    S2H_GLOBAL(gRegEditor)
    S2H_GLOBAL(gActorOverlayTable)
    S2H_GLOBAL(gBitFlags)
    S2H_GLOBAL(gCullBackDList)
    S2H_GLOBAL(gEmptyDL)
    S2H_GLOBAL(gItemIcons)
    S2H_GLOBAL(gItemSlots)
    S2H_GLOBAL(gSfxDefaultPos)
    S2H_GLOBAL(gSfxDefaultReverb)
    S2H_GLOBAL(gSfxDefaultFreqAndVolScale)

#undef S2H_FUNCTION
#undef S2H_GLOBAL

    return entries;
}

static void* GetSymbol(const char* name) {
    if (name == nullptr) {
        return nullptr;
    }

    static const std::unordered_map<std::string, void*> entries = BuildSymbolEntries();

    auto entry = entries.find(name);
    if (entry == entries.end()) {
        SPDLOG_ERROR("[ModApi] A mod asked for symbol '{}', which this build does not expose", name);
        return nullptr;
    }

    return entry->second;
}

static S2HModApi BuildTable() {
    S2HModApi api = {};

    api.tableSize = sizeof(S2HModApi);
    api.RegisterHookByName = RegisterHookByName;
    api.RegisterVB = RegisterVB;
    api.GetSymbol = GetSymbol;

    return api;
}

const S2HModApi* ModApi_Get() {
    static const S2HModApi sModApi = BuildTable();
    return &sModApi;
}
