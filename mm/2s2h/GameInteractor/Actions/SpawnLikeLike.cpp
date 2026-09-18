#include "Actions.h"

#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "z64.h"
#include "variables.h"
#include "functions.h"
#include "macros.h"
}

static GIActions::Register spawnLikeLikeAction({
    .id = GI_ACTION_SPAWN_LIKE_LIKE,
    .name = "spawnLikeLike",
    .displayName = "Spawn Like Like",
    .valence = GI_VALENCE_NEGATIVE,
    .canApply = GIActions::Gates::NotOnHorse,
    .onStart =
        [](GIAction& action) {
            Player* player = GET_PLAYER(gPlayState);

            Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_RR, player->actor.world.pos.x,
                        player->actor.world.pos.y, player->actor.world.pos.z, 0, 0, 0, 1);
        },
});

// Most scenes don't load OBJECT_RR, so without this the spawn above quietly does nothing.
void RegisterSpawnLikeLikeObjectDependency() {
    REGISTER_VB_SHOULD(VB_ENABLE_OBJECT_DEPENDENCY, {
        ObjectId objectId = (ObjectId)va_arg(args, int);
        if (objectId == OBJECT_RR || objectId == OBJECT_GI_SHIELD_2) {
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSpawnLikeLikeObjectDependency);
