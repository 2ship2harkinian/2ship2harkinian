#include "ActorBehavior.h"

// clang-format off
std::map<GetItemId, RandoCheckId> pictoContestRewardsMap = {
    { GI_HEART_PIECE, RC_TOURIST_INFORMATION_GOOD_PHOTO },
    { GI_RUPEE_SILVER, RC_TOURIST_INFORMATION_GOOD_PHOTO },
    { GI_RUPEE_RED, RC_TOURIST_INFORMATION_OKAY_PHOTO },
    { GI_RUPEE_BLUE, RC_TOURIST_INFORMATION_BAD_PHOTO },
};
// clang-format on

void Rando::ActorBehavior::InitEnShnBehavior() {

    COND_VB_SHOULD(VB_EXEC_MSG_EVENT, IS_RANDO, {
        u32 cmdId = va_arg(args, u32);
        Actor* actor = va_arg(args, Actor*);

        if (actor->id == ACTOR_EN_SHN && cmdId == MSCRIPT_CMD_ID_OFFER_ITEM) { // Swamp Tourist Center Guide
            MsgScriptCmdOfferItem* cmd = va_arg(args, MsgScriptCmdOfferItem*);
            GetItemId getItemId = (GetItemId)SCRIPT_PACK_16(cmd->itemIdH, cmd->itemIdL);
            auto it = pictoContestRewardsMap.find(getItemId);
            if (it != pictoContestRewardsMap.end()) {
                RandoSaveCheck randoSaveCheck = RANDO_SAVE_CHECKS[it->second];
                *should = !randoSaveCheck.shuffled || randoSaveCheck.cycleObtained;
            }
        }
    });
}
