#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "functions.h"
}

void Rando::ActorBehavior::InitEnScopecrowBehavior() {
    COND_VB_SHOULD(VB_SCOPECROW_FORCE_DROP, IS_RANDO, {
        s16 arg0 = va_arg(args, s16);
        RandoSaveCheck northCheck = RANDO_SAVE_CHECKS[RC_TERMINA_FIELD_TELESCOPE_NORTH_GUAY];
        RandoSaveCheck southCheck = RANDO_SAVE_CHECKS[RC_TERMINA_FIELD_TELESCOPE_SOUTH_GUAY];
        if (arg0 == 2 &&
            (northCheck.shuffled && !northCheck.cycleObtained || southCheck.shuffled && !southCheck.cycleObtained)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_54_01);
            *should = true;
        }
    });
}