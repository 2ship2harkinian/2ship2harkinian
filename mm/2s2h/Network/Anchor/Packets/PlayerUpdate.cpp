#include "2s2h/Network/Anchor/Anchor.h"
#include "2s2h/Network/Anchor/JsonConversions.hpp"
#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>

extern "C" {
#include "macros.h"
#include "variables.h"
extern PlayState* gPlayState;
}

/**
 * PLAYER_UPDATE
 *
 * Contains real-time data necessary to update other clients in the same scene as the player
 *
 * Sent every frame to other clients within the same scene
 *
 * Note: This packet is sent _a lot_, so please do not include any unnecessary data in it
 */

void Anchor::SendPacket_PlayerUpdate() {
    if (!IsSaveLoaded()) {
        return;
    }

    uint32_t currentPlayerCount = 0;
    for (auto& [clientId, client] : clients) {
        if (client.sceneId == gPlayState->sceneId && client.online && client.isSaveLoaded && !client.self) {
            currentPlayerCount++;
        }
    }
    if (currentPlayerCount == 0) {
        return;
    }

    Player* player = GET_PLAYER(gPlayState);
    nlohmann::json payload;

    payload["type"] = PLAYER_UPDATE;
    payload["sceneId"] = gPlayState->sceneId;
    payload["entrance"] = gSaveContext.save.entrance;
    payload["roomIndex"] = gPlayState->roomCtx.curRoom.num;
    payload["transformation"] = player->transformation;
    payload["posRot"]["pos"] = player->actor.world.pos;
    payload["posRot"]["rot"] = player->actor.shape.rot;
    payload["jointTable"] = player->jointTableBuffer;
    payload["upperJointTable"] = player->jointTableUpperBuffer;
    payload["currentMask"] = player->currentMask;
    payload["rightHandType"] = player->rightHandType;
    payload["leftHandType"] = player->leftHandType;
    payload["currentShield"] = player->currentShield;
    payload["sheathType"] = player->sheathType;
    payload["heldItemAction"] = player->heldItemAction;
    payload["heldItemId"] = player->heldItemId;
    payload["itemAction"] = player->itemAction;
    payload["stateFlags1"] = player->stateFlags1;
    payload["stateFlags2"] = player->stateFlags2;
    payload["stateFlags3"] = player->stateFlags3;
    payload["unk_B0C"] = player->unk_B0C;
    payload["unk_B28"] = player->unk_B28;
    payload["unk_ACC"] = player->unk_ACC;
    payload["invincibilityTimer"] = player->invincibilityTimer;
    payload["quiet"] = true;

    for (auto& [clientId, client] : clients) {
        if (client.sceneId == gPlayState->sceneId && client.online && client.isSaveLoaded && !client.self) {
            payload["targetClientId"] = clientId;
            QueueOutgoingPacket(payload);
        }
    }
}

void Anchor::HandlePacket_PlayerUpdate(nlohmann::json payload) {
    uint32_t clientId = payload["clientId"].get<uint32_t>();

    if (clients.contains(clientId)) {
        auto& client = clients[clientId];

        if (client.transformation != payload.value("transformation", (u8)PLAYER_FORM_HUMAN)) {
            shouldRefreshActors = true;
        }

        client.sceneId = payload.value("sceneId", (s16)SCENE_MAX);
        client.entrance = payload.value("entrance", (s32)0);
        client.transformation = payload.value("transformation", (uint8_t)PLAYER_FORM_HUMAN);
        client.posRot = payload.value("posRot", PosRot{});
        std::vector<u8> jointTableVec = payload.value("jointTable", std::vector<u8>{});
        std::vector<u8> upperJointTableVec = payload.value("upperJointTable", std::vector<u8>{});
        for (int i = 0; i < 159; i++) {
            if (i < jointTableVec.size()) {
                client.jointTable[i] = jointTableVec[i];
            }
            if (i < upperJointTableVec.size()) {
                client.upperJointTable[i] = upperJointTableVec[i];
            }
        }
        client.currentMask = payload.value("currentMask", (uint8_t)0);
        client.rightHandType = payload.value("rightHandType", (uint8_t)0);
        client.leftHandType = payload.value("leftHandType", (uint8_t)0);
        client.currentShield = payload.value("currentShield", (int8_t)0);
        client.sheathType = payload.value("sheathType", (uint8_t)0);
        client.heldItemAction = payload.value("heldItemAction", (int8_t)0);
        client.heldItemId = payload.value("heldItemId", (uint8_t)0);
        client.itemAction = payload.value("itemAction", (int8_t)0);
        client.stateFlags1 = payload.value("stateFlags1", (uint32_t)0);
        client.stateFlags2 = payload.value("stateFlags2", (uint32_t)0);
        client.stateFlags3 = payload.value("stateFlags3", (uint32_t)0);
        client.unk_B0C = payload.value("unk_B0C", 0.0f);
        client.unk_B28 = payload.value("unk_B28", (int16_t)0);
        client.unk_ACC = payload.value("unk_ACC", (int16_t)0);
        client.invincibilityTimer = payload.value("invincibilityTimer", (int8_t)0);
    }
}
