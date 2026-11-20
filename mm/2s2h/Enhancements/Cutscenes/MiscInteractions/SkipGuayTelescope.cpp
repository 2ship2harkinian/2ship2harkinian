#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"

#include "overlays/actors/ovl_En_Scopecrow/z_en_scopecrow.h"
void func_80BCD640(EnScopecrow* enScopecrow, PlayState* play);
void func_80BCD2BC(EnScopecrow* enScopecrow, PlayState* play);
s32 func_80BCD1AC(s16 arg0);
f32 func_80BCD4D0(Path* path, s32 count, Vec3f* arg2, Vec3s* arg3);
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipMiscInteractions"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

// This is the same as the regular pathing function except a special case was added for skipping most of the path.
s32 modified_EnScopecrow_HasReachedPoint(EnScopecrow* enScopecrow, Path* path, s32 pointIndex) {
    Vec3s* points = path->points;
    s32 count = path->count;
    s32 index = pointIndex;
    s32 reached = false;
    f32 diffX;
    f32 diffZ;
    f32 px;
    f32 pz;
    f32 d;
    Vec3f point;

    Math_Vec3s_ToVec3f(&point, &points[index]);

    if (index == 0) {
        diffX = points[1].x - points[0].x;
        diffZ = points[1].z - points[0].z;
    } else if (index == enScopecrow->unk_262) { // Custom diff for skipping most of the flight.
        diffX = points[index].x - points[1].x;
        diffZ = points[index].z - points[1].z;
    } else if (index == (count - 1)) {
        diffX = points[count - 1].x - points[count - 2].x;
        diffZ = points[count - 1].z - points[count - 2].z;
    } else {
        diffX = points[index + 1].x - points[index - 1].x;
        diffZ = points[index + 1].z - points[index - 1].z;
    }

    Math3D_RotateXZPlane(&point, RAD_TO_BINANG(Math_FAtan2F(diffX, diffZ)), &px, &pz, &d);

    if (((px * enScopecrow->actor.world.pos.x) + (pz * enScopecrow->actor.world.pos.z) + d) > 0.0f) {
        reached = true;
    }

    return reached;
}

// This is the same as the regular pathing function except sped up.
void modified_func_80BCD640(EnScopecrow* enScopecrow, PlayState* play) {
    Vec3s sp30;

    if (enScopecrow->path != NULL) {
        func_80BCD4D0(enScopecrow->path, enScopecrow->unk_1FC, &enScopecrow->actor.world.pos, &sp30);
        if (enScopecrow->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            sp30.y = enScopecrow->actor.wallYaw;
        }

        Math_SmoothStepToS(&enScopecrow->actor.world.rot.y, sp30.y, 4, 0x3E8, 1);
        enScopecrow->actor.shape.rot.y = enScopecrow->actor.world.rot.y;
        Math_SmoothStepToS(&enScopecrow->actor.world.rot.x, -sp30.x, 4, 0x3E8, 1);

        // Handle skipping to the drop point.
        if (modified_EnScopecrow_HasReachedPoint(enScopecrow, enScopecrow->path, enScopecrow->unk_1FC)) {
            if ((enScopecrow->unk_1FC == enScopecrow->unk_262) && func_80BCD1AC(enScopecrow->unk_260)) {
                func_80BCD2BC(enScopecrow, play);
            }

            if (enScopecrow->unk_1FC >= (enScopecrow->path->count - 1)) {
                Actor_Kill(&enScopecrow->actor);
            } else {
                enScopecrow->unk_1FC++;
            }
        }
    }

    // Speed up the entire flight.
    Math_ApproachF(&enScopecrow->actor.speed, 20.0f, 0.2f, 1.0f);
    Actor_MoveWithoutGravity(&enScopecrow->actor);
    enScopecrow->unk_264 += 0x1000;
    enScopecrow->actor.shape.yOffset = Math_SinS(enScopecrow->unk_264) * 500.0f;
}

void RegisterSkipGuayTelescope() {
    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_SCOPECROW, CVAR, [](Actor* actor) {
        EnScopecrow* enScopecrow = (EnScopecrow*)actor;

        if (enScopecrow->actionFunc == func_80BCD640 && enScopecrow->unk_1FC == 1) {
            enScopecrow->actionFunc = modified_func_80BCD640;
            // Skip to pathing towards the drop point.
            enScopecrow->unk_1FC = enScopecrow->unk_262;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipGuayTelescope, { CVAR_NAME });