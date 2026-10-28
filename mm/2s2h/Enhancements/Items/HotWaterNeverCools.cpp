#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.Items.HotWaterNeverCools"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterHotWaterNeverCools() {
    COND_VB_SHOULD(VB_COOL_HOT_WATER, CVAR, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterHotWaterNeverCools, { CVAR_NAME });
