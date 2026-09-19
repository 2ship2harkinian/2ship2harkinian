#include "2s2h/Network/Anchor/Anchor.h"
#include "2s2h/Network/Anchor/JsonConversions.hpp"
#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>
#include "2s2h/BenPort.h"
#include "2s2h/BenGui/Notification.h"
#include "2s2h/Rando/Rando.h"
#include "2s2h/Rando/CheckTracker/CheckTracker.h"
#include "2s2h/Rando/ActorBehavior/ActorBehavior.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
extern PlayState* gPlayState;
}

/**
 * UPDATE_TEAM_STATE
 *
 * Pushes the current save state to the server for other teammates to use.
 *
 * Fires when the server passes on a REQUEST_TEAM_STATE packet, or when this client saves the game
 *
 * When sending this packet we will assume that the team queue has been emptied for this client, so the queue
 * stored in the server will be cleared.
 *
 * When receiving this packet, if there is items in the team queue, we will play them back in order.
 */

void Anchor::SendPacket_UpdateTeamState(std::string targetTeamId) {
    if (!roomState.syncItemsAndFlags) {
        return;
    }

    json payload;
    payload["type"] = UPDATE_TEAM_STATE;
    payload["targetTeamId"] = targetTeamId;

    // Assume the team queue has been emptied, so clear it
    payload["queue"] = json::array();

    payload["state"] = gSaveContext.save;

    // Hack to reduce the amount of bytes for this data
    if (IS_RANDO) {
        payload["state"]["shipSaveInfo"]["rando"].erase("randoSaveChecks");
        payload["state"]["shipSaveInfo"]["rando"]["randoSaveChecksCopy"] = json::array();
        for (int i = 0; i < RC_MAX; i++) {
            payload["state"]["shipSaveInfo"]["rando"]["randoSaveChecksCopy"][i] = json::array();
            payload["state"]["shipSaveInfo"]["rando"]["randoSaveChecksCopy"][i][0] = RANDO_SAVE_CHECKS[i].randoItemId;
            payload["state"]["shipSaveInfo"]["rando"]["randoSaveChecksCopy"][i][1] = (u8)RANDO_SAVE_CHECKS[i].shuffled;
            payload["state"]["shipSaveInfo"]["rando"]["randoSaveChecksCopy"][i][2] = (u8)RANDO_SAVE_CHECKS[i].eligible;
            payload["state"]["shipSaveInfo"]["rando"]["randoSaveChecksCopy"][i][3] =
                (u8)RANDO_SAVE_CHECKS[i].cycleObtained;
            payload["state"]["shipSaveInfo"]["rando"]["randoSaveChecksCopy"][i][4] = (u8)RANDO_SAVE_CHECKS[i].obtained;
            payload["state"]["shipSaveInfo"]["rando"]["randoSaveChecksCopy"][i][5] = (u8)RANDO_SAVE_CHECKS[i].skipped;
            payload["state"]["shipSaveInfo"]["rando"]["randoSaveChecksCopy"][i][6] = RANDO_SAVE_CHECKS[i].price;
        }
    }

    QueueOutgoingPacket(payload);
}

void Anchor::SendPacket_ClearTeamState(std::string teamId) {
    json payload;
    payload["type"] = UPDATE_TEAM_STATE;
    payload["targetTeamId"] = teamId;
    payload["queue"] = json::array();
    payload["state"] = json::object();
    QueueOutgoingPacket(payload);
}

void Anchor::HandlePacket_UpdateTeamState(nlohmann::json payload) {
    if (!roomState.syncItemsAndFlags) {
        return;
    }

    if (payload.contains("state")) {
        // Hack to reduce the amount of bytes for this data
        if (IS_RANDO && payload["state"]["shipSaveInfo"].contains("rando")) {
            auto stuff =
                payload["state"]["shipSaveInfo"]["rando"]["randoSaveChecksCopy"].get<std::vector<std::vector<s32>>>();
            for (int i = 0; i < RC_MAX; i++) {
                payload["state"]["shipSaveInfo"]["rando"]["randoSaveChecks"][i] = RandoSaveCheck{
                    (RandoItemId)stuff[i][0], (bool)stuff[i][1], (bool)stuff[i][2], (bool)stuff[i][3],
                    (bool)stuff[i][4],        (bool)stuff[i][5], (u16)stuff[i][6],
                };
            }
        }

        Save loadedData = payload["state"].get<Save>();

        // Restore bottle contents (unless it's the Deku Princess)
        for (int i = 0; i < 6; i++) {
            if (gSaveContext.save.saveInfo.inventory.items[SLOT_BOTTLE_1 + i] != ITEM_NONE &&
                gSaveContext.save.saveInfo.inventory.items[SLOT_BOTTLE_1 + i] != ITEM_DEKU_PRINCESS) {
                loadedData.saveInfo.inventory.items[SLOT_BOTTLE_1 + i] =
                    gSaveContext.save.saveInfo.inventory.items[SLOT_BOTTLE_1 + i];
            }
        }

        // Restore ammo if it's non-zero, unless it's beans
        for (int i = 0; i < ARRAY_COUNT(gSaveContext.save.saveInfo.inventory.ammo); i++) {
            if (gSaveContext.save.saveInfo.inventory.ammo[i] != 0 && i != SLOT(ITEM_MAGIC_BEANS)) {
                loadedData.saveInfo.inventory.ammo[i] = gSaveContext.save.saveInfo.inventory.ammo[i];
            }
        }

        // Restore stuff that shouldn't be synced
        loadedData.saveInfo.checksum = gSaveContext.save.saveInfo.checksum;
        loadedData.shipSaveInfo.fileCreatedAt = gSaveContext.save.shipSaveInfo.fileCreatedAt;
        memcpy(loadedData.saveInfo.playerData.newf, gSaveContext.save.saveInfo.playerData.newf,
               sizeof(loadedData.saveInfo.playerData.newf));
        memcpy(&loadedData.shipSaveInfo.dpadEquips, &gSaveContext.save.shipSaveInfo.dpadEquips,
               sizeof(loadedData.shipSaveInfo.dpadEquips));
        memcpy(loadedData.saveInfo.equips.cButtonSlots, gSaveContext.save.saveInfo.equips.cButtonSlots,
               sizeof(loadedData.saveInfo.equips.cButtonSlots));
        memcpy(loadedData.saveInfo.equips.buttonItems, gSaveContext.save.saveInfo.equips.buttonItems,
               sizeof(loadedData.saveInfo.equips.buttonItems));
        memcpy(loadedData.saveInfo.playerData.playerName, gSaveContext.save.saveInfo.playerData.playerName,
               sizeof(loadedData.saveInfo.playerData.playerName));

        gSaveContext.save.saveInfo = loadedData.saveInfo;
        gSaveContext.save.shipSaveInfo = loadedData.shipSaveInfo;

        Notification::Emit({
            .message = "Save updated from team",
        });
        Rando::CheckTracker::OnFileLoad();
        Rando::ActorBehavior::OnFileLoad();
        ShipInit::Init("IS_RANDO");
    }

    if (payload.contains("queue") && payload["queue"].is_array()) {
        for (auto& item : payload["queue"]) {
            try {
                nlohmann::json jsonPayload = nlohmann::json::parse(item.get<std::string>());
                queuedPacketsFromTeamState.push(jsonPayload);
            } catch (const std::exception& e) {
                SPDLOG_ERROR("[Anchor] Failed to parse queued packet: \n{}\n{}\n", item.get<std::string>(), e.what());
            }
        }
    }
}
