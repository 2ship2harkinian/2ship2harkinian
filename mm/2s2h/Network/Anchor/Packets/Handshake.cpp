#include "2s2h/Network/Anchor/Anchor.h"
#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/BenPort.h"

/**
 * HANDSHAKE
 *
 * Sent by the client to the server when it first connects to the server, sends over both the local room settings
 * in case the room needs to be created, along with the current client state
 */

void Anchor::SendPacket_Handshake() {
    nlohmann::json payload;
    payload["type"] = HANDSHAKE;
    payload["roomId"] = CVarGetString("gNetwork.Anchor.RoomId", "");
    payload["roomState"] = PrepRoomState();
    payload["clientState"] = PrepClientState();

    QueueOutgoingPacket(payload);
}
