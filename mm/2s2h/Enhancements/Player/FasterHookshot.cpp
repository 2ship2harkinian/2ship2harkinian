#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_Arms_Hook/z_arms_hook.h"
}

#define CVAR_NAME "gEnhancements.Player.FasterHookshot"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

#define MULTIPLIER 2.0

static RegisterShipInitFunc initFunc(
    []() {
        COND_VB_SHOULD(VB_HOOKSHOT_SHOOT, CVAR, {
            ArmsHook* armsHook = va_arg(args, ArmsHook*);
            Actor_SetSpeeds(&armsHook->actor, 20.0f * MULTIPLIER);
            armsHook->timer /= MULTIPLIER;
        });

        COND_VB_SHOULD(VB_HOOKSHOT_SET_SPEED, CVAR, {
            f32* velocity = va_arg(args, f32*);
            *velocity *= MULTIPLIER;
        });
    },
    { CVAR_NAME });
