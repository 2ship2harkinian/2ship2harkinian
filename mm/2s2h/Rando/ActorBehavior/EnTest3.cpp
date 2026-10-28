#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

extern "C" {
#include "functions.h"
#include "variables.h"
#include "overlays/actors/ovl_En_Test3/z_en_test3.h"
#include "objects/gameplay_keep/gameplay_keep.h"
}

static MtxF kafeiHeadMatrix;
static MtxF kafeiHandMatrix;
static bool hasValidHeadMatrix = false;
static bool hasValidHandMatrix = false;

void stopMaskDraw(Gfx** dList1) {
    *dList1 = NULL;
    Matrix_Get(&kafeiHeadMatrix);
    hasValidHeadMatrix = true;
}

void stopMaskDrawInHand(Actor* actor) {
    EnTest3* kafei = (EnTest3*)actor;
    if (BEN_ANIM_EQUAL((const char*)kafei->player.skelAnime.animation, gPlayerAnim_cl_maskoff) ||
        (kafei->player.stateFlags2 & PLAYER_STATE2_1000000)) {
        Matrix_Get(&kafeiHandMatrix);
        hasValidHandMatrix = true;
    }
}

void drawReplacement() {
    if (hasValidHeadMatrix) {
        OPEN_DISPS(gPlayState->state.gfxCtx);
        Matrix_Push();
        Matrix_Put(&kafeiHeadMatrix);
        Matrix_Scale(20.2f, 20.2f, 20.2f, MTXMODE_APPLY);
        Matrix_Translate(50.0f, -23.0f, 0.0f, MTXMODE_APPLY);
        Matrix_RotateZYX(0, 50000, 33500, MTXMODE_APPLY);
        gPlayState->state.gfxCtx->polyOpa.p = POLY_OPA_DISP;
        gPlayState->state.gfxCtx->polyXlu.p = POLY_XLU_DISP;
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP, gPlayState->state.gfxCtx);
        auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_KAFEIS_HIDEOUT_KEATON_MASK];
        auto randoItemId = Rando::ConvertItem(randoSaveCheck.randoItemId, RC_KAFEIS_HIDEOUT_KEATON_MASK);
        Rando::DrawItem(randoItemId, RC_KAFEIS_HIDEOUT_KEATON_MASK);
        Rando::DrawSparklesForReplacement(randoItemId, 3);
        POLY_OPA_DISP = gPlayState->state.gfxCtx->polyOpa.p;
        POLY_XLU_DISP = gPlayState->state.gfxCtx->polyXlu.p;
        Matrix_Pop();
        CLOSE_DISPS(gPlayState->state.gfxCtx);
        hasValidHeadMatrix = false;
    } else if (hasValidHandMatrix) {
        OPEN_DISPS(gPlayState->state.gfxCtx);
        Matrix_Push();
        Matrix_Put(&kafeiHandMatrix);
        Matrix_Scale(20.2f, 20.2f, 20.2f, MTXMODE_APPLY);
        Matrix_Translate(0.0f, 1000.0f, 0.0f, MTXMODE_APPLY);
        Matrix_RotateZYX(0, 0, 0, MTXMODE_APPLY);
        gPlayState->state.gfxCtx->polyOpa.p = POLY_OPA_DISP;
        gPlayState->state.gfxCtx->polyXlu.p = POLY_XLU_DISP;
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP, gPlayState->state.gfxCtx);
        auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_KAFEIS_HIDEOUT_KEATON_MASK];
        auto randoItemId = Rando::ConvertItem(randoSaveCheck.randoItemId, RC_KAFEIS_HIDEOUT_KEATON_MASK);
        Rando::DrawItem(randoItemId, RC_KAFEIS_HIDEOUT_KEATON_MASK);
        Rando::DrawSparklesForReplacement(randoItemId, 3);
        POLY_OPA_DISP = gPlayState->state.gfxCtx->polyOpa.p;
        POLY_XLU_DISP = gPlayState->state.gfxCtx->polyXlu.p;
        Matrix_Pop();
        CLOSE_DISPS(gPlayState->state.gfxCtx);
        hasValidHandMatrix = false;
    }
}

void drawPendant() {
    OPEN_DISPS(gPlayState->state.gfxCtx);
    Matrix_Scale(10.0f, 10.0f, 10.0f, MTXMODE_APPLY);
    Matrix_Translate(50.0f, 50.0f, 0.0f, MTXMODE_APPLY);
    Matrix_RotateZYX(19737, -16100, 26000, MTXMODE_APPLY);
    auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_KAFEIS_HIDEOUT_PENDANT_OF_MEMORIES];
    auto randoItemId = Rando::ConvertItem(randoSaveCheck.randoItemId, RC_KAFEIS_HIDEOUT_PENDANT_OF_MEMORIES);
    Rando::DrawItem(randoItemId, RC_KAFEIS_HIDEOUT_PENDANT_OF_MEMORIES);
    Rando::DrawSparklesForReplacement(randoItemId, 5);
    CLOSE_DISPS(gPlayState->state.gfxCtx);
}

void drawPendantInHand(Actor* enTest3) {
    OPEN_DISPS(gPlayState->state.gfxCtx);
    EnTest3* kafei = (EnTest3*)enTest3;
    Vec3f refPos;
    refPos.x = kafei->player.bodyPartsPos[PLAYER_BODYPART_LEFT_HAND].x;
    refPos.y = kafei->player.bodyPartsPos[PLAYER_BODYPART_LEFT_HAND].y - 6.0f;
    refPos.z = kafei->player.bodyPartsPos[PLAYER_BODYPART_LEFT_HAND].z;
    Matrix_Translate((Math_SinS(kafei->player.actor.shape.rot.y) * 3.3f) + refPos.x, refPos.y + 10.0f,
                     (Math_CosS(kafei->player.actor.shape.rot.y) * 3.3f) + refPos.z, MTXMODE_NEW);
    Matrix_RotateZYX(0, (gPlayState->gameplayFrames * 1000), 0, MTXMODE_APPLY);
    Matrix_Scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);
    auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_KAFEIS_HIDEOUT_PENDANT_OF_MEMORIES];
    auto randoItemId = Rando::ConvertItem(randoSaveCheck.randoItemId, RC_KAFEIS_HIDEOUT_PENDANT_OF_MEMORIES);
    Rando::DrawItem(randoItemId, RC_KAFEIS_HIDEOUT_PENDANT_OF_MEMORIES);
    Rando::DrawSparklesForReplacement(randoItemId, 5);
    CLOSE_DISPS(gPlayState->state.gfxCtx);
}

void Rando::ActorBehavior::InitTest3Behavior() {
    COND_VB_SHOULD(VB_DRAW_KAFEI_KEATON_MASK, IS_RANDO, {
        Gfx** dList1 = va_arg(args, Gfx**);
        if (*should) {
            if (!RANDO_SAVE_CHECKS[RC_KAFEIS_HIDEOUT_KEATON_MASK].shuffled ||
                RANDO_SAVE_CHECKS[RC_KAFEIS_HIDEOUT_KEATON_MASK].cycleObtained) {
                *should = true;
                return;
            }
            *should = false;
            stopMaskDraw(dList1);
        }
    });
    COND_VB_SHOULD(VB_DRAW_KAFEI_PENDANT, IS_RANDO, {
        if (*should) {
            if (!RANDO_SAVE_CHECKS[RC_KAFEIS_HIDEOUT_PENDANT_OF_MEMORIES].shuffled ||
                RANDO_SAVE_CHECKS[RC_KAFEIS_HIDEOUT_PENDANT_OF_MEMORIES].cycleObtained) {
                *should = true;
                return;
            }
            *should = false;
            drawPendant();
        }
    });
    COND_VB_SHOULD(VB_DRAW_KAFEI_KEATON_IN_HAND, IS_RANDO, {
        Actor* test3 = va_arg(args, Actor*);
        if (*should) {
            if (!RANDO_SAVE_CHECKS[RC_KAFEIS_HIDEOUT_KEATON_MASK].shuffled ||
                RANDO_SAVE_CHECKS[RC_KAFEIS_HIDEOUT_KEATON_MASK].cycleObtained) {
                *should = true;
                return;
            }
            *should = false;
            stopMaskDrawInHand(test3);
        }
    });
    COND_VB_SHOULD(VB_DRAW_KAFEI_GET_ITEM, IS_RANDO, {
        Actor* test3 = va_arg(args, Actor*);
        EnTest3* kafei = (EnTest3*)test3;
        if (*should) {
            if ((kafei->player.stateFlags1 & PLAYER_STATE1_400)) {
                *should = true;
                return;
            } else {
                if (!RANDO_SAVE_CHECKS[RC_KAFEIS_HIDEOUT_PENDANT_OF_MEMORIES].shuffled ||
                    RANDO_SAVE_CHECKS[RC_KAFEIS_HIDEOUT_PENDANT_OF_MEMORIES].cycleObtained) {
                    *should = true;
                    return;
                }
                *should = false;
                drawPendantInHand(test3);
            }
        }
    });
    COND_ID_HOOK(OnActorDraw, ACTOR_EN_TEST3, IS_RANDO, [](Actor* actor) { drawReplacement(); });
}