#include "2s2h/Network/Anchor/Anchor.h"
#include "2s2h/Network/Anchor/JsonConversions.hpp"
#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>
#include "2s2h/BenPort.h"

extern "C" {
#include "variables.h"
extern PlayState* gPlayState;
}

/**
 * UPDATE_ROOM_STATE
 */

nlohmann::json Anchor::PrepRoomState() {
    nlohmann::json payload;
    payload["ownerClientId"] = ownClientId;
    bool isGlobalRoom = (std::string("2ship-global") == CVarGetString("gNetwork.Anchor.RoomId", ""));

    if (isGlobalRoom) {
        payload["pvpMode"] = 0;
        payload["teleportMode"] = 0;
        payload["showLocationsMode"] = 0;
        payload["syncItemsAndFlags"] = 0;
    } else {
        payload["pvpMode"] = CVarGetInteger("gNetwork.Anchor.RoomSettings.PvpMode", 2);
        payload["teleportMode"] = CVarGetInteger("gNetwork.Anchor.RoomSettings.TeleportMode", 1);
        payload["showLocationsMode"] = CVarGetInteger("gNetwork.Anchor.RoomSettings.ShowLocationsMode", 1);
        payload["syncItemsAndFlags"] = CVarGetInteger("gNetwork.Anchor.RoomSettings.SyncItemsAndFlags", 1);
    }

    return payload;
}

void Anchor::SendPacket_UpdateRoomState() {
    nlohmann::json payload;
    payload["type"] = UPDATE_ROOM_STATE;
    payload["state"] = PrepRoomState();

    Network::QueueOutgoingPacket(payload);
}

void Anchor::HandlePacket_UpdateRoomState(nlohmann::json payload) {
    if (!payload.contains("state")) {
        return;
    }

    roomState.ownerClientId = payload["state"].value("ownerClientId", 0);
    roomState.pvpMode = payload["state"].value("pvpMode", 2);
    roomState.teleportMode = payload["state"].value("teleportMode", 1);
    roomState.showLocationsMode = payload["state"].value("showLocationsMode", 1);
    roomState.syncItemsAndFlags = payload["state"].value("syncItemsAndFlags", 1);
}
