#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/Rando/Logic/EntranceShuffle.h"

extern "C" {
#include "functions.h"
#include "variables.h"
#include "z64scene.h"
}

namespace Rando {

namespace EntranceShuffle {

static s32 sPlayerSceneExitEntrance = -1;

static s32 GreatBayTurtleRide(s32 entrance) {
    if (gPlayState == NULL) {
        return -1;
    }

    if (((gPlayState->sceneId == SCENE_31MISAKI) || (gPlayState->sceneId == SCENE_KONPEKI_ENT)) &&
        ((entrance == ENTRANCE(GREAT_BAY_TEMPLE, 0)) || (entrance == ENTRANCE(GREAT_BAY_TEMPLE, 1)))) {
        return ENTRANCE(GREAT_BAY_TEMPLE, 0);
    }

    if ((gPlayState->sceneId == SCENE_SEA) && (entrance == ENTRANCE(ZORA_CAPE, 7))) {
        return ENTRANCE(ZORA_CAPE, 7);
    }

    return -1;
}

static RegisterShipInitFunc registerHooks(
    []() {
        COND_HOOK(OnPlayerSceneExit, IsEntranceShuffleEnabled(),
                  [](s32 nextEntrance) { sPlayerSceneExitEntrance = nextEntrance; });

        COND_HOOK(OnSceneInit, IsEntranceShuffleEnabled(), [](s8 sceneId, s8 spawnNum) {
            if (gPlayState->skyboxCtx.dListBuf == NULL) {
                gPlayState->skyboxId = SKYBOX_NONE;
            }
        });

        COND_HOOK(OnPlayDestroy, IsEntranceShuffleEnabled(), []() {
            s32 walkedThroughEntrance = sPlayerSceneExitEntrance;
            sPlayerSceneExitEntrance = -1;

            if (gPlayState != NULL && gPlayState->sceneId == SCENE_KAKUSIANA) {
                return;
            }

            s32 originalEntrance = gSaveContext.save.entrance;
            s32 turtleRideEntrance = GreatBayTurtleRide(originalEntrance);
            s32 takenEntrance;

            if (walkedThroughEntrance == originalEntrance) {
                takenEntrance = originalEntrance;
            } else if (turtleRideEntrance != -1) {
                takenEntrance = turtleRideEntrance;
            } else if (originalEntrance == ENTRANCE(SOUTH_CLOCK_TOWN, 0) && gSaveContext.respawnFlag == 0) {
                takenEntrance = originalEntrance;
            } else {
                return;
            }

            s32 shuffledEntrance = GetShuffledEntrance(takenEntrance);

            if (shuffledEntrance != takenEntrance) {
                gSaveContext.save.entrance = shuffledEntrance;

                gSaveContext.nextCutsceneIndex = 0xFFEF;
            }
        });
    },
    { "IS_RANDO" });

} // namespace EntranceShuffle

} // namespace Rando
