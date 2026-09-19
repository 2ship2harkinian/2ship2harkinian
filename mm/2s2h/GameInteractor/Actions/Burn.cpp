#include "Actions.h"

extern "C" {
#include "z64.h"
#include "variables.h"
#include "functions.h"
#include "macros.h"
void func_80833B18(PlayState* play, Player* thisx, s32 arg2, f32 speed, f32 velocityY, s16 arg5,
                   s32 invincibilityTimer);
}

static GIActions::Register burnAction({
    .id = GI_ACTION_BURN,
    .name = "burn",
    .displayName = "Burn",
    .valence = GI_VALENCE_NEGATIVE,
    .canApply = GIActions::Gates::NotOnHorse,
    .onStart =
        [](GIAction& action) {
            Player* player = GET_PLAYER(gPlayState);

            for (int i = 0; i < 18; i++) {
                player->bodyFlameTimers[i] = static_cast<uint8_t>(Rand_S16Offset(0, 200));
            }
            player->bodyIsBurning = true;
            func_80833B18(gPlayState, player, 0, 0, 0, 0, 0);
        },
});
