#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h_assets.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Pm/z_en_pm.h"
#include "assets/objects/object_gi_mask13/object_gi_mask13.h"
void Player_SetupTalk(PlayState* play, Player* player);
s32 Player_SetupWaitForPutAway(PlayState* play, Player* player, AfterPutAwayFunc afterPutAwayFunc);
}

void drawRandoPostmanHat(Gfx** gfx) {
    gPlayState->state.gfxCtx->polyOpa.p = *gfx;
    Matrix_Push();
    auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_CLOCK_TOWN_EAST_POSTMAN_HAT];
    Matrix_Scale(21.0f, 21.0f, 21.0f, MTXMODE_APPLY);
    Vec3f pos;
    pos.x = 70.0;
    pos.y = 20.0;
    pos.z = 0.0;
    Vec3s rot;
    rot.x = 63000;
    rot.y = 47000;
    rot.z = 0xc000;
    Matrix_TranslateRotateZYX(&pos, &rot);
    auto randoItemId = Rando::ConvertItem(randoSaveCheck.randoItemId, RC_CLOCK_TOWN_EAST_POSTMAN_HAT);
    Rando::DrawItem(randoItemId, RC_CLOCK_TOWN_EAST_POSTMAN_HAT);
    Vec3f sparklePos;
    Matrix_MultZero(&sparklePos);
    sparklePos.y -= 3;
    Rando::DrawSparklesAtPos(randoItemId, &sparklePos);
    Matrix_Pop();
    *gfx = gPlayState->state.gfxCtx->polyOpa.p;
}

void Rando::ActorBehavior::InitEnPmBehavior() {
    COND_VB_SHOULD(VB_EXEC_MSG_EVENT, IS_RANDO, {
        u32 cmdId = va_arg(args, u32);
        Actor* actor = va_arg(args, Actor*);
        if (actor->id == ACTOR_EN_PM) { // Postman
            MsgScript* script = va_arg(args, MsgScript*);
            Player* player = GET_PLAYER(gPlayState);
            switch (cmdId) {
                case MSCRIPT_CMD_ID_OFFER_ITEM:
                    // Lock the player into conversation because a notebook message might appear
                    Player_SetupWaitForPutAway(gPlayState, player, Player_SetupTalk);
                    *should = false;
                    break;
                case MSCRIPT_CMD_ID_CHECK_ITEM: {
                    /*
                     * The Postman's Hat check does a branch if the player already has the Postman's Hat in their
                     * inventory. In rando, it is possible to already have the Postman's Hat before ever giving him the
                     * Express Mail, so we're just going to always act as if the player does not have the Postman's Hat.
                     * This enables repeat rewards as well, which does slightly differ from vanilla.
                     */
                    MsgScriptCmdCheckItem* cmd = (MsgScriptCmdCheckItem*)script;
                    ItemId itemId = (ItemId)SCRIPT_PACK_16(cmd->itemH, cmd->itemL);
                    if (itemId == ITEM_MASK_POSTMAN) {
                        *should = false;
                    }
                } break;
                case MSCRIPT_CMD_ID_DONE: // MSCRIPT_DONE
                    // Prevent softlocks in case a notebook message did not appear
                    Message_CloseTextbox(gPlayState);
                    break;
                default:
                    break;
            }
        }
    });

    COND_VB_SHOULD(VB_DRAW_POSTMAN_HAT, IS_RANDO, {
        Gfx** gfx = va_arg(args, Gfx**);
        if (*should) {
            *should = false;
            drawRandoPostmanHat(gfx);
        }
    });
}
