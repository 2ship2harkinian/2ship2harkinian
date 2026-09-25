#include "Actions.h"

extern "C" {
#include "z64.h"
#include "variables.h"
#include "functions.h"
#include "macros.h"
void func_80833B18(PlayState* play, Player* thisx, s32 arg2, f32 speed, f32 velocityY, s16 arg5,
                   s32 invincibilityTimer);
}

static GIActions::Register blastAction({
    .id = GI_ACTION_BLAST,
    .name = "blast",
    .displayName = "Blast",
    .valence = GI_VALENCE_NEGATIVE,
    .canApply = GIActions::Gates::NotOnHorse,
    .onStart =
        [](GIAction& action) {
            Player* player = GET_PLAYER(gPlayState);

            player->actor.colChkInfo.damage = 16;
            func_80833B18(gPlayState, player, 2, 0, 0, 0, 0);
            Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_BOM, player->actor.world.pos.x,
                        player->actor.world.pos.y, player->actor.world.pos.z, 1, 0, 0, 0);
        },
});
