#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.Cycle.MayorArguesUntilMoonfall"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

// The actor entries for the four people arguing with Mayor Dotour carry a half-day mask that excludes the night of
// the Final Day, so they are neither spawned on room load nor allowed to survive the half-day rolling over while
// the player is standing in the room. Granting them that half-day lets them spawn from their own entries, in their
// own positions and with their own params. Dotour is not listed because he is present either way.
static void KeepArguingOnFinalNight(ActorEntry* entry, s32* halfDaysBits) {
    if ((gPlayState->sceneId == SCENE_SONCHONOIE) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_RESOLVED_MAYOR_MEETING)) {
        *halfDaysBits |= HALFDAYBIT_DAY3_NIGHT;
    }
}

static void RegisterMayorArguesUntilMoonfall() {
    COND_VB_SHOULD(VB_MAYOR_MEETING_END_ON_FINAL_NIGHT, CVAR, {
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_MAYOR_HEART_PIECE)) {
            *should = false;
        }
    });

    COND_ID_HOOK(OnActorEntryHalfDaysBits, ACTOR_EN_MUTO, CVAR, KeepArguingOnFinalNight);   // Mutoh
    COND_ID_HOOK(OnActorEntryHalfDaysBits, ACTOR_EN_BAISEN, CVAR, KeepArguingOnFinalNight); // Viscen
    COND_ID_HOOK(OnActorEntryHalfDaysBits, ACTOR_EN_HEISHI, CVAR, KeepArguingOnFinalNight); // Soldier
    COND_ID_HOOK(OnActorEntryHalfDaysBits, ACTOR_EN_DAIKU, CVAR, KeepArguingOnFinalNight);  // Carpenter
}

static RegisterShipInitFunc initFunc(RegisterMayorArguesUntilMoonfall, { CVAR_NAME });
