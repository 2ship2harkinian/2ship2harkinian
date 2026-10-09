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

static void HandleTelescopeAim(s16* inputX, s16* inputY) {
    if (!Mouse_IsCaptured()) {
        return;
    }
    MouseCoords d = Mouse_GetDelta();
    *inputX -= (s16)(d.x * 12.0f * CVarGetFloat("gEnhancements.Camera.FirstPerson.GyroSensitivityX", 1.0f));
    *inputY += (s16)(d.y * 12.0f * CVarGetFloat("gEnhancements.Camera.FirstPerson.GyroSensitivityY", 1.0f));
}

static void HandleOvershoulderAim(bool* should, Player* player) {
    if (!Mouse_IsCaptured()) {
        return;
    }
    MouseCoords d = Mouse_GetDelta();
    if (d.y != 0) {
        // FIXME: to remove? Why was it there?
        // player->actor.focus.rot.x += d.y * 8;
        f32 yBuf = d.y * 12.0f * CVarGetFloat("gEnhancements.Camera.FirstPerson.GyroSensitivityY", 1.0f);
        yBuf *= -GameInteractor_InvertControl(GI_INVERT_FIRST_PERSON_GYRO_Y);
        player->actor.focus.rot.x = CLAMP(player->actor.focus.rot.x - (s16)yBuf, -60 * 240, 60 * 240);
        *should = false;
    }
}

void RegisterMouseFirstPersonHooks() {
    COND_HOOK(OnPlayerTelescopeAim,
              CVarGetInteger("gSettings.EnableMouse", 0) &&
                  CVarGetInteger("gEnhancements.Camera.FirstPerson.GyroEnabled", 0),
              HandleTelescopeAim);
    COND_VB_SHOULD(VB_SHOULD_OVERSHOULDER_AIM,
                   CVarGetInteger("gSettings.EnableMouse", 0) &&
                       CVarGetInteger("gEnhancements.Camera.FirstPerson.GyroEnabled", 0),
                   { HandleOvershoulderAim(should, va_arg(args, Player*)); });
}

static RegisterShipInitFunc initFunc(RegisterMouseFirstPersonHooks,
                                     { "gSettings.EnableMouse", "gEnhancements.Camera.FirstPerson.GyroEnabled" });

#ifdef __cplusplus
} // extern "C"
#endif
