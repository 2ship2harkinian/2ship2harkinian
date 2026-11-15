#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

#include "2s2h/ObjectExtension/ActorListIndex.h"
#include "2s2h/CustomItem/CustomItem.h"
#include "assets/2s2h_assets.h"

extern "C" {
#include "overlays/actors/ovl_En_Ishi/z_en_ishi.h"
#include "overlays/actors/ovl_Obj_Mure2/z_obj_mure2.h"
}

// clang-format off
// Rocks In Scenes
std::map<std::tuple<s16, s16, s16>, RandoCheckId> enIshiItemMap = {
    // Goron Shrine
    { { SCENE_16GORON_HOUSE, 0, 2 }, RC_GORON_SHRINE_ROCK_01 },
    { { SCENE_16GORON_HOUSE, 0, 4 }, RC_GORON_SHRINE_ROCK_02 },
    { { SCENE_16GORON_HOUSE, 0, 5 }, RC_GORON_SHRINE_ROCK_03 },
    { { SCENE_16GORON_HOUSE, 0, 3 }, RC_GORON_SHRINE_ROCK_04 },
    { { SCENE_16GORON_HOUSE, 0, 9 }, RC_GORON_SHRINE_ROCK_05 },
    { { SCENE_16GORON_HOUSE, 0, 8 }, RC_GORON_SHRINE_ROCK_06 },
    { { SCENE_16GORON_HOUSE, 0, 7 }, RC_GORON_SHRINE_ROCK_07 },
    { { SCENE_16GORON_HOUSE, 0, 6 }, RC_GORON_SHRINE_ROCK_08 },
    { { SCENE_16GORON_HOUSE, 0, 15 }, RC_GORON_SHRINE_ROCK_09 },
    { { SCENE_16GORON_HOUSE, 0, 14 }, RC_GORON_SHRINE_ROCK_10 },
    { { SCENE_16GORON_HOUSE, 0, 16 }, RC_GORON_SHRINE_ROCK_11 },
    { { SCENE_16GORON_HOUSE, 0, 11 }, RC_GORON_SHRINE_ROCK_12 },
    { { SCENE_16GORON_HOUSE, 0, 10 }, RC_GORON_SHRINE_ROCK_13 },
    { { SCENE_16GORON_HOUSE, 0, 13 }, RC_GORON_SHRINE_ROCK_14 },
    { { SCENE_16GORON_HOUSE, 0, 17 }, RC_GORON_SHRINE_ROCK_15 },
    { { SCENE_16GORON_HOUSE, 0, 12 }, RC_GORON_SHRINE_ROCK_16 },
    // Great Bay Coast
    { { SCENE_30GYOSON, 0, 102 }, RC_GREAT_BAY_COAST_CLIFFSIDE_ROCK_1 },
    { { SCENE_30GYOSON, 0, 100 }, RC_GREAT_BAY_COAST_CLIFFSIDE_ROCK_2 },
    { { SCENE_30GYOSON, 0, 96 }, RC_GREAT_BAY_COAST_SHORE_ROCK_1 },
    { { SCENE_30GYOSON, 0, 97 }, RC_GREAT_BAY_COAST_SHORE_ROCK_2 },
    { { SCENE_30GYOSON, 0, 98 }, RC_GREAT_BAY_COAST_SHORE_ROCK_3 },
    { { SCENE_30GYOSON, 0, 99 }, RC_GREAT_BAY_COAST_SHORE_ROCK_4 },
    { { SCENE_30GYOSON, 0, 95 }, RC_GREAT_BAY_COAST_SHORE_ROCK_5 },
    { { SCENE_30GYOSON, 0, 67 }, RC_GREAT_BAY_COAST_UNDERWATER_SHORE_ROCK_1 },
    { { SCENE_30GYOSON, 0, 68 }, RC_GREAT_BAY_COAST_UNDERWATER_SHORE_ROCK_2 },
    { { SCENE_30GYOSON, 0, 93 }, RC_GREAT_BAY_COAST_UNDERWATER_SHORE_ROCK_3 },
    { { SCENE_30GYOSON, 0, 92 }, RC_GREAT_BAY_COAST_UNDERWATER_SHORE_ROCK_4 },
    { { SCENE_30GYOSON, 0, 103 }, RC_GREAT_BAY_COAST_UNDERWATER_TIDE_POOL_ROCK },
    // Ikana Graveyard
    { { SCENE_BOTI, 1, 34 }, RC_IKANA_GRAVEYARD_WALL_ROCK_1 },
    { { SCENE_BOTI, 1, 36 }, RC_IKANA_GRAVEYARD_WALL_ROCK_2 },
    { { SCENE_BOTI, 1, 37 }, RC_IKANA_GRAVEYARD_WALL_ROCK_3 },
    { { SCENE_BOTI, 1, 38 }, RC_IKANA_GRAVEYARD_WALL_ROCK_4 },
    { { SCENE_BOTI, 1, 35 }, RC_IKANA_GRAVEYARD_WALL_ROCK_5 },
    // Mountain Village (Spring)
    { { SCENE_10YUKIYAMANOMURA2, 0, 14 }, RC_MOUNTAIN_VILLAGE_SPRING_GRAVEYARD_ROCK_1 },
    { { SCENE_10YUKIYAMANOMURA2, 0, 15 }, RC_MOUNTAIN_VILLAGE_SPRING_GRAVEYARD_ROCK_2 },
    { { SCENE_10YUKIYAMANOMURA2, 0, 19 }, RC_MOUNTAIN_VILLAGE_SPRING_ROCK_1 },
    { { SCENE_10YUKIYAMANOMURA2, 0, 18 }, RC_MOUNTAIN_VILLAGE_SPRING_ROCK_2 },
    { { SCENE_10YUKIYAMANOMURA2, 0, 20 }, RC_MOUNTAIN_VILLAGE_SPRING_ROCK_3 },
    { { SCENE_10YUKIYAMANOMURA2, 0, 16 }, RC_MOUNTAIN_VILLAGE_SPRING_ROCK_4 },
    { { SCENE_10YUKIYAMANOMURA2, 0, 17 }, RC_MOUNTAIN_VILLAGE_SPRING_ROCK_5 },
    // Inverted Stone Tower
    { { SCENE_F41, 0, 21 }, RC_STONE_TOWER_INVERTED_ROCK_1 },
    { { SCENE_F41, 0, 22 }, RC_STONE_TOWER_INVERTED_ROCK_2 },
    // Swamp Spider House
    { { SCENE_KINSTA1, 4, 5 }, RC_SWAMP_SPIDER_HOUSE_JAR_ROOM_ROCK },
    { { SCENE_KINSTA1, 0, 3 }, RC_SWAMP_SPIDER_HOUSE_ENTRANCE_ROCK_1 },
    { { SCENE_KINSTA1, 0, 4 }, RC_SWAMP_SPIDER_HOUSE_ENTRANCE_ROCK_2 },
    // Termina Field
    { { SCENE_KAKUSIANA, 11, 1 }, RC_TERMINA_FIELD_BIO_BABA_GROTTO_ROCK },
    { { SCENE_00KEIKOKU, 0, 127 }, RC_TERMINA_FIELD_GREAT_BAY_COAST_ROCK },
    { { SCENE_00KEIKOKU, 0, 138 }, RC_TERMINA_FIELD_WALL_ROCK_01 },
    { { SCENE_00KEIKOKU, 0, 131 }, RC_TERMINA_FIELD_WALL_ROCK_10 },
    { { SCENE_00KEIKOKU, 0, 130 }, RC_TERMINA_FIELD_WALL_ROCK_11 },
    { { SCENE_00KEIKOKU, 0, 135 }, RC_TERMINA_FIELD_WALL_ROCK_12 },
    { { SCENE_00KEIKOKU, 0, 137 }, RC_TERMINA_FIELD_WALL_ROCK_02 },
    { { SCENE_00KEIKOKU, 0, 133 }, RC_TERMINA_FIELD_WALL_ROCK_03 },
    { { SCENE_00KEIKOKU, 0, 132 }, RC_TERMINA_FIELD_WALL_ROCK_04 },
    { { SCENE_00KEIKOKU, 0, 129 }, RC_TERMINA_FIELD_WALL_ROCK_05 },
    { { SCENE_00KEIKOKU, 0, 136 }, RC_TERMINA_FIELD_WALL_ROCK_06 },
    { { SCENE_00KEIKOKU, 0, 139 }, RC_TERMINA_FIELD_WALL_ROCK_07 },
    { { SCENE_00KEIKOKU, 0, 134 }, RC_TERMINA_FIELD_WALL_ROCK_08 },
    { { SCENE_00KEIKOKU, 0, 128 }, RC_TERMINA_FIELD_WALL_ROCK_09 },
    // Twin Islands
    { { SCENE_17SETUGEN2, 0, 25 }, RC_TWIN_ISLANDS_SPRING_ROCK_1 },
    { { SCENE_17SETUGEN2, 0, 24 }, RC_TWIN_ISLANDS_SPRING_ROCK_2 },
    { { SCENE_17SETUGEN2, 0, 26 }, RC_TWIN_ISLANDS_SPRING_ROCK_3 },
    // Zora Cape
    { { SCENE_31MISAKI, 0, 54 }, RC_ZORA_CAPE_SHORE_ROCK_01 },
    { { SCENE_31MISAKI, 0, 41 }, RC_ZORA_CAPE_SHORE_ROCK_10 },
    { { SCENE_31MISAKI, 0, 53 }, RC_ZORA_CAPE_SHORE_ROCK_02 },
    { { SCENE_31MISAKI, 0, 55 }, RC_ZORA_CAPE_SHORE_ROCK_03 },
    { { SCENE_31MISAKI, 0, 56 }, RC_ZORA_CAPE_SHORE_ROCK_04 },
    { { SCENE_31MISAKI, 0, 57 }, RC_ZORA_CAPE_SHORE_ROCK_05 },
    { { SCENE_31MISAKI, 0, 40 }, RC_ZORA_CAPE_SHORE_ROCK_06 },
    { { SCENE_31MISAKI, 0, 38 }, RC_ZORA_CAPE_SHORE_ROCK_07 },
    { { SCENE_31MISAKI, 0, 37 }, RC_ZORA_CAPE_SHORE_ROCK_08 },
    { { SCENE_31MISAKI, 0, 39 }, RC_ZORA_CAPE_SHORE_ROCK_09 },
    { { SCENE_31MISAKI, 0, 36 }, RC_ZORA_CAPE_TREE_ISLAND_ROCK_1 },
    { { SCENE_31MISAKI, 0, 34 }, RC_ZORA_CAPE_TREE_ISLAND_ROCK_2 },
    { { SCENE_31MISAKI, 0, 32 }, RC_ZORA_CAPE_TREE_ISLAND_ROCK_3 },
    { { SCENE_31MISAKI, 0, 33 }, RC_ZORA_CAPE_TREE_ISLAND_ROCK_4 },
    { { SCENE_31MISAKI, 0, 35 }, RC_ZORA_CAPE_TREE_ISLAND_ROCK_5 },
};

// Rock Rings
std::map<std::tuple<s16, s16, s16>, RandoCheckId> objMure2RockMap = {
    { { SCENE_00KEIKOKU, 0, 140 }, RC_TERMINA_FIELD_KAMARO_ROCK_1 },
    { { SCENE_IKANAMAE, 0, 35 }, RC_ROAD_TO_IKANA_SHIRO_ROCK_1 },
    { { SCENE_BOTI, 1, 44 }, RC_IKANA_GRAVEYARD_RING_ROCK_1 },
};

// Goron Shrine Cleared Indices
std::map<s16, s16> goronShrineReplacementIndices = {
    { 7, 2 },
    { 9, 4 },
    { 10, 5 },
    { 8, 3 },
    { 14, 9 },
    { 13, 8 },
    { 12, 7 },
    { 11, 6 },
    { 19, 15 },
    { 20, 14 },
    { 21, 16 },
    { 16, 11 },
    { 15, 10 },
    { 18, 13 },
    { 22, 17 },
    { 17, 12 },
};
// clang-format on

void EnIshi_RandoDraw(Actor* actor, PlayState* play) {
    /* to be uncommented if/when no rando containers is pulled
    if (!CVarGetInteger("gRando.RandoContainers", 0)) {
        Gfx_DrawDListOpa(play, (Gfx*)gSmallRockDL);
        return;
    }
    */
    if (!CVarGetInteger("gRando.CSMC", 0)) {
        Gfx_DrawDListOpa(play, (Gfx*)gRockJunkDL);
        return;
    }
    RandoCheckId randoCheckId = Rando::ActorBehavior::GetObjectRandoCheckId(actor);
    RandoItemId randoItemId = Rando::ConvertItem(RANDO_SAVE_CHECKS[randoCheckId].randoItemId, randoCheckId);
    RandoItemType randoItemType = Rando::StaticData::Items[randoItemId].randoItemType;

    switch (randoItemType) {
        case RITYPE_BOSS_KEY:
            Gfx_DrawDListOpa(play, (Gfx*)gRockBossKeyDL);
            break;
        case RITYPE_HEALTH:
            Gfx_DrawDListOpa(play, (Gfx*)gRockHealthDL);
            break;
        case RITYPE_LESSER:
            Gfx_DrawDListOpa(play, (Gfx*)gRockLesserDL);
            break;
        case RITYPE_MAJOR:
            Gfx_DrawDListOpa(play, (Gfx*)gRockMajorDL);
            break;
        case RITYPE_MASK:
            Gfx_DrawDListOpa(play, (Gfx*)gRockMaskDL);
            break;
        case RITYPE_SKULLTULA_TOKEN:
            Gfx_DrawDListOpa(play, (Gfx*)gRockTokenDL);
            break;
        case RITYPE_SMALL_KEY:
            Gfx_DrawDListOpa(play, (Gfx*)gRockSmallKeyDL);
            break;
        case RITYPE_STRAY_FAIRY:
            Gfx_DrawDListOpa(play, (Gfx*)gRockFairyDL);
            break;
        case RITYPE_JUNK:
            Gfx_DrawDListOpa(play, (Gfx*)gRockJunkDL);
            break;
        default:
            Gfx_DrawDListOpa(play, (Gfx*)gRockJunkDL);
            break;
    }
}

void SpawnRockDrop(Vec3f pos, RandoCheckId randoCheckId) {
    CustomItem::Spawn(
        pos.x, pos.y, pos.z, 0, CustomItem::KILL_ON_TOUCH | CustomItem::TOSS_ON_SPAWN | CustomItem::ABLE_TO_ZORA_RANG,
        randoCheckId,
        [](Actor* actor, PlayState* play) {
            auto& randoStaticCheck = Rando::StaticData::Checks[(RandoCheckId)CUSTOM_ITEM_PARAM];
            switch (randoStaticCheck.flagType) {
                case FLAG_NONE:
                    RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM].eligible = true;
                    break;
                case FLAG_CYCL_SCENE_COLLECTIBLE:
                    Flags_SetCollectible(play, randoStaticCheck.flag);
                    break;
                default:
                    break;
            }
        },
        [](Actor* actor, PlayState* play) {
            auto& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
            Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
            Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, (RandoCheckId)CUSTOM_ITEM_PARAM),
                            (RandoCheckId)CUSTOM_ITEM_PARAM, actor);
        });
}

void Rando::ActorBehavior::InitEnIshiBehavior() {
    bool shouldRegister = IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_ROCK_DROPS];
    // Catches rocks that are part of a scene
    COND_ID_HOOK(OnActorInit, ACTOR_EN_ISHI, shouldRegister, [](Actor* actor) {
        s16 actorIndex = GetActorListIndex(actor);
        if (gPlayState->sceneId == SCENE_16GORON_HOUSE && CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_SNOWHEAD_TEMPLE)) {
            auto repEntry = goronShrineReplacementIndices.find({ actorIndex });
            if (repEntry != goronShrineReplacementIndices.end()) {
                actorIndex = repEntry->second;
            }
        }
        auto it =
            enIshiItemMap.find({ gPlayState->sceneId, gPlayState->roomCtx.curRoom.num, actorIndex });
        if (it != enIshiItemMap.end()) {
            SetObjectRandoCheckId(actor, it->second);
        }
    });
    // Catches rocks that are spawned in rings
    COND_VB_SHOULD(VB_OBJ_MURE2_SET_CHILD_ROOM, shouldRegister, {
        Actor* actor = va_arg(args, Actor*);
        ObjMure2* objMure2 = (ObjMure2*)actor;
        s32 i = va_arg(args, s32);
        Actor* child = objMure2->actors[i];
        if (child != nullptr && child->id == ACTOR_EN_ISHI) {
            auto it = objMure2RockMap.find({ gPlayState->sceneId, actor->room, GetActorListIndex(actor) });
            if (it != objMure2RockMap.end()) {
                SetObjectRandoCheckId(child, static_cast<RandoCheckId>(it->second + i));
            }
        }
    });
    // Overrides spawn behavior of rock drops
    COND_VB_SHOULD(VB_ROCK_DROP_COLLECTIBLE, shouldRegister, {
        EnIshi* enIshi = va_arg(args, EnIshi*);
        RandoCheckId randoCheckId = GetObjectRandoCheckId(enIshi);
        if (randoCheckId != RC_UNKNOWN && RANDO_SAVE_CHECKS[randoCheckId].shuffled &&
            !RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
            SpawnRockDrop(enIshi->actor.world.pos, randoCheckId);
            *should = false;
        }
    });
    COND_VB_SHOULD(VB_ISHI_DRAW_BE_OVERRIDDEN, shouldRegister, {
        Actor* actor = va_arg(args, Actor*);
        RandoCheckId randoCheckId = GetObjectRandoCheckId(actor);
        if (randoCheckId != RC_UNKNOWN && !RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
            *should = false;
            actor->draw = EnIshi_RandoDraw;
        }
    });
}