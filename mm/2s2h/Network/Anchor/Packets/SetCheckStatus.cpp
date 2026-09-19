#include "2s2h/Network/Anchor/Anchor.h"
#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/BenJsonConversions.hpp"

/**
 * SET_CHECK_STATUS
 *
 * Fired when a check status is updated or skipped
 */

void Anchor::SendPacket_SetCheckStatus(RandoCheckId randoCheckId) {
    if (!IsSaveLoaded() || !roomState.syncItemsAndFlags) {
        return;
    }

    nlohmann::json payload;
    payload["type"] = SET_CHECK_STATUS;
    payload["targetTeamId"] = CVarGetString("gNetwork.Anchor.TeamId", "default");
    payload["addToQueue"] = true;
    payload["randoCheckId"] = randoCheckId;
    payload["data"] = RANDO_SAVE_CHECKS[randoCheckId];

    QueueOutgoingPacket(payload);
}

void Anchor::HandlePacket_SetCheckStatus(nlohmann::json payload) {
    if (!IsSaveLoaded() || !roomState.syncItemsAndFlags) {
        return;
    }

    RandoCheckId randoCheckId = payload["randoCheckId"].get<RandoCheckId>();
    // RANDO_SAVE_CHECKS[randoCheckId].randoItemId = payload["data"]["randoItemId"].get<RandoItemId>();
    // RANDO_SAVE_CHECKS[randoCheckId].shuffled = payload["data"]["shuffled"].get<bool>();
    // RANDO_SAVE_CHECKS[randoCheckId].eligible = payload["data"]["eligible"].get<bool>();
    // RANDO_SAVE_CHECKS[randoCheckId].cycleObtained = payload["data"]["cycleObtained"].get<bool>();
    RANDO_SAVE_CHECKS[randoCheckId].obtained = payload["data"]["obtained"].get<bool>();
    RANDO_SAVE_CHECKS[randoCheckId].skipped = payload["data"]["skipped"].get<bool>();
    // RANDO_SAVE_CHECKS[randoCheckId].price = payload["data"]["price"].get<u16>();
}
