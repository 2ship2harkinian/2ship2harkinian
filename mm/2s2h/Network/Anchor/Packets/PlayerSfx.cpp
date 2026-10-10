#include "2s2h/Network/Anchor/Anchor.h"
#include "2s2h/Network/Anchor/JsonConversions.hpp"
#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>

extern "C" {
#include "macros.h"
#include "functions.h"
#include "variables.h"
extern PlayState* gPlayState;
}

/**
 * PLAYER_SFX
 *
 * Sound effects, only sent to other clients in the same scene as the player
 */

void Anchor::SendPacket_PlayerSfx(u16 sfxId) {
    if (!IsSaveLoaded()) {
        return;
    }

    nlohmann::json payload;

    payload["type"] = PLAYER_SFX;
    payload["sfxId"] = sfxId;
    payload["quiet"] = true;

    for (auto& [clientId, client] : clients) {
        if (client.sceneId == gPlayState->sceneId && client.online && client.isSaveLoaded && !client.self) {
            payload["targetClientId"] = clientId;
            QueueOutgoingPacket(payload);
        }
    }
}

void Anchor::HandlePacket_PlayerSfx(nlohmann::json payload) {
    if (!IsSaveLoaded()) {
        return;
    }

    uint32_t clientId = payload["clientId"].get<uint32_t>();
    u16 sfxId = payload["sfxId"].get<u16>();

    if (!clients.contains(clientId) || !clients[clientId].player) {
        return;
    }

    Player_PlaySfx(clients[clientId].player, sfxId);
}
