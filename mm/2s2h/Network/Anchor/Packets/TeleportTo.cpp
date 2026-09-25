#include "2s2h/Network/Anchor/Anchor.h"
#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/Network/Anchor/JsonConversions.hpp"

extern "C" {
#include "macros.h"
extern PlayState* gPlayState;
}

/**
 * TELEPORT_TO
 *
 * See REQUEST_TELEPORT for more information, this is the second part of the process.
 */

void Anchor::SendPacket_TeleportTo(uint32_t clientId) {
    if (!IsSaveLoaded()) {
        return;
    }

    Player* player = GET_PLAYER(gPlayState);

    nlohmann::json payload;
    payload["type"] = TELEPORT_TO;
    payload["targetClientId"] = clientId;
    payload["entrance"] = gSaveContext.save.entrance;
    payload["roomIndex"] = gPlayState->roomCtx.curRoom.num;
    payload["posRot"] = player->actor.world;

    QueueOutgoingPacket(payload);
}

void Anchor::HandlePacket_TeleportTo(nlohmann::json payload) {
    if (!IsSaveLoaded()) {
        return;
    }

    s32 entrance = payload["entrance"].get<s32>();
    s8 roomIndex = payload["roomIndex"].get<s8>();
    PosRot posRot = payload["posRot"].get<PosRot>();

    gPlayState->nextEntrance = entrance;
    gPlayState->transitionTrigger = TRANS_TRIGGER_START;
    gPlayState->transitionType = TRANS_TYPE_INSTANT;
    gSaveContext.respawn[RESPAWN_MODE_DOWN].entrance = entrance;
    gSaveContext.respawn[RESPAWN_MODE_DOWN].roomIndex = roomIndex;
    gSaveContext.respawn[RESPAWN_MODE_DOWN].pos = posRot.pos;
    gSaveContext.respawn[RESPAWN_MODE_DOWN].yaw = posRot.rot.y;
    gSaveContext.respawn[RESPAWN_MODE_DOWN].playerParams = 0xDFF;
    gSaveContext.nextTransitionType = TRANS_TYPE_FADE_BLACK_FAST;
    gSaveContext.respawnFlag = -8;
}
