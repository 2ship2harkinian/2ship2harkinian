#include "2s2h/Network/Anchor/Anchor.h"
#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>
#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {
#include "macros.h"
#include "functions.h"
extern PlayState* gPlayState;
void func_80833B18(PlayState* play, Player* player, s32 arg2, f32 speed, f32 velocityY, s16 arg5,
                   s32 invincibilityTimer);
}

/**
 * DAMAGE_PLAYER
 */

void Anchor::SendPacket_DamagePlayer(u32 clientId, u8 damageEffect, u8 damage) {
    if (!IsSaveLoaded()) {
        return;
    }

    nlohmann::json payload;
    payload["type"] = DAMAGE_PLAYER;
    payload["targetClientId"] = clientId;
    payload["damageEffect"] = damageEffect;
    payload["damage"] = damage;

    QueueOutgoingPacket(payload);
}

void Anchor::HandlePacket_DamagePlayer(nlohmann::json payload) {
    if (!IsSaveLoaded()) {
        return;
    }

    uint32_t clientId = payload["clientId"].get<uint32_t>();
    if (!clients.contains(clientId) || clients[clientId].player == nullptr) {
        return;
    }

    AnchorClient& anchorClient = clients[clientId];
    Player* otherPlayer = anchorClient.player;
    Player* self = GET_PLAYER(gPlayState);

    // Prevent incoming damage during cutscenes or item get sequences
    if (Player_InBlockingCsMode(gPlayState, self) || self->stateFlags1 & PLAYER_STATE1_400 ||
        self->stateFlags1 & PLAYER_STATE1_10000000) {
        return;
    }

    u8 damageEffect = payload["damageEffect"].get<u8>();
    u8 damage = payload["damage"].get<u8>();

    s32 scaledDamage = damage * 8;
    self->actor.colChkInfo.damage = scaledDamage > 0xFF ? 0xFF : scaledDamage;

    if (damageEffect == DUMMY_PLAYER_HIT_RESPONSE_FIRE) {
        for (int i = 0; i < ARRAY_COUNT(self->bodyFlameTimers); i++) {
            self->bodyFlameTimers[i] = Rand_S16Offset(0, 200);
        }
        self->bodyIsBurning = true;
    } else if (damageEffect == DUMMY_PLAYER_HIT_RESPONSE_STUN) {
        self->actor.freezeTimer = 20;
        Actor_SetColorFilter(&self->actor, 0, 0xFF, 0, 24);
        return;
    }

    func_80833B18(gPlayState, self, damageEffect, 4.0f, 5.0f,
                  Actor_WorldYawTowardActor(&otherPlayer->actor, &self->actor), 20);
}
