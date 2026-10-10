#include "Actions.h"

extern "C" {
#include "z64.h"
#include "variables.h"
#include "functions.h"
#include "macros.h"
void func_80833B18(PlayState* play, Player* thisx, s32 arg2, f32 speed, f32 velocityY, s16 arg5,
                   s32 invincibilityTimer);
}

static GIActions::Register freezeAction({
    .id = GI_ACTION_FREEZE,
    .name = "freeze",
    .displayName = "Freeze",
    .valence = GI_VALENCE_NEGATIVE,
    .canApply = GIActions::Gates::NotOnHorse,
    .onStart = [](GIAction& action) { func_80833B18(gPlayState, GET_PLAYER(gPlayState), 3, 0, 0, 0, 0); },
});
