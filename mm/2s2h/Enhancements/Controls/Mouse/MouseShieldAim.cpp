#include "2s2h/Mouse.h"
#include "MouseShieldAim.h"

#include "ShipInit.hpp"
#include "GameInteractor/GameInteractor.h"
#include "z64player.h"
#include "z64camera.h"
#include "functions.h"
#include "macros.h"
#include <libultraship/bridge/consolevariablebridge.h>

static bool mPrevShieldHandled = false;

#ifdef __cplusplus
extern "C" {
#endif

static void HandleShieldAim(Player* player, PlayState* play, f32* xStick, f32* yStick, bool* handled) {
    if (!Mouse_IsCaptured()) {
        *handled = mPrevShieldHandled = false;
        return;
    }

    MouseCoords mouseDelta = Mouse_GetDelta();
    bool hasDelta = (mouseDelta.x != 0 || mouseDelta.y != 0);

    if (!hasDelta) {
        *handled = mPrevShieldHandled = (mPrevShieldHandled && *xStick == 0 && *yStick == 0);
        return;
    }

    bool hasFocusActor = (player->focusActor != NULL);
    bool camRotate = CVarGetInteger("gEnhancements.Equipment.MouseShielding.CameraControl", 1) && !hasFocusActor;

    if (camRotate) {
        // Hook shield to camera view
        Camera* camera = GET_ACTIVE_CAM(play);
        VecGeo viewOffset = OLib_Vec3fDiffToVecGeo(&camera->at, &camera->eye);
        s16 camYaw = viewOffset.yaw + 0x8000;
        s16 camPitch = viewOffset.pitch;

        player->actor.shape.rot.y = camYaw;
        player->yaw = camYaw;
        player->upperLimbRot.y = 0;
        player->upperLimbRot.x = camPitch;
    } else {
        // Plain aim
        f32 xDelta = ((f32)mouseDelta.x) * 60 *
                     CVarGetFloat("gEnhancements.Camera.FirstPerson.GyroSensitivityX", 1.0f) *
                     GameInteractor_InvertControl(GI_INVERT_SHIELD_X);
        f32 yDelta = -((f32)mouseDelta.y) * 60 *
                     CVarGetFloat("gEnhancements.Camera.FirstPerson.GyroSensitivityY", 1.0f) *
                     GameInteractor_InvertControl(GI_INVERT_SHIELD_Y);

        s16 rotYTarget = CLAMP(player->upperLimbRot.y + (s16)xDelta, -60 * 120, 60 * 120);
        s16 rotXTarget = CLAMP(player->upperLimbRot.x + (s16)yDelta, -60 * 180, 0xDAC);

        player->upperLimbRot.y = rotYTarget;
        player->upperLimbRot.x = rotXTarget;
    }
    *handled = mPrevShieldHandled = true;
}

void HandleShieldCameraControl(Camera* camera, s16 viewYaw) {
    if (!mPrevShieldHandled || !CVarGetInteger("gEnhancements.Equipment.MouseShielding.CameraControl", 1)) {
        return;
    }

    f32 shoulderOffset = CVarGetFloat("gEnhancements.Mouse.Shielding.ShoulderOffset", -12.0f);
    VecGeo lateral = { .r = shoulderOffset, .pitch = 0, .yaw = viewYaw + 0x4000 };
    Vec3f offset = OLib_VecGeoToVec3f(&lateral);

    camera->at.x += offset.x;
    camera->at.z += offset.z;
}

void RegisterMouseShieldHooks() {
    COND_HOOK(OnPlayerShieldControl,
              CVarGetInteger("gSettings.EnableMouse", 0) &&
                  CVarGetInteger("gEnhancements.Equipment.MouseShielding.Enable", 0),
              HandleShieldAim);
}

static RegisterShipInitFunc initFunc(RegisterMouseShieldHooks,
                                     { "gSettings.EnableMouse", "gEnhancements.Equipment.MouseShielding.Enable" });

#ifdef __cplusplus
} // extern "C"
#endif
