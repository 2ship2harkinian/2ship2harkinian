#include "ActorBehavior.h"
#include "2s2h/ObjectExtension/ActorListIndex.h"
#include <spdlog/spdlog.h>

extern "C" {
#include "variables.h"
#include "functions.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"
}

#define CUSTOM_PARAM (enElf->actor.home.rot.x)

std::map<std::tuple<s16, s16, s16>, RandoCheckId> enElfItemMap = {

};

void EnElf_DrawCustom(Actor* thisx, PlayState* play) {
    EnElf* enElf = (EnElf*)thisx;
    Matrix_Scale(20.0f, 20.0f, 20.0f, MTXMODE_APPLY);

    if (CUSTOM_PARAM == RC_UNKNOWN) {
        return;
    }

    auto randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_PARAM];

    // thisx->shape.rot.y = thisx->shape.rot.y + 960;

    Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, (RandoCheckId)CUSTOM_PARAM),
                    (RandoCheckId)CUSTOM_PARAM, thisx);
}

void EnElf_Setup(EnElf* enElf) {
    auto it =
        enElfItemMap.find({ gPlayState->sceneId, gPlayState->roomCtx.curRoom.num, GetActorListIndex((Actor*)enElf) });
    if (it != enElfItemMap.end() && RANDO_SAVE_CHECKS[it->second].shuffled) {
        CUSTOM_PARAM = it->second;
        enElf->actor.draw = EnElf_DrawCustom;
    } else {
        SPDLOG_DEBUG("{} {} {}", gPlayState->sceneId, gPlayState->roomCtx.curRoom.num,
                     GetActorListIndex((Actor*)enElf));
    }
}

void Rando::ActorBehavior::InitEnElfBehavior() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_ELF, IS_RANDO, [](Actor* actor) { EnElf_Setup((EnElf*)actor); });

    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_ELF, IS_RANDO, {
        *should = false;
        EnElf* enElf = va_arg(args, EnElf*);
        RandoCheckId randoCheckId = (RandoCheckId)CUSTOM_PARAM;
        if (RANDO_SAVE_CHECKS[randoCheckId].shuffled && !RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
            RANDO_SAVE_CHECKS[randoCheckId].eligible = true;
        }
    });
}