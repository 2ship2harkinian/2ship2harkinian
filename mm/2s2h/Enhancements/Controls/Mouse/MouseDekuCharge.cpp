#include "2s2h/Mouse.h"

#include "ShipInit.hpp"
#include "GameInteractor/GameInteractor.h"
#include "z64player.h"
#include "functions.h"
#include "macros.h"
#include <libultraship/bridge/consolevariablebridge.h>

#ifdef __cplusplus
extern "C" {
#endif

static void HandleDekuCharge(Player* player) {
    if (!Mouse_IsCaptured()) {
        return;
    }
    MouseCoords d = Mouse_GetDelta();
    if (d.x != 0) {
        player->yaw -= (s16)(d.x * 40 * CVarGetFloat("gEnhancements.Camera.RightStick.CameraSensitivity.X", 1.0f) *
                             GameInteractor_InvertControl(GI_INVERT_CAMERA_RIGHT_STICK_X));
    }
}

void RegisterMouseDekuChargeHooks() {
    COND_HOOK(OnPlayerDekuCharge,
              CVarGetInteger("gSettings.EnableMouse", 0) &&
                  !CVarGetInteger("gEnhancements.Camera.Mouse.DisableThirdPerson", 0),
              HandleDekuCharge);
}

static RegisterShipInitFunc initFunc(RegisterMouseDekuChargeHooks,
                                     { "gSettings.EnableMouse", "gEnhancements.Camera.Mouse.DisableThirdPerson" });

#ifdef __cplusplus
} // extern "C"
#endif
