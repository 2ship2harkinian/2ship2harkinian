#include "ActorBehavior.h"

void Rando::ActorBehavior::InitEnSthBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* refActor = va_arg(args, Actor*);

        if (refActor->id != ACTOR_EN_STH) {
            return;
        }

        s32 day = CURRENT_DAY - 1;

        RandoCheckId randoCheckId = RC_UNKNOWN;

        if (day == 1) { // GI_RUPEE_PURPLE
            randoCheckId = RC_OCEAN_SPIDER_HOUSE_DAY_2_REWARD;
        } else if (day == 2) { // GI_RUPEE_RED
            randoCheckId = RC_OCEAN_SPIDER_HOUSE_DAY_3_REWARD;
        }

        if (randoCheckId != RC_UNKNOWN) {
            RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];
            // No shuffled validating here for the check, as other rando behavior prevents Oceanside man from giving
            // you the rupees if you've excluded only the Day 2 OR the Day 3 reward check.
            randoSaveCheck.eligible = true;
            // Rando::MiscBehavior::InitOfferGetItemBehavior() handles the rest
        }
    });
}