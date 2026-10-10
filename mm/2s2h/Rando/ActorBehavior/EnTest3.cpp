#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

extern "C" {
#include "functions.h"
#include "variables.h"
#include "overlays/actors/ovl_En_Test3/z_en_test3.h"
}

void drawMask(Actor* kafei) {
    EnTest3* test3 = (EnTest3*)kafei;
    OPEN_DISPS(gPlayState->state.gfxCtx);
    Matrix_Scale(20.0f, 20.0f, 20.0f, MTXMODE_APPLY);
    Vec3s rot;
    rot.x = 19737;
    rot.y = -16100;
    rot.z = 13674;
    Vec3f pos;
    pos.x = 40.0f;
    pos.y = -25.0f;
    pos.z = 0.0f;
    Matrix_TranslateRotateZYX(&pos, &rot);
    auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_KAFEIS_HIDEOUT_KEATON_MASK];
    Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, RC_KAFEIS_HIDEOUT_KEATON_MASK),
                    RC_KAFEIS_HIDEOUT_KEATON_MASK, kafei);
    CLOSE_DISPS(gPlayState->state.gfxCtx);
}

void drawPendant(Actor* kafei) {
    OPEN_DISPS(gPlayState->state.gfxCtx);
    Matrix_Scale(10.0f, 10.0f, 10.0f, MTXMODE_APPLY);
    Vec3s rot;
    rot.x = 19737;
    rot.y = -16100;
    rot.z = 26000;
    Vec3f pos;
    pos.x = 50.0f;
    pos.y = 50.0f;
    pos.z = 0.0f;
    Matrix_TranslateRotateZYX(&pos, &rot);
    auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_KAFEIS_HIDEOUT_PENDANT_OF_MEMORIES];
    Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, RC_KAFEIS_HIDEOUT_PENDANT_OF_MEMORIES),
                    RC_KAFEIS_HIDEOUT_PENDANT_OF_MEMORIES, kafei);
    CLOSE_DISPS(gPlayState->state.gfxCtx);
}

void Rando::ActorBehavior::InitTest3Behavior() {
    COND_VB_SHOULD(VB_DRAW_KAFEI_KEATON_MASK, IS_RANDO, {
        Actor* test3 = va_arg(args, Actor*);
        if (*should) {
            *should = false;
            drawMask(test3);
        }
    });
    COND_VB_SHOULD(VB_DRAW_KAFEI_PENDANT, IS_RANDO, {
        Actor* test3 = va_arg(args, Actor*);
        if (*should) {
            *should = false;
            drawPendant(test3);
        }
    });
}