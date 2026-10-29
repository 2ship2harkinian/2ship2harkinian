#include "ActorBehavior.h"

void Rando::ActorBehavior::InitEnHsBehavior() {
    COND_VB_SHOULD(VB_HAVE_BUNNY_HOOD, IS_RANDO, {
        RandoSaveCheck cuccoCheck = RANDO_SAVE_CHECKS[RC_ROMANI_RANCH_GROG];
        // either getting the check now or have already gotten it
        *should = cuccoCheck.eligible || cuccoCheck.cycleObtained;
    });
}