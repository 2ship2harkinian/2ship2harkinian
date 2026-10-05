#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/Rando/Logic/Logic.h"

extern "C" {
#include "variables.h"
#include "functions.h"

#include "include/z64player.h"
extern s32 Player_SetAction(PlayState* play, Player* player, PlayerActionFunc actionFunc, s32 arg3);
extern void Player_Action_1(Player* player, PlayState* play);
extern s32 Player_PutAwayHeldItem(PlayState* play, Player* player);
}

static u8 lastOcarinaButton = OCARINA_BTN_INVALID;

void RespawnOnWaterTouch(Player* player) {
    // This is Honey & Darlings Shop, touching the water ends the minigame as its vanilla behavior.
    // No reason to handle it a second time here.
    if (gPlayState->sceneId == SCENE_BOWLING) {
        return;
    }

    if (player->stateFlags1 & PLAYER_STATE1_8000000) {
        // Mimic Deku Hop failure behavior
        Player_SetAction(gPlayState, player, Player_Action_1, 0);
        player->stateFlags1 |= PLAYER_STATE1_20000000;
    }
}

void PreventGrab(Player* player) {
    // This prevents picking actors up like Bushes, Rocks, Pots, etc. if
    // The ability to pickup things has not yet been found.

    /*
     *  List of Actors that are being checked for:
     *   - ACTOR_EN_KUSA        (Grass Bushes)
     *   - ACTOR_EN_KUSA2       (Keaton Grass Bushes)
     *   - ACTOR_EN_ISHI        (Small rocks)
     *   - ACTOR_EN_BOMBF       (Bomb Flowers)
     *   - ACTOR_OBJ_TSUBO      (Pots)
     *   - ACTOR_OBJ_KIBAKO     (Small Crates)
     *   - ACTOR_OBJ_SNOWBALL2  (Small Snowballs)
     *
     *  Due to many grass actors on Termina Field and other areas being a special one, ACTOR_OBJ_GRASS_CARRY is also
     * included. Not including this makes the grass itself be grabbable again, which defeats the purpose of this
     * addition.
     */
    ActorId actorsToRestrict[8] = { ACTOR_EN_KUSA,   ACTOR_EN_KUSA2,   ACTOR_EN_ISHI,       ACTOR_EN_BOMBF,
                                    ACTOR_OBJ_TSUBO, ACTOR_OBJ_KIBAKO, ACTOR_OBJ_SNOWBALL2, ACTOR_OBJ_GRASS_CARRY };

    // We allow bombs/powder kegs/bombchus to be grabbable since they don't appear naturally anywhere in the world
    // but rather are actively used items instead.
    if (player->interactRangeActor != NULL &&
        !(player->interactRangeActor->id == ACTOR_EN_BOM || player->interactRangeActor->id == ACTOR_EN_BOM_CHU)) {
        // Check if the player is already carrying something, like a sword or similar
        if (player->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
            // Put the held item away, otherwise we bypass the grab restriction
            // Also less elegant, because player has no visual or audio feedback that the held item
            // has been put away.
            Player_PutAwayHeldItem(gPlayState, player);
        }

        Actor* interactActor = player->interactRangeActor;

        if (std::ranges::find(actorsToRestrict, interactActor->id) != std::end(actorsToRestrict)) {
            interactActor = NULL;
            player->interactRangeActor = interactActor;
            player->stateFlags1 &= ~PLAYER_STATE1_CARRYING_ACTOR;
        }
    }
}

void Rando::ActorBehavior::InitPlayerBehavior() {
    COND_ID_HOOK(OnActorUpdate, ACTOR_PLAYER, IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_GRAB], [](Actor* actor) {
        Player* player = GET_PLAYER(gPlayState);
        if (!Flags_GetRandoInf(RANDO_INF_OBTAINED_GRAB)) {
            PreventGrab(player);
        }
    });

    COND_ID_HOOK(OnActorUpdate, ACTOR_PLAYER, IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_SWIM], [](Actor* actor) {
        Player* player = GET_PLAYER(gPlayState);
        if (!Flags_GetRandoInf(RANDO_INF_OBTAINED_SWIM)) {
            RespawnOnWaterTouch(player);
        }
    });

    COND_VB_SHOULD(VB_PLAY_OCARINA_NOTE, IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_OCARINA_BUTTONS], {
        u8* sCurOcarinaButtonIndex = va_arg(args, u8*);
        u8* sCurOcarinaPitch = va_arg(args, u8*);
        u8 currentOcarinaButton = *sCurOcarinaButtonIndex;

        if (!Flags_GetRandoInf(((*sCurOcarinaButtonIndex) - OCARINA_BTN_A) + RANDO_INF_OBTAINED_OCARINA_BUTTON_A)) {
            *sCurOcarinaButtonIndex = OCARINA_BTN_INVALID;
            *sCurOcarinaPitch = OCARINA_PITCH_NONE;
            if (lastOcarinaButton != currentOcarinaButton) {
                lastOcarinaButton = currentOcarinaButton;
                if (currentOcarinaButton != OCARINA_BTN_INVALID) {
                    Audio_PlaySfx(NA_SE_SY_OCARINA_ERROR);
                }
            }
        }
    });

    COND_VB_SHOULD(VB_SONG_AVAILABLE_TO_PLAY, IS_RANDO, {
        uint8_t* songIndex = va_arg(args, uint8_t*);

        if (*songIndex == OCARINA_SONG_SUNS && RANDO_SAVE_OPTIONS[RO_SHUFFLE_SONG_SUN]) {
            if (CHECK_QUEST_ITEM(QUEST_SONG_SUN)) {
                *should = true;
            } else {
                *should = false;
            }
        }

        if (*songIndex == OCARINA_SONG_DOUBLE_TIME && RANDO_SAVE_OPTIONS[RO_SHUFFLE_SONG_DOUBLE_TIME]) {
            if (Flags_GetRandoInf(RANDO_INF_OBTAINED_SONG_DOUBLE_TIME)) {
                *should = true;
            } else {
                *should = false;
            }
        }

        if (*songIndex == OCARINA_SONG_INVERTED_TIME && RANDO_SAVE_OPTIONS[RO_SHUFFLE_SONG_INVERTED_TIME]) {
            if (Flags_GetRandoInf(RANDO_INF_OBTAINED_SONG_INVERTED_TIME)) {
                *should = true;
            } else {
                *should = false;
            }
        }
    });
}
