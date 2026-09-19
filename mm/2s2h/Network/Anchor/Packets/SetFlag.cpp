#include "2s2h/Network/Anchor/Anchor.h"
#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/BenPort.h"

/**
 * SET_FLAG
 *
 * Fired when a flag is set in the save context
 */

void Anchor::SendPacket_SetFlag(s16 sceneId, s16 flagType, s16 flag) {
    if (!IsSaveLoaded() || !roomState.syncItemsAndFlags) {
        return;
    }

    nlohmann::json payload;
    payload["type"] = SET_FLAG;
    payload["targetTeamId"] = CVarGetString("gNetwork.Anchor.TeamId", "default");
    payload["addToQueue"] = true;
    payload["sceneId"] = sceneId;
    payload["flagType"] = flagType;
    payload["flag"] = flag;

    QueueOutgoingPacket(payload);
}

void Anchor::HandlePacket_SetFlag(nlohmann::json payload) {
    if (!IsSaveLoaded() || !roomState.syncItemsAndFlags) {
        return;
    }

    s16 sceneId = payload["sceneId"].get<s16>();
    s16 flagType = payload["flagType"].get<s16>();
    s16 flag = payload["flag"].get<s16>();

    // if (sceneId == SCENE_MAX) {
    //     auto effect = new GameInteractionEffect::SetFlag();
    //     effect->parameters[0] = payload["flagType"].get<int16_t>();
    //     effect->parameters[1] = payload["flag"].get<int16_t>();
    //     effect->Apply();
    // } else {
    //     auto effect = new GameInteractionEffect::SetSceneFlag();
    //     effect->parameters[0] = payload["sceneId"].get<int16_t>();
    //     effect->parameters[1] = payload["flagType"].get<int16_t>();
    //     effect->parameters[2] = payload["flag"].get<int16_t>();
    //     effect->Apply();
    // }
}
