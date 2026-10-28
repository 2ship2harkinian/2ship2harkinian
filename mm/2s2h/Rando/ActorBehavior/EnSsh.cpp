#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipUtils.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "overlays/actors/ovl_En_Sth/z_en_sth.h"
}

void ApplySwampSpiderHouseHint(u16* textId, bool* loadFromMessageTable) {
    CustomMessage::Entry entry = {
        .msg = "Make me...normal again...I'll give you %g{{item}}%w...Please...help me...\xE0",
    };

    CustomMessage::Replace(
        &entry.msg, "{{item}}",
        Rando::StaticData::GetItemName(RANDO_SAVE_CHECKS[RC_SWAMP_SPIDER_HOUSE_MASK_OF_TRUTH].randoItemId, true,
                                       RC_SWAMP_SPIDER_HOUSE_MASK_OF_TRUTH));

    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
}

void ApplyOceanSpiderHouseHint(u16* textId, bool* loadFromMessageTable) {
    CustomMessage::Entry entry = {
        .msg = "Huh? How'd I get up here... Why do I have %g{{item}}%w in my pocket...?\xE0",
    };

    CustomMessage::Replace(&entry.msg, "{{item}}",
                           Rando::StaticData::GetItemName(RANDO_SAVE_CHECKS[RC_OCEAN_SPIDER_HOUSE_WALLET].randoItemId,
                                                          true, RC_OCEAN_SPIDER_HOUSE_WALLET));

    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
}

void drawMaskOfTruth(Vec3f* pos, Vec3s* rot, f32 scale) {
    OPEN_DISPS(gPlayState->state.gfxCtx);
    Matrix_Push();
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    Matrix_TranslateRotateZYX(pos, rot);
    auto randoSaveCheck = RANDO_SAVE_CHECKS[RC_SWAMP_SPIDER_HOUSE_MASK_OF_TRUTH];
    auto randoItemId = Rando::ConvertItem(randoSaveCheck.randoItemId, RC_SWAMP_SPIDER_HOUSE_MASK_OF_TRUTH);
    Rando::DrawItem(randoItemId, RC_SWAMP_SPIDER_HOUSE_MASK_OF_TRUTH);
    Rando::DrawSparklesForReplacement(randoItemId, 3);
    Matrix_Pop();
    CLOSE_DISPS(gPlayState->state.gfxCtx);
}

void Rando::ActorBehavior::InitEnSshBehavior() {
    bool shouldRegister = IS_RANDO && RANDO_SAVE_OPTIONS[RO_HINTS_SPIDER_HOUSES];

    // "Recruiting Soldiers..." Posters around Clock Town
    COND_ID_HOOK(OnOpenText, 0x915, shouldRegister, ApplySwampSpiderHouseHint);
    COND_ID_HOOK(OnOpenText, 0x1130, shouldRegister, ApplyOceanSpiderHouseHint);
    COND_ID_HOOK(OnOpenText, 0x1131, shouldRegister, ApplyOceanSpiderHouseHint);

    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_SSH, IS_RANDO, [](Actor* actor, bool* should) {
        // Skip first dialog
        SET_WEEKEVENTREG(WEEKEVENTREG_TALKED_SWAMP_SPIDER_HOUSE_MAN);
    });

    // Use RO setting for tokens required
    COND_VB_SHOULD(VB_HAVE_ALL_SKULLTULA_TOKENS, IS_RANDO, {
        /*
         * Note that the use case for determining whether to spawn the squatter in South Clock Town directly checks the
         * skullTokenCount value with the Oceanside bitwise operation, rather than call Inventory_GetSkullTokenCount
         * with a scene ID. Inventory_GetSkullTokenCount only specially checks for the Swamp Spider House scene, with
         * Oceanside as the else. South Clock Town is not the Swamp Spider House, so it will still return the Oceanside
         * token count as expected.
         */
        *should = Inventory_GetSkullTokenCount(gPlayState->sceneId) >= RANDO_SAVE_OPTIONS[RO_SKULLTULA_TOKENS_REQUIRED];
    });

    COND_VB_SHOULD(VB_NOT_HAVE_ALL_SKULLTULA_TOKENS, IS_RANDO, {
        *should = Inventory_GetSkullTokenCount(gPlayState->sceneId) < RANDO_SAVE_OPTIONS[RO_SKULLTULA_TOKENS_REQUIRED];
    });

    COND_VB_SHOULD(VB_DRAW_MASK_OF_TRUTH_SPIDER, IS_RANDO, {
        if (*should) {
            if (!RANDO_SAVE_CHECKS[RC_SWAMP_SPIDER_HOUSE_MASK_OF_TRUTH].shuffled ||
                RANDO_SAVE_CHECKS[RC_SWAMP_SPIDER_HOUSE_MASK_OF_TRUTH].cycleObtained) {
                *should = true;
                return;
            }
            *should = false;
            Vec3s rot;
            rot.x = 20000;
            rot.y = -13000;
            rot.z = -1000;
            Vec3f pos;
            pos.x = 20.0f;
            pos.y = -50.0f;
            pos.z = 10.0f;
            drawMaskOfTruth(&pos, &rot, 10.0f);
        }
    });

    COND_VB_SHOULD(VB_DRAW_MASK_OF_TRUTH_HUMAN, IS_RANDO, {
        EnSth* enSth = va_arg(args, EnSth*);
        if (enSth->sthFlags & STH_FLAG_DRAW_MASK_OF_TRUTH) {
            if (*should) {
                if (!RANDO_SAVE_CHECKS[RC_SWAMP_SPIDER_HOUSE_MASK_OF_TRUTH].shuffled ||
                    RANDO_SAVE_CHECKS[RC_SWAMP_SPIDER_HOUSE_MASK_OF_TRUTH].cycleObtained) {
                    *should = true;
                    return;
                }
                *should = false;
                Vec3s rot;
                rot.x = -5000;
                rot.y = -16000;
                rot.z = -11000;
                Vec3f pos;
                pos.x = 20.0f;
                pos.y = 36.0f;
                pos.z = 0.0f;
                drawMaskOfTruth(&pos, &rot, 20.0f);
            }
        }
    });
}
