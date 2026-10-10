#include "Actions.h"

#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {
#include "z64.h"
#include "variables.h"
#include "macros.h"
// Not in any header, but it has external linkage in z_player.c.
extern FloorType sPlayerFloorType;
}

static HOOK_ID sFloorHook = 0;

// Ice is a branch keyed on the floor under Link being FLOOR_TYPE_5; Player_ProcessSceneCollision
// restores the true floor later in the same update. Written from ShouldActorUpdate rather than
// onTick, which runs after the player updates -- too late for a value that doesn't survive the frame.
static GIActions::Register slipperyFloorAction({
    .id = GI_ACTION_SLIPPERY_FLOOR,
    .name = "slipperyFloor",
    .displayName = "Slippery Floor",
    .valence = GI_VALENCE_NEGATIVE,
    .defaultDuration = 20 * 30, // 30 seconds
    .stacking = GI_STACK_REFRESH,
    // On a horse the player skips the ice branch entirely, so this would silently do nothing.
    .canApply = GIActions::Gates::NotOnHorse,
    .onStart =
        [](GIAction& action) {
            GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldActorUpdate>(sFloorHook);
            sFloorHook = GameInteractor::Instance->RegisterGameHookForID<GameInteractor::ShouldActorUpdate>(
                ACTOR_PLAYER, [](Actor* actor, bool* should) { sPlayerFloorType = FLOOR_TYPE_5; });
        },
    .onEnd =
        [](GIAction& action) {
            GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldActorUpdate>(sFloorHook);
            sFloorHook = 0;
        },
});
