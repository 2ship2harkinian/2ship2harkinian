#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/ObjectExtension/ActorListIndex.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

extern "C" {
void EnKanban_Draw(Actor* thisx, PlayState* play);
}

// clang-format off
std::map<std::tuple<s16, s16, s16, s16>, RandoCheckId> enKanbanMap = {
    // East Clock Town
    { { ACTOR_EN_KANBAN, SCENE_TOWN,             0,  56  }, RC_CLOCK_TOWN_EAST_ROOF_SIGN },
    // North Clock Town
    { { ACTOR_EN_KANBAN, SCENE_BACKTOWN,         0,  31  }, RC_CLOCK_TOWN_NORTH_PLAYGROUND_SIGN },
    // Termina Field
    { { ACTOR_EN_KANBAN, SCENE_00KEIKOKU,        0,  141 }, RC_TERMINA_FIELD_MILK_ROAD_SIGN },
    // Road to Southern Swamp
    { { ACTOR_EN_KANBAN, SCENE_24KEMONOMITI,     0,  32  }, RC_ROAD_TO_SOUTHERN_SWAMP_SHOOTING_GALLERY_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_24KEMONOMITI,     0,  33  }, RC_ROAD_TO_SOUTHERN_SWAMP_WATER_SIGN },
    // Southern Swamp
    { { ACTOR_EN_KANBAN, SCENE_20SICHITAI,       0,  8   }, RC_SOUTHERN_SWAMP_TOURIST_DOCK_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_20SICHITAI,       2,  5   }, RC_SOUTHERN_SWAMP_HAGS_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_20SICHITAI,       2,  4   }, RC_SOUTHERN_SWAMP_WOODS_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_20SICHITAI,       0,  9   }, RC_SOUTHERN_SWAMP_FLOWER_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_20SICHITAI,       1,  11  }, RC_SOUTHERN_SWAMP_SPIDER_HOUSE_SIGN },
    // Woods of Mystery
    { { ACTOR_EN_KANBAN, SCENE_26SARUNOMORI,     7,  12  }, RC_WOODS_OF_MYSTERY_SIGN_01 },
    { { ACTOR_EN_KANBAN, SCENE_26SARUNOMORI,     7,  13  }, RC_WOODS_OF_MYSTERY_SIGN_02 },
    { { ACTOR_EN_KANBAN, SCENE_26SARUNOMORI,     7,  14  }, RC_WOODS_OF_MYSTERY_SIGN_03 },
    // Deku Palace
    { { ACTOR_EN_KANBAN, SCENE_22DEKUCITY,       0,  13  }, RC_DEKU_PALACE_PALACE_SIGN_01 },
    { { ACTOR_EN_KANBAN, SCENE_22DEKUCITY,       0,  14  }, RC_DEKU_PALACE_PALACE_SIGN_02 },
    { { ACTOR_EN_KANBAN, SCENE_22DEKUCITY,       0,  15  }, RC_DEKU_PALACE_ROYAL_CHAMBER_SIGN },
    // Swamp Spider House
    { { ACTOR_EN_KANBAN, SCENE_KINSTA1,          1,  14  }, RC_SWAMP_SPIDER_HOUSE_MAIN_ROOM_SIGN },
    //Doggy Racetrack
    { { ACTOR_EN_KANBAN, SCENE_F01_B,            0,  2   }, RC_DOGGY_RACETRACK_SIGN },
    // Path to Mountain Village
    { { ACTOR_EN_KANBAN, SCENE_13HUBUKINOMITI,   0,  9   }, RC_PATH_TO_MOUNTAIN_VILLAGE_SIGN },
    // Mountain Village
    { { ACTOR_EN_KANBAN, SCENE_10YUKIYAMANOMURA, 0,  3   }, RC_MOUNTAIN_VILLAGE_SMITHY_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_10YUKIYAMANOMURA, 0,  6   }, RC_MOUNTAIN_VILLAGE_BRIDGE_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_10YUKIYAMANOMURA, 0,  5   }, RC_MOUNTAIN_VILLAGE_SNOWHEAD_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_10YUKIYAMANOMURA, 0,  7   }, RC_MOUNTAIN_VILLAGE_WATER_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_10YUKIYAMANOMURA, 0,  4   }, RC_MOUNTAIN_VILLAGE_GORON_VILLAGE_SIGN },
    // Path to Goron Village
    { { ACTOR_EN_KANBAN, SCENE_17SETUGEN,        0,  23  }, RC_TWIN_ISLANDS_SLOPE_SIGN },
    // Goron Village
    { { ACTOR_EN_KANBAN, SCENE_11GORONNOSATO,    0,  43  }, RC_GORON_VILLAGE_POWDER_KEG_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_11GORONNOSATO,    0,  41  }, RC_GORON_VILLAGE_GORON_SHRINE_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_11GORONNOSATO,    0,  42  }, RC_GORON_VILLAGE_LONE_PEAK_SIGN },
    // Path to Snowhead
    { { ACTOR_EN_KANBAN, SCENE_14YUKIDAMANOMITI, 0,  13  }, RC_PATH_TO_SNOWHEAD_LOWER_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_14YUKIDAMANOMITI, 0,  12  }, RC_PATH_TO_SNOWHEAD_UPPER_SIGN },
    // Snowhead
    { { ACTOR_EN_KANBAN, SCENE_12HAKUGINMAE,     0,  30  }, RC_SNOWHEAD_SIGN },
    // Ikana Canyon
    { { ACTOR_EN_KANBAN, SCENE_IKANA,            0,  52  }, RC_IKANA_CANYON_DOCK_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_IKANA,            2,  2   }, RC_IKANA_CANYON_WATERFALL_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_IKANA,            4,  5   }, RC_IKANA_CANYON_HIDEOUT_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_IKANA,            0,  56  }, RC_IKANA_CANYON_CASTLE_GATE_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_IKANA,            0,  54  }, RC_IKANA_CANYON_CAVE_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_IKANA,            0,  55  }, RC_IKANA_CANYON_SPIRIT_HOUSE_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_IKANA,            0,  53  }, RC_IKANA_CANYON_WELL_SIGN },
    // Great Bay Coast
    { { ACTOR_EN_KANBAN, SCENE_30GYOSON,         0,  155 }, RC_GREAT_BAY_COAST_NEAR_TERMINA_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_30GYOSON,         0,  150 }, RC_GREAT_BAY_COAST_FISHERMANS_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_30GYOSON,         0,  156 }, RC_GREAT_BAY_COAST_OCEAN_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_30GYOSON,         0,  154 }, RC_GREAT_BAY_COAST_ZORA_CAPE_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_30GYOSON,         0,  151 }, RC_GREAT_BAY_COAST_PIRATES_COVE_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_30GYOSON,         0,  152 }, RC_GREAT_BAY_COAST_LAB_SIGN },
    // Zora Cape
    { { ACTOR_EN_KANBAN, SCENE_31MISAKI,         0,  78  }, RC_ZORA_CAPE_GAME_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_31MISAKI,         0,  75  }, RC_ZORA_CAPE_WATERFALL_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_31MISAKI,         0,  77  }, RC_ZORA_CAPE_GREAT_BAY_TEMPLE_SIGN },
    // Bean Grotto
    { { ACTOR_EN_KANBAN, SCENE_KAKUSIANA,        12, 5   }, RC_DEKU_PALACE_BEAN_GROTTO_SIGN },

    // Duplicates in world state (poison/clear or winter/spring)
    // Southern Swamp
    { { ACTOR_EN_KANBAN, SCENE_20SICHITAI2,       0, 8  }, RC_SOUTHERN_SWAMP_TOURIST_DOCK_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_20SICHITAI2,       2, 5  }, RC_SOUTHERN_SWAMP_HAGS_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_20SICHITAI2,       2, 4  }, RC_SOUTHERN_SWAMP_WOODS_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_20SICHITAI2,       1, 3  }, RC_SOUTHERN_SWAMP_SPIDER_HOUSE_SIGN },
    // Mountain Village
    { { ACTOR_EN_KANBAN, SCENE_10YUKIYAMANOMURA2, 0, 21 }, RC_MOUNTAIN_VILLAGE_SMITHY_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_10YUKIYAMANOMURA2, 0, 22 }, RC_MOUNTAIN_VILLAGE_BRIDGE_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_10YUKIYAMANOMURA2, 0, 24 }, RC_MOUNTAIN_VILLAGE_SNOWHEAD_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_10YUKIYAMANOMURA2, 0, 25 }, RC_MOUNTAIN_VILLAGE_WATER_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_10YUKIYAMANOMURA2, 0, 23 }, RC_MOUNTAIN_VILLAGE_GORON_VILLAGE_SIGN },
    // Path to Goron Village
    { { ACTOR_EN_KANBAN, SCENE_17SETUGEN2,        0, 10 }, RC_TWIN_ISLANDS_SLOPE_SIGN },
    // Goron Village
    { { ACTOR_EN_KANBAN, SCENE_11GORONNOSATO2,    0, 4  }, RC_GORON_VILLAGE_POWDER_KEG_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_11GORONNOSATO2,    0, 2  }, RC_GORON_VILLAGE_GORON_SHRINE_SIGN },
    { { ACTOR_EN_KANBAN, SCENE_11GORONNOSATO2,    0, 3  }, RC_GORON_VILLAGE_LONE_PEAK_SIGN },
};
// clang-format on

void DrawKanban(Actor* actor, PlayState* play) {
    EnKanban_Draw(actor, play);

    OPEN_DISPS(gPlayState->state.gfxCtx);
    RandoCheckId randoCheckId = Rando::ActorBehavior::GetObjectRandoCheckId(actor);
    Matrix_Push();
    Matrix_Translate(actor->world.pos.x, actor->world.pos.y + 69.0f, actor->world.pos.z, MTXMODE_NEW);
    Matrix_RotateYS(actor->shape.rot.y, MTXMODE_APPLY);
    Matrix_Scale(0.3f, 0.3f, 0.3f, MTXMODE_APPLY);
    Rando::DrawItem(Rando::ConvertItem(RANDO_SAVE_CHECKS[randoCheckId].randoItemId, randoCheckId), randoCheckId, actor);
    Matrix_Pop();
    CLOSE_DISPS(gPlayState->state.gfxCtx);
}

void EnKanbanInit(Actor* actor) {
    RandoCheckId randoCheckId = RC_UNKNOWN;
    auto it =
        enKanbanMap.find({ actor->id, gPlayState->sceneId, gPlayState->roomCtx.curRoom.num, GetActorListIndex(actor) });
    if (it != enKanbanMap.end()) {
        randoCheckId = it->second;
    }

    if (!RANDO_SAVE_CHECKS[randoCheckId].shuffled || RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
        return;
    }

    Rando::ActorBehavior::SetObjectRandoCheckId(actor, randoCheckId);
}

void Rando::ActorBehavior::InitEnKanbanBehavior() {
    bool shouldRegister = IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_SIGNPOSTS];

    COND_ID_HOOK(OnActorInit, ACTOR_EN_KANBAN, shouldRegister, EnKanbanInit);
    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_KANBAN, shouldRegister, [](Actor* actor) {
        RandoCheckId randoCheckId = Rando::ActorBehavior::GetObjectRandoCheckId(actor);
        if (!RANDO_SAVE_CHECKS[randoCheckId].shuffled || RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
            actor->draw = EnKanban_Draw;
            return;
        }

        actor->draw = DrawKanban;
    });

    COND_VB_SHOULD(VB_PLAYER_START_TALKING, shouldRegister, {
        Actor* talkActor = GET_PLAYER(gPlayState)->talkActor;
        if (talkActor->id != ACTOR_EN_KANBAN) {
            return;
        }
        RandoCheckId randoCheckId = Rando::ActorBehavior::GetObjectRandoCheckId(talkActor);
        RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];

        if (!randoSaveCheck.shuffled || randoSaveCheck.cycleObtained) {
            return;
        }

        randoSaveCheck.eligible = true;
        *should = false;
    });
}
