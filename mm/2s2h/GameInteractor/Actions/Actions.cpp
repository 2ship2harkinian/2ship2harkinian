#include "Actions.h"

#include <libultraship/bridge/consolevariablebridge.h>
#include <spdlog/spdlog.h>

#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {
#include "z64.h"
#include "variables.h"
#include "macros.h"
// Has external linkage in z_camera.c, but isn't in any header.
Vec3f Camera_CalcUpVec(s16 pitch, s16 yaw, s16 roll);
}

namespace GIActions {

// Function-local static so the registry exists before any Register in another TU runs.
static std::vector<Definition>& Registry() {
    static std::vector<Definition> registry;
    return registry;
}

Register::Register(Definition definition) {
    Registry().push_back(std::move(definition));
}

const std::vector<Definition>& All() {
    return Registry();
}

const Definition* Get(GIActionId id) {
    for (const auto& definition : Registry()) {
        if (definition.id == id) {
            return &definition;
        }
    }
    return nullptr;
}

const Definition* FindByName(std::string_view name) {
    for (const auto& definition : Registry()) {
        if (name == definition.name) {
            return &definition;
        }
    }
    return nullptr;
}

std::optional<GIAction> Definition::Build(GIParams params, std::string* error) const {
    if (auto problem = GIParamsValidate(schema, params)) {
        if (error != nullptr) {
            *error = *problem;
        }
        SPDLOG_WARN("[GameInteractor] Rejected '{}': {}", name, *problem);
        return std::nullopt;
    }

    return GIAction{
        .id = id,
        .params = std::move(params),
        .duration = defaultDuration,
        .canApply = canApply,
        .onStart = onStart,
        .onTick = onTick,
        .onEnd = onEnd,
    };
}

GIActionAvailability Gates::NotOnHorse(const GIAction&) {
    if (gPlayState == NULL) {
        return GI_AVAILABILITY_NOT_YET;
    }
    if (GET_PLAYER(gPlayState)->stateFlags1 & PLAYER_STATE1_800000) {
        return GI_AVAILABILITY_NOT_YET;
    }
    return GI_AVAILABILITY_READY;
}

Player* PlayerOrNull() {
    if (gPlayState == NULL) {
        return NULL;
    }
    return GET_PLAYER(gPlayState);
}

int32_t Setting::Snapshot(const char* cvar) {
    return CVarGetInteger(cvar, Setting::ABSENT);
}

void Setting::Restore(const char* cvar, int32_t previous) {
    if (previous == Setting::ABSENT) {
        CVarClear(cvar);
    } else {
        CVarSetInteger(cvar, previous);
    }
}

void CameraRoll::Apply(Camera* camera, int16_t offset) {
    if (camera == NULL || camera->camId != CAM_ID_MAIN) {
        return;
    }

    // Recomputed from camDir each frame; rotating camera->up in place would compound the offset.
    camera->up = Camera_CalcUpVec(camera->camDir.x, camera->camDir.y, camera->roll + offset);
    camera->viewFlags |= CAM_VIEW_UP;
}

// File statics because REGISTER_VB_SHOULD expands to a captureless lambda.
static float sHeightScale = 1.0f;
static HOOK_ID sFocalHeightHook = 0;
static HOOK_ID sLegIkHook = 0;

// Camera_Normal1 divides by the focal height, so it must never reach zero.
#define MIN_FOCAL_HEIGHT 1.0f

static void InstallScaleCompensation() {
    if (sFocalHeightHook != 0) {
        return;
    }

    // The follow cam frames against a fixed per-form height constant, so scale it too or a resized
    // player is framed from the normal distance.
    sFocalHeightHook = REGISTER_VB_SHOULD(VB_MODIFY_CAMERA_FOCAL_HEIGHT, {
        f32* height = va_arg(args, f32*);

        *height *= sHeightScale;
        if (*height < MIN_FOCAL_HEIGHT) {
            *height = MIN_FOCAL_HEIGHT;
        }
    });

    // The leg IK solves against limb lengths baked for a normal-sized player and breaks at any
    // other scale, so skip it; all it does is plant the feet on slopes.
    sLegIkHook = REGISTER_VB_SHOULD(VB_NOT_ADJUST_PLAYER_LEGS, { *should = true; });
}

static void RemoveScaleCompensation() {
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(sFocalHeightHook);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(sLegIkHook);
    sFocalHeightHook = 0;
    sLegIkHook = 0;
}

void PlayerScale::Set(float x, float y, float z) {
    // The camera frames against height, so y is the axis it follows.
    sHeightScale = y;
    InstallScaleCompensation();

    Player* player = PlayerOrNull();
    if (player == NULL) {
        return;
    }

    player->actor.scale.x = BASE * x;
    player->actor.scale.y = BASE * y;
    player->actor.scale.z = BASE * z;
}

void PlayerScale::Reset() {
    RemoveScaleCompensation();
    sHeightScale = 1.0f;

    Player* player = PlayerOrNull();
    if (player == NULL) {
        return;
    }

    player->actor.scale.x = BASE;
    player->actor.scale.y = BASE;
    player->actor.scale.z = BASE;
}

} // namespace GIActions
