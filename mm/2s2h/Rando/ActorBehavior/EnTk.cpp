#include "ActorBehavior.h"

void Rando::ActorBehavior::InitEnTkBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_TK, IS_RANDO,
                   { *should = !RANDO_SAVE_CHECKS[RC_IKANA_GRAVEYARD_DAMPE_DAY_2_BATS_REWARD].shuffled; });
}