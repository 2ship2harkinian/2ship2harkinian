#include "Actions.h"

#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {
#include "z64.h"
#include "variables.h"
#include "functions.h"
#include "macros.h"
}

// Instant rather than timed: the bouncing ends when the player lands, not after a fixed duration.
static GIActions::Register knockbackAction({
    .id = GI_ACTION_KNOCKBACK,
    .name = "knockback",
    .displayName = "Knockback",
    .valence = GI_VALENCE_NEGATIVE,
    .schema =
        {
            { .name = "strength", .type = GI_PARAM_FLOAT, .defaultValue = 4.0f, .min = 1.0f, .max = 10.0f },
        },
    .canApply = GIActions::Gates::NotOnHorse,
    .onStart =
        [](GIAction& action) {
            Player* player = GET_PLAYER(gPlayState);
            float strength = action.params.Float("strength");

            func_800B8D98(gPlayState, &player->actor, strength * 5, player->actor.world.rot.y + 0x8000, strength * 5);

            static HOOK_ID knockbackBounceHook = 0;
            GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnActorUpdate>(knockbackBounceHook);
            knockbackBounceHook = GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorUpdate>(
                ACTOR_PLAYER, [](Actor* actor) {
                    Player* player = (Player*)actor;

                    if (player->actor.bgCheckFlags & 0x08 && abs(player->speedXZ) > 15.0f) {
                        player->yaw = ((player->actor.wallYaw - player->yaw) + player->actor.wallYaw) - 0x8000;
                        Player_PlaySfx(player, NA_SE_PL_BODY_HIT);
                    }

                    if (player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                        GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnActorUpdate>(
                            knockbackBounceHook);
                    }
                });
        },
});
