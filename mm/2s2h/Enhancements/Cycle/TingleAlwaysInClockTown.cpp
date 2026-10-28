#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
Actor* Actor_SpawnEntry(ActorContext* actorCtx, ActorEntry* actorEntry, PlayState* play);
}

#define CVAR_NAME "gEnhancements.Cycle.TingleAlwaysInClockTown"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

static void RegisterTingleAlwaysInClockTown() {
    COND_ID_HOOK(OnActorEntryHalfDaysBits, ACTOR_EN_BAL, CVAR, [](ActorEntry* entry, s32* halfDaysBits) {
        if (gPlayState->sceneId == SCENE_BACKTOWN) {
            *halfDaysBits = HALFDAYBIT_ALL;
        }
    });

    if (gPlayState == NULL || gPlayState->sceneId != SCENE_BACKTOWN ||
        !(HALFDAYBIT_NIGHTS & gPlayState->actorCtx.halfDaysBit)) {
        return;
    }

    // At this point we toggled while already standing in North Clock Town at night.

    Actor* tingle = SubS_FindActor(gPlayState, NULL, ACTORCAT_NPC, ACTOR_EN_BAL);
    if (CVAR && tingle == NULL) {
        // numSetupActors is negated once the setup actors have been spawned
        ActorEntry* entry = gPlayState->setupActorList;

        // Looking for the og tingle entry so we don't have to hardcode his position
        for (s32 i = 0; i < ABS(gPlayState->numSetupActors); i++, entry++) {
            if ((entry->id & 0x1FFF) == ACTOR_EN_BAL) {
                Actor_SpawnEntry(&gPlayState->actorCtx, entry, gPlayState);
                break;
            }
        }
    } else if (!CVAR && tingle != NULL) {
        Actor_Kill(tingle);
    }
}

static RegisterShipInitFunc initFunc(RegisterTingleAlwaysInClockTown, { CVAR_NAME });
