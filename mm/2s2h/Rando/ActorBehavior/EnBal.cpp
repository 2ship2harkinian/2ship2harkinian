#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/CustomMessage/CustomMessage.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Bal/z_en_bal.h"
void EnBal_SetupTalk(EnBal* enBal);
}

std::map<int16_t, std::vector<RandoCheckId>> tingleMap = {
    { SCENE_BACKTOWN, { RC_CLOCK_TOWN_NORTH_TINGLE_MAP_01, RC_CLOCK_TOWN_NORTH_TINGLE_MAP_02 } },
    { SCENE_24KEMONOMITI, { RC_ROAD_TO_SOUTHERN_SWAMP_TINGLE_MAP_01, RC_ROAD_TO_SOUTHERN_SWAMP_TINGLE_MAP_02 } },
    { SCENE_17SETUGEN, { RC_TWIN_ISLANDS_TINGLE_MAP_01, RC_TWIN_ISLANDS_TINGLE_MAP_02 } },
    { SCENE_17SETUGEN2, { RC_TWIN_ISLANDS_TINGLE_MAP_01, RC_TWIN_ISLANDS_TINGLE_MAP_02 } },
    { SCENE_ROMANYMAE, { RC_MILK_ROAD_TINGLE_MAP_01, RC_MILK_ROAD_TINGLE_MAP_02 } },
    { SCENE_30GYOSON, { RC_GREAT_BAY_COAST_TINGLE_MAP_01, RC_GREAT_BAY_COAST_TINGLE_MAP_02 } },
    { SCENE_IKANA, { RC_IKANA_CANYON_TINGLE_MAP_01, RC_IKANA_CANYON_TINGLE_MAP_02 } }
};

std::map<int16_t, std::vector<RandoCheckId>> tingleMapShared = {
    { SCENE_BACKTOWN, { RC_CLOCK_TOWN_TINGLE_MAP, RC_WOODFALL_TINGLE_MAP } },
    { SCENE_24KEMONOMITI, { RC_WOODFALL_TINGLE_MAP, RC_SNOWHEAD_TINGLE_MAP } },
    { SCENE_17SETUGEN, { RC_SNOWHEAD_TINGLE_MAP, RC_ROMANI_RANCH_TINGLE_MAP } },
    { SCENE_17SETUGEN2, { RC_SNOWHEAD_TINGLE_MAP, RC_ROMANI_RANCH_TINGLE_MAP } },
    { SCENE_ROMANYMAE, { RC_ROMANI_RANCH_TINGLE_MAP, RC_GREAT_BAY_TINGLE_MAP } },
    { SCENE_30GYOSON, { RC_GREAT_BAY_TINGLE_MAP, RC_STONE_TOWER_TINGLE_MAP } },
    { SCENE_IKANA, { RC_STONE_TOWER_TINGLE_MAP, RC_CLOCK_TOWN_TINGLE_MAP } }
};

void OnOpenShopText(u16* textId, bool* loadFromMessageTable) {
    RandoCheckId randoCheckId1;
    RandoCheckId randoCheckId2;
    if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_TINGLE] == RO_SHUFFLE_TINGLE_FULL) {
        randoCheckId1 = tingleMap[gPlayState->sceneId][0];
        randoCheckId2 = tingleMap[gPlayState->sceneId][1];
    } else {
        randoCheckId1 = tingleMapShared[gPlayState->sceneId][0];
        randoCheckId2 = tingleMapShared[gPlayState->sceneId][1];
    }

    auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
    entry.autoFormat = false;

    entry.msg = "\x02\xC3{item1}\x01 {price1} Rupees\x11"
                "\x02{item2}\x01 {price2} Rupees\x11"
                "\x02No thanks";

    CustomMessage::Replace(
        &entry.msg, "{item1}",
        Rando::StaticData::GetItemName(Rando::ConvertItem(RANDO_SAVE_CHECKS[randoCheckId1].randoItemId, randoCheckId1),
                                       false, randoCheckId1));
    CustomMessage::Replace(
        &entry.msg, "{item2}",
        Rando::StaticData::GetItemName(Rando::ConvertItem(RANDO_SAVE_CHECKS[randoCheckId2].randoItemId, randoCheckId2),
                                       false, randoCheckId2));
    CustomMessage::Replace(&entry.msg, "{price1}", std::to_string(RANDO_SAVE_CHECKS[randoCheckId1].price));
    if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_TINGLE] == RO_SHUFFLE_TINGLE_FULL) {
        CustomMessage::Replace(&entry.msg, "{price2}", std::to_string(RANDO_SAVE_CHECKS[randoCheckId2].price));
    } else {
        CustomMessage::Replace(&entry.msg, "{price2}", std::to_string(RANDO_SAVE_CHECKS[randoCheckId2].altPrice));
    }
    CustomMessage::EnsureMessageEnd(&entry.msg);
    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
};

void OnOpenCantGetText(u16* textId, bool* loadFromMessageTable) {
    auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
    entry.msg = "I'm sorry, but it seems I cannot sell this to you now.";
    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
};

void Rando::ActorBehavior::InitEnBalBehavior() {
    bool shouldRegister = IS_RANDO && !(RANDO_SAVE_OPTIONS[RO_SHUFFLE_TINGLE] == RO_SHUFFLE_TINGLE_OFF);

    COND_VB_SHOULD(VB_NOT_AFFORD_TINGLE_MAP, shouldRegister, {
        EnBal* enBal = va_arg(args, EnBal*);
        s32* price = va_arg(args, s32*);
        RandoCheckId randoCheckId;
        if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_TINGLE] == RO_SHUFFLE_TINGLE_FULL) {
            randoCheckId = tingleMap[gPlayState->sceneId][gPlayState->msgCtx.choiceIndex];

            *price = RANDO_SAVE_CHECKS[randoCheckId].price;

        } else {
            auto randoCheckId = tingleMapShared[gPlayState->sceneId][gPlayState->msgCtx.choiceIndex];
            if (gPlayState->msgCtx.choiceIndex == 0) {
                *price = RANDO_SAVE_CHECKS[randoCheckId].price;
            } else {
                *price = RANDO_SAVE_CHECKS[randoCheckId].altPrice;
            }
        }
        *should = gSaveContext.save.saveInfo.playerData.rupees < *price;
    });

    COND_VB_SHOULD(VB_ALREADY_HAVE_TINGLE_MAP, shouldRegister, {
        EnBal* enBal = va_arg(args, EnBal*);
        RandoCheckId randoCheckId;
        if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_TINGLE] == RO_SHUFFLE_TINGLE_FULL) {
            randoCheckId = tingleMap[gPlayState->sceneId][gPlayState->msgCtx.choiceIndex];
        } else {
            randoCheckId = tingleMapShared[gPlayState->sceneId][gPlayState->msgCtx.choiceIndex];
        }

        RandoItemId randoItemId = Rando::ConvertItem(RANDO_SAVE_CHECKS[randoCheckId].randoItemId, randoCheckId);

        *should = !Rando::IsItemObtainable(randoItemId, randoCheckId);
    });

    COND_VB_SHOULD(VB_TINGLE_GIVE_MAP_UNLOCK, shouldRegister, {
        EnBal* enBal = va_arg(args, EnBal*);
        if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_TINGLE] == RO_SHUFFLE_TINGLE_FULL) {
            RANDO_SAVE_CHECKS[tingleMap[gPlayState->sceneId][gPlayState->msgCtx.choiceIndex]].eligible = true;
        } else {
            RANDO_SAVE_CHECKS[tingleMapShared[gPlayState->sceneId][gPlayState->msgCtx.choiceIndex]].eligible = true;
        }
        Message_StartTextbox(gPlayState, 0x1D17, &enBal->picto.actor);
        enBal->textId = 0x1D17;
        EnBal_SetupTalk(enBal);
        *should = false;
    });

    COND_ID_HOOK(OnOpenText, 0x1D11, shouldRegister, OnOpenShopText);
    COND_ID_HOOK(OnOpenText, 0x1D12, shouldRegister, OnOpenShopText);
    COND_ID_HOOK(OnOpenText, 0x1D13, shouldRegister, OnOpenShopText);
    COND_ID_HOOK(OnOpenText, 0x1D14, shouldRegister, OnOpenShopText);
    COND_ID_HOOK(OnOpenText, 0x1D15, shouldRegister, OnOpenShopText);
    COND_ID_HOOK(OnOpenText, 0x1D16, shouldRegister, OnOpenShopText);
    COND_ID_HOOK(OnOpenText, 0x1D09, shouldRegister, OnOpenCantGetText);

    COND_VB_SHOULD(VB_HAVE_MAGIC_FOR_TINGLE, shouldRegister, { *should = true; });
}
