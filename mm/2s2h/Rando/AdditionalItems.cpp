#include "Rando.h"
#include <libultraship/libultraship.h>

static constexpr const char* ADDITIONAL_ITEMS_PATH = "CVars.gRando.AdditionalItems";

namespace Rando {

static std::map<RandoItemId, u16> AdditionalItemsFromJson(nlohmann::json& additionalItemsJson) {
    std::map<RandoItemId, u16> additionalItems;

    if (!additionalItemsJson.is_array()) {
        return additionalItems;
    }

    for (auto& entry : additionalItemsJson) {
        if (!entry.is_object() || !entry.contains("item") || !entry["item"].is_string()) {
            continue;
        }

        RandoItemId randoItemId = Rando::StaticData::GetItemIdFromName(entry["item"].get<std::string>().c_str());
        if (randoItemId <= RI_UNKNOWN || randoItemId >= RI_MAX) {
            continue;
        }

        int count = entry.contains("count") && entry["count"].is_number_integer() ? entry["count"].get<int>() : 0;
        if (count > 0) {
            additionalItems.insert_or_assign(randoItemId, count);
        }
    }

    return additionalItems;
}

static nlohmann::json AdditionalItemsToJson(std::map<RandoItemId, u16>& additionalItems) {
    auto additionalItemsJson = nlohmann::json::array();

    for (auto& [randoItemId, count] : additionalItems) {
        if (randoItemId <= RI_UNKNOWN || randoItemId >= RI_MAX) {
            continue;
        }
        additionalItemsJson.push_back(
            { { "item", Rando::StaticData::Items[randoItemId].spoilerName }, { "count", count } });
    }

    return additionalItemsJson;
}

std::map<RandoItemId, u16> GetAdditionalItemsFromConfig() {
    auto allConfig = Ship::Context::GetRawInstance()->GetConfig()->GetNestedJson();
    std::map<RandoItemId, u16> additionalItems;

    if (allConfig.find("CVars") != allConfig.end() && allConfig["CVars"].is_object() &&
        allConfig["CVars"].find("gRando") != allConfig["CVars"].end() && allConfig["CVars"]["gRando"].is_object() &&
        allConfig["CVars"]["gRando"].find("AdditionalItems") != allConfig["CVars"]["gRando"].end()) {

        additionalItems = AdditionalItemsFromJson(allConfig["CVars"]["gRando"]["AdditionalItems"]);
    }

    return additionalItems;
}

void SetAdditionalItemsInConfig(std::map<RandoItemId, u16>& additionalItems) {
    // SetBlock() already persists to disk internally - no separate Save() call needed here.
    Ship::Context::GetRawInstance()->GetConfig()->SetBlock(ADDITIONAL_ITEMS_PATH,
                                                           AdditionalItemsToJson(additionalItems));
}

std::map<RandoItemId, u16> GetAdditionalItemsFromSpoiler(nlohmann::json& spoiler) {
    // Spoilers written before this option existed won't have the key
    if (!spoiler.contains("additionalItems")) {
        return {};
    }

    return AdditionalItemsFromJson(spoiler["additionalItems"]);
}

void SetAdditionalItemsInSpoiler(nlohmann::json& spoiler, std::map<RandoItemId, u16>& additionalItems) {
    spoiler["additionalItems"] = AdditionalItemsToJson(additionalItems);
}

std::map<RandoItemId, u16> GetAdditionalItemsFromSave(RandoSaveInfo& randoSaveInfo) {
    std::map<RandoItemId, u16> additionalItems;

    for (int i = 0; i < RI_MAX; i++) {
        if (randoSaveInfo.additionalItemCounts[i] > 0) {
            additionalItems[(RandoItemId)i] = randoSaveInfo.additionalItemCounts[i];
        }
    }

    return additionalItems;
}

void SetAdditionalItemsInSave(RandoSaveInfo& randoSaveInfo, std::map<RandoItemId, u16>& additionalItems) {
    memset(randoSaveInfo.additionalItemCounts, 0, sizeof(randoSaveInfo.additionalItemCounts));

    for (auto& [randoItemId, count] : additionalItems) {
        if (randoItemId <= RI_UNKNOWN || randoItemId >= RI_MAX) {
            continue;
        }
        randoSaveInfo.additionalItemCounts[randoItemId] = count;
    }
}

} // namespace Rando
