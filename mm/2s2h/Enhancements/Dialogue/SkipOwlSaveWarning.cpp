#include <libultraship/bridge/consolevariablebridge.h>
#include <string_view>
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "BenPort.h"
uint32_t ResourceMgr_GetGameVersion(int index);
}

#define CVAR_NAME "gEnhancements.Saving.PersistentOwlSaves"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

// "You can save your progress and quit here."
static constexpr u16 TEXT_ID_OWL_SAVE = 0xC01;

// "Warning: If you reopen this Owl File, then reset without saving..."
static constexpr size_t TEXT_WARNING_BEGIN_NTSC = 258;
static constexpr size_t TEXT_WARNING_LENGTH_NTSC = 261;
static constexpr std::string_view TEXT_WARNING_PREFIX_NTSC = "\x01\x17Warning:";

static void ModifySaveExplanation(u16* textId, bool* loadFromMessageTable) {
    size_t warningStart;
    size_t warningLength;
    std::string_view warningPrefix;

    // TODO: Add different cases when other versions are supported
    switch (ResourceMgr_GetGameVersion(0)) {
        case MM_NTSC_US_10:
        case MM_NTSC_US_GC:
            warningStart = TEXT_WARNING_BEGIN_NTSC;
            warningLength = TEXT_WARNING_LENGTH_NTSC;
            warningPrefix = TEXT_WARNING_PREFIX_NTSC;
            break;
        default:
            // Unknown region, don't modify the message
            return;
    }

    auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
    // Text mods can change the message, so only erase the warning if it's where we expect
    if (entry.msg.size() < warningStart + warningLength ||
        entry.msg.compare(warningStart, warningPrefix.size(), warningPrefix) != 0) {
        return;
    }

    entry.msg.erase(warningStart, warningLength);
    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
}

static void RegisterSkipOwlSaveWarning() {
    COND_ID_HOOK(OnOpenText, TEXT_ID_OWL_SAVE, CVAR, ModifySaveExplanation);
}

static RegisterShipInitFunc initFunc(RegisterSkipOwlSaveWarning, { CVAR_NAME });
