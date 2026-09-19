#include "2s2h/Network/Anchor/Anchor.h"
#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/BenPort.h"

/**
 * UPDATE_DUNGEON_ITEMS
 *
 * This is for 2 things, first is updating the dungeon items in vanilla saves, and second is
 * for ensuring the amount of keys used is synced as players are using them.
 */

void Anchor::SendPacket_UpdateDungeonItems() {
    if (!IsSaveLoaded() || !roomState.syncItemsAndFlags) {
        return;
    }

    nlohmann::json payload;
    payload["type"] = UPDATE_DUNGEON_ITEMS;
    payload["targetTeamId"] = CVarGetString("gNetwork.Anchor.TeamId", "default");
    payload["addToQueue"] = true;
    payload["mapIndex"] = gSaveContext.mapIndex;
    payload["dungeonItems"] = gSaveContext.save.saveInfo.inventory.dungeonItems[gSaveContext.mapIndex];
    payload["dungeonKeys"] = gSaveContext.save.saveInfo.inventory.dungeonKeys[gSaveContext.mapIndex];
    payload["strayFairies"] = gSaveContext.save.saveInfo.inventory.strayFairies[gSaveContext.mapIndex];

    QueueOutgoingPacket(payload);
}

void Anchor::HandlePacket_UpdateDungeonItems(nlohmann::json payload) {
    if (!IsSaveLoaded() || !roomState.syncItemsAndFlags) {
        return;
    }

    u16 mapIndex = payload["mapIndex"].get<u16>();
    gSaveContext.save.saveInfo.inventory.dungeonItems[mapIndex] = payload["dungeonItems"].get<u8>();
    gSaveContext.save.saveInfo.inventory.dungeonKeys[mapIndex] = payload["dungeonKeys"].get<s8>();
    gSaveContext.save.saveInfo.inventory.strayFairies[mapIndex] = payload["strayFairies"].get<s8>();
}
