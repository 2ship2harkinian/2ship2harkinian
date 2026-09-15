#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/ObjectExtension/ActorListIndex.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_Obj_Etcetera/z_obj_etcetera.h"
#include "overlays/actors/ovl_Obj_Rotlift/z_obj_rotlift.h"
}

#define SHUFFLED_DEKU_FLOWERS (IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_DEKU_FLOWERS])
#define IS_AT(xx, zz) (actor->home.pos.x == xx && actor->home.pos.z == zz)

// clang-format off
std::map < std::tuple<s16, s16, s16, s16>, RandoCheckId> objEtceteraMap {
    //Termina Field
    { { ACTOR_OBJ_ETCETERA, SCENE_00KEIKOKU, 0, 47 }, RC_TERMINA_FIELD_NEAR_HOLLOW_LOG_RAMP_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_00KEIKOKU, 0, 48 }, RC_TERMINA_FIELD_NEAR_CHEST_STUMP_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_00KEIKOKU, 0, 49 }, RC_TERMINA_FIELD_NEAR_SWAMP_PATH_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_00KEIKOKU, 0, 50 }, RC_TERMINA_FIELD_NEAR_OBSERVATORY_DEKU_FLOWER, },
	//South Clock Town
	{ { ACTOR_OBJ_ETCETERA, SCENE_CLOCKTOWER, 0, 4 }, RC_CLOCK_TOWN_SOUTH_SCRUB_DEKU_FLOWER, },
    //North Clock Town
    { { ACTOR_OBJ_ETCETERA, SCENE_BACKTOWN, 0, 2 }, RC_CLOCK_TOWN_NORTH_OUTSIDE_FENCE_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_BACKTOWN, 0, 3 }, RC_CLOCK_TOWN_NORTH_INSIDE_FENCE_DEKU_FLOWER, },
    //Deku Playground
    { { ACTOR_OBJ_ETCETERA, SCENE_DEKUTES, 0, 0 }, RC_DEKU_PLAYGROUND_SCRUB_DEKU_FLOWER_1, },
    { { ACTOR_OBJ_ETCETERA, SCENE_DEKUTES, 0, 1 }, RC_DEKU_PLAYGROUND_SCRUB_DEKU_FLOWER_2, },
    //East Clock Town
    { { ACTOR_OBJ_ETCETERA, SCENE_TOWN, 0, 9 }, RC_CLOCK_TOWN_EAST_NEAR_INN_DEKU_FLOWER, },
    //Road to Southern Swamp
    { { ACTOR_OBJ_ETCETERA, SCENE_24KEMONOMITI, 0, 18 }, RC_ROAD_TO_SOUTHERN_SWAMP_PATH_TO_TERMINA_DEKU_FLOWER_1, },
    { { ACTOR_OBJ_ETCETERA, SCENE_24KEMONOMITI, 0, 19 }, RC_ROAD_TO_SOUTHERN_SWAMP_PATH_TO_TERMINA_DEKU_FLOWER_2, },
    { { ACTOR_OBJ_ETCETERA, SCENE_24KEMONOMITI, 0, 17 }, RC_ROAD_TO_SOUTHERN_SWAMP_PATH_TO_TERMINA_DEKU_FLOWER_3, },
    { { ACTOR_OBJ_ETCETERA, SCENE_24KEMONOMITI, 0, 20 }, RC_ROAD_TO_SOUTHERN_SWAMP_PATH_TO_ARCHERY_DEKU_FLOWER_1, },
    { { ACTOR_OBJ_ETCETERA, SCENE_24KEMONOMITI, 0, 21 }, RC_ROAD_TO_SOUTHERN_SWAMP_PATH_TO_ARCHERY_DEKU_FLOWER_2, },
    { { ACTOR_OBJ_ETCETERA, SCENE_24KEMONOMITI, 0, 22 }, RC_ROAD_TO_SOUTHERN_SWAMP_PATH_TO_ARCHERY_DEKU_FLOWER_3, },
    { { ACTOR_OBJ_ETCETERA, SCENE_24KEMONOMITI, 0, 16 }, RC_ROAD_TO_SOUTHERN_SWAMP_PATH_TO_SWAMP_DEKU_FLOWER_1, },
    { { ACTOR_OBJ_ETCETERA, SCENE_24KEMONOMITI, 0, 15 }, RC_ROAD_TO_SOUTHERN_SWAMP_PATH_TO_SWAMP_DEKU_FLOWER_2, },
    //Southern Swamp (poison and clear)
    { { ACTOR_OBJ_ETCETERA, SCENE_20SICHITAI, 0, 5 }, RC_SOUTHERN_SWAMP_SCRUB_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_20SICHITAI, 1, 1 }, RC_SOUTHERN_SWAMP_BIG_MUSHROOM_DEKU_FLOWER_1, },
    { { ACTOR_OBJ_ETCETERA, SCENE_20SICHITAI, 1, 0 }, RC_SOUTHERN_SWAMP_BIG_MUSHROOM_DEKU_FLOWER_2, },
    { { ACTOR_OBJ_ETCETERA, SCENE_20SICHITAI, 1, 2 }, RC_SOUTHERN_SWAMP_BIG_MUSHROOM_DEKU_FLOWER_3, },
    { { ACTOR_OBJ_ETCETERA, SCENE_20SICHITAI, 1, 3 }, RC_SOUTHERN_SWAMP_BIG_MUSHROOM_DEKU_FLOWER_4, },
    { { ACTOR_OBJ_ETCETERA, SCENE_20SICHITAI, 1, 4 }, RC_SOUTHERN_SWAMP_SOARING_STONE_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_20SICHITAI, 1, 5 }, RC_SOUTHERN_SWAMP_WOODFALL_ENTRANCE_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_20SICHITAI2, 0, 5 }, RC_SOUTHERN_SWAMP_SCRUB_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_20SICHITAI2, 1, 0 }, RC_SOUTHERN_SWAMP_SOARING_STONE_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_20SICHITAI2, 1, 1 }, RC_SOUTHERN_SWAMP_WOODFALL_ENTRANCE_DEKU_FLOWER, },
    //Woods of Mystery
    { { ACTOR_OBJ_ETCETERA, SCENE_26SARUNOMORI, 3, 3 }, RC_WOODS_OF_MYSTERY_DEKU_FLOWER_1, },
    { { ACTOR_OBJ_ETCETERA, SCENE_26SARUNOMORI, 3, 2 }, RC_WOODS_OF_MYSTERY_DEKU_FLOWER_2, },
    { { ACTOR_OBJ_ETCETERA, SCENE_26SARUNOMORI, 4, 1 }, RC_WOODS_OF_MYSTERY_DEKU_FLOWER_3, },
    { { ACTOR_OBJ_ETCETERA, SCENE_26SARUNOMORI, 5, 3 }, RC_WOODS_OF_MYSTERY_DEKU_FLOWER_4, },
    { { ACTOR_OBJ_ETCETERA, SCENE_26SARUNOMORI, 5, 2 }, RC_WOODS_OF_MYSTERY_DEKU_FLOWER_5, },
    { { ACTOR_OBJ_ETCETERA, SCENE_26SARUNOMORI, 7, 2 }, RC_WOODS_OF_MYSTERY_DEKU_FLOWER_6, },
    { { ACTOR_OBJ_ETCETERA, SCENE_26SARUNOMORI, 7, 3 }, RC_WOODS_OF_MYSTERY_DEKU_FLOWER_7, },
    //Deku Palace
    { { ACTOR_OBJ_ETCETERA, SCENE_22DEKUCITY, 0, 6 }, RC_DEKU_PALACE_WOODFALL_EXIT_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_22DEKUCITY, 1, 11 }, RC_DEKU_PALACE_UPPER_BEAN_SIDE_ENTRANCE_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_22DEKUCITY, 1, 10 }, RC_DEKU_PALACE_UPPER_BEAN_SIDE_CENTRAL_CONNECTOR_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_22DEKUCITY, 2, 15 }, RC_DEKU_PALACE_UPPER_CELL_SIDE_CENTRAL_CONNECTOR_DEKU_FLOWER, },
    //Woodfall
    { { ACTOR_OBJ_ETCETERA, SCENE_21MITURINMAE, 0, 1 }, RC_WOODFALL_OWL_STATUE_PLATFORM_DEKU_FLOWER, },
    //Woodfall Temple
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 2, 6 }, RC_WOODFALL_TEMPLE_ENTRANCE_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 2, 5 }, RC_WOODFALL_TEMPLE_ENTRANCE_SMALL_PLATFORM_1_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 2, 3 }, RC_WOODFALL_TEMPLE_ENTRANCE_CHEST_PLATFORM_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 2, 4 }, RC_WOODFALL_TEMPLE_ENTRANCE_SMALL_PLATFORM_2_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 1, 5 }, RC_WOODFALL_TEMPLE_MAIN_ROOM_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 4, 0 }, RC_WOODFALL_TEMPLE_COMPASS_ROOM_DEKU_FLOWER_1, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 4, 1 }, RC_WOODFALL_TEMPLE_COMPASS_ROOM_DEKU_FLOWER_2, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 4, 2 }, RC_WOODFALL_TEMPLE_COMPASS_ROOM_DEKU_FLOWER_3, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 10, 3 }, RC_WOODFALL_TEMPLE_PIT_ROOM_NEAR_STAIRS_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 10, 2 }, RC_WOODFALL_TEMPLE_PIT_ROOM_BALCONY_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 7, 1 }, RC_WOODFALL_TEMPLE_BOW_ROOM_DEKU_FLOWER_1, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 7, 2 }, RC_WOODFALL_TEMPLE_BOW_ROOM_DEKU_FLOWER_2, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 5, 2 }, RC_WOODFALL_TEMPLE_WATER_ROOM_NEAR_BOW_ROOM_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 5, 1 }, RC_WOODFALL_TEMPLE_WATER_ROOM_NEAR_BOSS_KEY_ROOM_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 6, 5 }, RC_WOODFALL_TEMPLE_MAP_ROOM_DEKU_FLOWER_1, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 6, 4 }, RC_WOODFALL_TEMPLE_MAP_ROOM_DEKU_FLOWER_2, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 6, 3 }, RC_WOODFALL_TEMPLE_MAP_ROOM_DEKU_FLOWER_3, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 8, 5 }, RC_WOODFALL_TEMPLE_BOSS_KEY_ROOM_DEKU_FLOWER_1, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 8, 1 }, RC_WOODFALL_TEMPLE_BOSS_KEY_ROOM_DEKU_FLOWER_2, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 8, 4 }, RC_WOODFALL_TEMPLE_BOSS_KEY_ROOM_DEKU_FLOWER_3, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 8, 2 }, RC_WOODFALL_TEMPLE_BOSS_KEY_ROOM_DEKU_FLOWER_4, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 8, 3 }, RC_WOODFALL_TEMPLE_BOSS_KEY_ROOM_DEKU_FLOWER_5, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 0, 16 }, RC_WOODFALL_TEMPLE_PRE_BOSS_ENTRANCE_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 0, 17 }, RC_WOODFALL_TEMPLE_PRE_BOSS_LOWER_ALCOVE_DEKU_FLOWER_1, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 0, 18 }, RC_WOODFALL_TEMPLE_PRE_BOSS_LOWER_ALCOVE_DEKU_FLOWER_2, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 0, 12 }, RC_WOODFALL_TEMPLE_PRE_BOSS_MID_ALCOVE_DEKU_FLOWER_1, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 0, 9 }, RC_WOODFALL_TEMPLE_PRE_BOSS_MID_ALCOVE_DEKU_FLOWER_2, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 0, 11 }, RC_WOODFALL_TEMPLE_PRE_BOSS_UPPER_ALCOVE_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 0, 10 }, RC_WOODFALL_TEMPLE_PRE_BOSS_MID_ROOM_PLATFORM_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 0, 13 }, RC_WOODFALL_TEMPLE_PRE_BOSS_UNDER_OVERHANG_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 0, 15 }, RC_WOODFALL_TEMPLE_PRE_BOSS_ON_TOP_OF_OVERHANG_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN, 0, 14 }, RC_WOODFALL_TEMPLE_PRE_BOSS_FLAMING_PILLAR_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_MITURIN_BS, 0, 1 }, RC_WOODFALL_TEMPLE_BOSS_ROOM_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_KINSTA1, 1, 0 }, RC_SWAMP_SPIDER_HOUSE_MAIN_ROOM_UPPER_RIGHT_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_KINSTA1, 1, 1 }, RC_SWAMP_SPIDER_HOUSE_MAIN_ROOM_UPPER_CENTER_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_KINSTA1, 1, 2 }, RC_SWAMP_SPIDER_HOUSE_MAIN_ROOM_UPPER_LEFT_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_KINSTA1, 4, 2 }, RC_SWAMP_SPIDER_HOUSE_BIG_POT_ROOM_UPPER_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_KINSTA1, 4, 12 }, RC_SWAMP_SPIDER_HOUSE_BIG_POT_ROOM_LOWER_DEKU_FLOWER, },
    //Goron Village 
    { { ACTOR_OBJ_ETCETERA, SCENE_11GORONNOSATO, 0, 7 }, RC_GORON_VILLAGE_SCRUB_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_11GORONNOSATO2, 0, 6 }, RC_GORON_VILLAGE_SCRUB_DEKU_FLOWER, },
    //Snowhead Temple
    { { ACTOR_OBJ_ETCETERA, SCENE_HAKUGIN, 5, 1 }, RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_LOWER_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_HAKUGIN, 5, 2 }, RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_UPPER_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_HAKUGIN, 4, 8 }, RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_ALCOVE_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_HAKUGIN, 4, 9 }, RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_PRE_BOSS_DOOR_DEKU_FLOWER, },
    //Zora Cape
    { { ACTOR_OBJ_ETCETERA, SCENE_31MISAKI, 0, 21 }, RC_ZORA_CAPE_NEAR_BEAVERS_DEKU_FLOWER, },
    //Zora Hall
    { { ACTOR_OBJ_ETCETERA, SCENE_BANDROOM, 2, 2 }, RC_ZORA_HALL_SCRUB_DEKU_FLOWER, },
    //Ikana Canyon
    { { ACTOR_OBJ_ETCETERA, SCENE_IKANA, 4, 2 }, RC_IKANA_CANYON_SCRUB_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_IKANA, 4, 3 }, RC_IKANA_CANYON_HEART_PIECE_DEKU_FLOWER, },
    //Beneath the Well
    { { ACTOR_OBJ_ETCETERA, SCENE_REDEAD, 2, 0 }, RC_BENEATH_THE_WELL_THREE_SPIKED_BARS_DEKU_FLOWER_1, },
    { { ACTOR_OBJ_ETCETERA, SCENE_REDEAD, 2, 1 }, RC_BENEATH_THE_WELL_THREE_SPIKED_BARS_DEKU_FLOWER_2, },
    //Ikana Castle
    { { ACTOR_OBJ_ETCETERA, SCENE_CASTLE, 0, 5 }, RC_ANCIENT_CASTLE_OF_IKANA_ROOF_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_CASTLE, 0, 6 }, RC_ANCIENT_CASTLE_OF_IKANA_PILLAR_OPPOSITE_SWITCH_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_CASTLE, 0, 4 }, RC_ANCIENT_CASTLE_OF_IKANA_FLAMING_PILLAR_DEKU_FLOWER_1, },
    { { ACTOR_OBJ_ETCETERA, SCENE_CASTLE, 0, 3 }, RC_ANCIENT_CASTLE_OF_IKANA_FLAMING_PILLAR_DEKU_FLOWER_2, },
    { { ACTOR_OBJ_ETCETERA, SCENE_CASTLE, 2, 1 }, RC_ANCIENT_CASTLE_OF_IKANA_LEFT_FIRST_ROOM_DEKU_FLOWER_1, },
    { { ACTOR_OBJ_ETCETERA, SCENE_CASTLE, 2, 3 }, RC_ANCIENT_CASTLE_OF_IKANA_LEFT_FIRST_ROOM_DEKU_FLOWER_2, },
    { { ACTOR_OBJ_ETCETERA, SCENE_CASTLE, 2, 2 }, RC_ANCIENT_CASTLE_OF_IKANA_LEFT_FIRST_ROOM_DEKU_FLOWER_3, },
    { { ACTOR_OBJ_ETCETERA, SCENE_CASTLE, 2, 4 }, RC_ANCIENT_CASTLE_OF_IKANA_LEFT_FIRST_ROOM_DEKU_FLOWER_4, },
    { { ACTOR_OBJ_ETCETERA, SCENE_CASTLE, 2, 5 }, RC_ANCIENT_CASTLE_OF_IKANA_LEFT_FIRST_ROOM_DEKU_FLOWER_5, },
    { { ACTOR_OBJ_ETCETERA, SCENE_CASTLE, 4, 7 }, RC_ANCIENT_CASTLE_OF_IKANA_LEFT_SECOND_ROOM_DEKU_FLOWER_1, },
    { { ACTOR_OBJ_ETCETERA, SCENE_CASTLE, 4, 8 }, RC_ANCIENT_CASTLE_OF_IKANA_LEFT_SECOND_ROOM_DEKU_FLOWER_2, },
    { { ACTOR_OBJ_ETCETERA, SCENE_CASTLE, 4, 9 }, RC_ANCIENT_CASTLE_OF_IKANA_LEFT_SECOND_ROOM_DEKU_FLOWER_3, },
    //Stone Tower Temple
    { { ACTOR_OBJ_ETCETERA, SCENE_INISIE_N, 9, 9 }, RC_STONE_TOWER_TEMPLE_WIND_ROOM_DEKU_FLOWER, },
    //Inverted Stone Tower Temple
    { { ACTOR_OBJ_ETCETERA, SCENE_INISIE_R, 3, 11 }, RC_STONE_TOWER_TEMPLE_INVERTED_WIND_ROOM_ENTRANCE_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_INISIE_R, 3, 12 }, RC_STONE_TOWER_TEMPLE_INVERTED_WIND_ROOM_MID_LEVEL_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_INISIE_R, 2, 0 }, RC_STONE_TOWER_TEMPLE_INVERTED_POE_WIZZROBE_SIDE_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_INISIE_R, 2, 1 }, RC_STONE_TOWER_TEMPLE_INVERTED_POE_MAZE_SIDE_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_INISIE_R, 1, 7 }, RC_STONE_TOWER_TEMPLE_INVERTED_UNDER_BRIDGE_POE_SIDE_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_INISIE_R, 1, 8 }, RC_STONE_TOWER_TEMPLE_INVERTED_UNDER_BRIDGE_POTS_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_INISIE_R, 1, 9 }, RC_STONE_TOWER_TEMPLE_INVERTED_UNDER_BRIDGE_GOMESS_DEKU_FLOWER, },
    //Moon Deku Trial
    { { ACTOR_OBJ_ETCETERA, SCENE_LAST_DEKU, 0, 7 }, RC_MOON_TRIAL_DEKU_ENTRANCE_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_LAST_DEKU, 0, 8 }, RC_MOON_TRIAL_DEKU_LEDGE_NEAR_ENTRANCE_DEKU_FLOWER_1, },
    { { ACTOR_OBJ_ETCETERA, SCENE_LAST_DEKU, 0, 9 }, RC_MOON_TRIAL_DEKU_LEDGE_NEAR_ENTRANCE_DEKU_FLOWER_2, },
    { { ACTOR_OBJ_ETCETERA, SCENE_LAST_DEKU, 0, 10 }, RC_MOON_TRIAL_DEKU_MID_ROOM_LEDGE_DEKU_FLOWER_1, },
    { { ACTOR_OBJ_ETCETERA, SCENE_LAST_DEKU, 0, 11 }, RC_MOON_TRIAL_DEKU_MID_ROOM_LEDGE_DEKU_FLOWER_2, },
    { { ACTOR_OBJ_ETCETERA, SCENE_LAST_DEKU, 0, 12 }, RC_MOON_TRIAL_DEKU_HEART_PIECE_LEDGE_DEKU_FLOWER, },
    { { ACTOR_OBJ_ETCETERA, SCENE_LAST_DEKU, 0, 13 }, RC_MOON_TRIAL_DEKU_EXIT_ROOM_LEDGE_DEKU_FLOWER, },
};

std::map < std::tuple<s16, s16, s16, s16>, RandoCheckId> objRailliftMap{
    //Deku Palace
    { { ACTOR_OBJ_RAILLIFT, SCENE_22DEKUCITY, 1, 8}, RC_DEKU_PALACE_UPPER_BEAN_SIDE_MOVING_PLATFORM_DEKU_FLOWER_1, },
    { { ACTOR_OBJ_RAILLIFT, SCENE_22DEKUCITY, 1, 9}, RC_DEKU_PALACE_UPPER_BEAN_SIDE_MOVING_PLATFORM_DEKU_FLOWER_2, },
    { { ACTOR_OBJ_RAILLIFT, SCENE_22DEKUCITY, 2, 14}, RC_DEKU_PALACE_UPPER_CELL_SIDE_MOVING_PLATFORM_DEKU_FLOWER_1, },
    { { ACTOR_OBJ_RAILLIFT, SCENE_22DEKUCITY, 2, 13}, RC_DEKU_PALACE_UPPER_CELL_SIDE_MOVING_PLATFORM_DEKU_FLOWER_2, },
    //Woodfall Temple
    { { ACTOR_OBJ_RAILLIFT, SCENE_MITURIN, 10, 0}, RC_WOODFALL_TEMPLE_PIT_ROOM_MOVING_PLATFORM_DEKU_FLOWER_1, },
    { { ACTOR_OBJ_RAILLIFT, SCENE_MITURIN, 10, 1}, RC_WOODFALL_TEMPLE_PIT_ROOM_MOVING_PLATFORM_DEKU_FLOWER_2, },
    { { ACTOR_OBJ_RAILLIFT, SCENE_MITURIN, 5, 0}, RC_WOODFALL_TEMPLE_WATER_ROOM_MOVING_PLATFORM_DEKU_FLOWER, },
};
// clang-format on

// For flowers that are identified based on being a bouncing pink flower, identify based on the home pos.
// This home position is inherited from the object that spawned it
RandoCheckId IdentifyEtceteraBasedOnPos(Actor* actor) {
    RandoCheckId randoCheckId = RC_UNKNOWN;
    switch (gPlayState->sceneId) {
        case SCENE_22DEKUCITY:
            if (IS_AT(681.0f, 1091.0f)) {
                randoCheckId = RC_DEKU_PALACE_UPPER_BEAN_SIDE_MAD_SCRUB_DEKU_FLOWER_1;
            } else if (IS_AT(681.0f, 1785.0f)) {
                randoCheckId = RC_DEKU_PALACE_UPPER_BEAN_SIDE_MAD_SCRUB_DEKU_FLOWER_2;
            } else if (IS_AT(-672.0f, 1702.0f)) {
                randoCheckId = RC_DEKU_PALACE_UPPER_CELL_SIDE_MAD_SCRUB_DEKU_FLOWER_1;
            } else if (IS_AT(-931.0f, 1149.0f)) {
                randoCheckId = RC_DEKU_PALACE_UPPER_CELL_SIDE_MAD_SCRUB_DEKU_FLOWER_2;
            }
            break;
        case SCENE_21MITURINMAE:
            if (IS_AT(-1155.0f, 203.0f)) {
                randoCheckId = RC_WOODFALL_MAD_SCRUB_DEKU_FLOWER_1;
            } else if (IS_AT(-1349.0f, 667.0f)) {
                randoCheckId = RC_WOODFALL_MAD_SCRUB_DEKU_FLOWER_2;
            } else if (IS_AT(-596.0f, 1204.0f)) {
                randoCheckId = RC_WOODFALL_MAD_SCRUB_DEKU_FLOWER_3;
            } else if (IS_AT(570.0f, 1100.0f)) {
                randoCheckId = RC_WOODFALL_MAD_SCRUB_DEKU_FLOWER_4;
            } else if (IS_AT(990.0f, 690.0f)) {
                randoCheckId = RC_WOODFALL_MAD_SCRUB_DEKU_FLOWER_5;
            } else if (IS_AT(1131.0f, 105.0f)) {
                randoCheckId = RC_WOODFALL_MAD_SCRUB_DEKU_FLOWER_6;
            } else if (IS_AT(932.0f, -310.0f)) {
                randoCheckId = RC_WOODFALL_MAD_SCRUB_DEKU_FLOWER_7;
            } else if (IS_AT(1212.0f, -935.0f)) {
                randoCheckId = RC_WOODFALL_MAD_SCRUB_DEKU_FLOWER_8;
            } else if (IS_AT(5.0f, 294.0f)) {
                randoCheckId = RC_WOODFALL_DEKU_FLOWER_AT_TEMPLE_ENTRANCE;
            }
            break;
    }
    return randoCheckId;
}

// For flowers spawned by the scene, identify based on actor id
RandoCheckId IdentifyEtceteraBasedOnId(Actor* actor) {
    auto it = objEtceteraMap.find(
        { actor->id, gPlayState->sceneId, gPlayState->roomCtx.curRoom.num, GetActorListIndex(actor) });
    if (it == objEtceteraMap.end()) {
        return RC_UNKNOWN;
    }
    return it->second;
}

RandoCheckId IdentifyEtceteraBasedOnParent(Actor* actor) {
    RandoCheckId randoCheckId = RC_UNKNOWN;
    ObjEtcetera* flower = (ObjEtcetera*)actor;
    Actor* parent = actor->parent;
    if (parent->id == ACTOR_OBJ_RAILLIFT) {
        auto it = objRailliftMap.find(
            { parent->id, gPlayState->sceneId, gPlayState->roomCtx.curRoom.num, GetActorListIndex(parent) });
        if (it == objRailliftMap.end()) {
            return RC_UNKNOWN;
        }
        randoCheckId = it->second;
    }
    if (parent->id == ACTOR_OBJ_ROTLIFT) {
        ObjRotlift* lift = (ObjRotlift*)parent;
        if (GetActorListIndex(parent) == 5) {
            if (lift->dekuFlowers[0] == flower) {
                randoCheckId = RC_MOON_TRIAL_DEKU_ROTATING_PLATFORM_SET_1_DEKU_FLOWER_1;
            } else {
                randoCheckId = RC_MOON_TRIAL_DEKU_ROTATING_PLATFORM_SET_1_DEKU_FLOWER_2;
            }
        } else if (GetActorListIndex(parent) == 6) {
            if (lift->dekuFlowers[0] == flower) {
                randoCheckId = RC_MOON_TRIAL_DEKU_ROTATING_PLATFORM_SET_2_GOLD_DEKU_FLOWER;
            } else {
                randoCheckId = RC_MOON_TRIAL_DEKU_ROTATING_PLATFORM_SET_2_PINK_DEKU_FLOWER;
            }
        }
    }
    return randoCheckId;
}

void Draw_ObjEtcetera_Idle_Rando(Actor* actor, PlayState* play) {
    ObjEtcetera* etcetera = (ObjEtcetera*)actor;

    OPEN_DISPS(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, etcetera->dList);

    CLOSE_DISPS(play->state.gfxCtx);
    Rando::ActorBehavior::SpawnDekuFlowerSparkle(actor);
}

void Draw_ObjEtcetera_Animated_Rando(Actor* actor, PlayState* play) {
    ObjEtcetera* etcetera = (ObjEtcetera*)actor;

    Gfx_SetupDL37_Opa(play->state.gfxCtx);
    SkelAnime_DrawOpa(play, etcetera->skelAnime.skeleton, etcetera->skelAnime.jointTable, NULL, NULL,
                      &etcetera->dyna.actor);
    Rando::ActorBehavior::SpawnDekuFlowerSparkle(actor);
}

// Exact copy of the vanilla function to break out of the rando idle -> animated -> idle loop after the check has been
// collected. Animated because the only way to collect the check is to launch out of the flower, which always sets the
// function to animated
void ObjEtcetera_DrawAnimated(Actor* actor, PlayState* play) {
    ObjEtcetera* etcetera = (ObjEtcetera*)actor;

    Gfx_SetupDL37_Opa(play->state.gfxCtx);
    SkelAnime_DrawOpa(play, etcetera->skelAnime.skeleton, etcetera->skelAnime.jointTable, NULL, NULL,
                      &etcetera->dyna.actor);
}

void Rando::ActorBehavior::SpawnDekuFlowerSparkle(Actor* actor) {
    if ((gGameState->frames % 4) != 0) {
        return;
    }

    static Vec3f sVelocity = { 0.0f, 3.0f, 0.0f };
    static Vec3f sAccel = { 0.0f, 0.0f, 0.0f };
    static Color_RGBA8 sPrimColor = { 255, 255, 255, 255 };
    static Color_RGBA8 sEnvColor = { 0, 255, 64, 255 };
    Vec3f newPos;

    newPos.x = Rand_CenteredFloat(10.0f) + actor->world.pos.x;
    newPos.y = (Rand_ZeroOne() * 10.0f) + actor->world.pos.y;
    newPos.z = Rand_CenteredFloat(10.0f) + actor->world.pos.z;

    EffectSsKirakira_SpawnDispersed(gPlayState, &newPos, &sVelocity, &sAccel, &sPrimColor, &sEnvColor, 5000, 16);
}

void Rando::ActorBehavior::InitObjEtceteraBehavior() {

    // Hijack the normal idle -> animated -> idle cycle of actor draw to rando specific ones
    COND_VB_SHOULD(VB_DRAW_ETCETERA_IDLE, SHUFFLED_DEKU_FLOWERS, {
        ObjEtcetera* etcetera = va_arg(args, ObjEtcetera*);
        if (etcetera->dyna.actor.draw == nullptr) {
            *should = true;
        } else if ((etcetera->dyna.actor.draw == Draw_ObjEtcetera_Idle_Rando) ||
                   (etcetera->dyna.actor.draw == Draw_ObjEtcetera_Animated_Rando)) {
            etcetera->dyna.actor.draw = Draw_ObjEtcetera_Idle_Rando;
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_DRAW_ETCETERA_ANIMATED, SHUFFLED_DEKU_FLOWERS, {
        ObjEtcetera* etcetera = va_arg(args, ObjEtcetera*);
        if (etcetera->dyna.actor.draw == nullptr) {
            *should = true;
        } else if ((etcetera->dyna.actor.draw == Draw_ObjEtcetera_Idle_Rando) ||
                   (etcetera->dyna.actor.draw == Draw_ObjEtcetera_Animated_Rando)) {
            etcetera->dyna.actor.draw = Draw_ObjEtcetera_Animated_Rando;
            *should = false;
        }
    });

    COND_ID_HOOK(OnActorUpdate, ACTOR_OBJ_ETCETERA, SHUFFLED_DEKU_FLOWERS, [](Actor* actor) {
        RandoCheckId randoCheckId = RC_UNKNOWN;
        ObjEtcetera* flower = (ObjEtcetera*)actor;
        Player* player = GET_PLAYER(gPlayState);
        // Without the oscillation timer check, the rest of the function fires 10 times per launch
        if ((player->stateFlags3 & PLAYER_STATE3_200) && (flower->dyna.actor.xzDistToPlayer < 20.0f) &&
            (flower->oscillationTimer >= 28)) {
            // All flowers spawned after scene init have their params set to 128 - Pink flower with bounce
            if (actor->params == DEKU_FLOWER_PARAMS(DEKU_FLOWER_TYPE_PINK_WITH_INITIAL_BOUNCE)) {
                randoCheckId = IdentifyEtceteraBasedOnPos(actor);
            } else if (actor->parent != nullptr) {
                randoCheckId = IdentifyEtceteraBasedOnParent(actor);
            } else {
                randoCheckId = IdentifyEtceteraBasedOnId(actor);
            }
            if (((randoCheckId != RC_UNKNOWN)) && (!RANDO_SAVE_CHECKS[randoCheckId].cycleObtained)) {
                RANDO_SAVE_CHECKS[randoCheckId].eligible = true;
                actor->draw = ObjEtcetera_DrawAnimated;
            }
        }
    });

    COND_ID_HOOK(OnActorInit, ACTOR_OBJ_ETCETERA, SHUFFLED_DEKU_FLOWERS, [](Actor* actor) {
        RandoCheckId randoCheckId = RC_UNKNOWN;
        if (actor->params == DEKU_FLOWER_PARAMS(DEKU_FLOWER_TYPE_PINK_WITH_INITIAL_BOUNCE)) {
            randoCheckId = IdentifyEtceteraBasedOnPos(actor);
        } else if (actor->parent != nullptr) {
            randoCheckId = IdentifyEtceteraBasedOnParent(actor);
        } else {
            randoCheckId = IdentifyEtceteraBasedOnId(actor);
        }
        if (!RANDO_SAVE_CHECKS[randoCheckId].cycleObtained && (randoCheckId != RC_UNKNOWN)) {
            // Init as idle, actor setup will take care of changing this function to animated if its a pink bouncing
            // flower
            actor->draw = Draw_ObjEtcetera_Idle_Rando;
        };
    });
}