#include "ActorBehavior.h"
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/ObjectExtension/ActorListIndex.h"

extern "C" {
#include "src/overlays/actors/ovl_Obj_Swprize/z_obj_swprize.h"
}

// clang-format off
std::map<std::tuple<s16, s16, s16>, RandoCheckId> softSoilMap = {
    { { SCENE_00KEIKOKU, 0, 5 }, RC_TERMINA_FIELD_STUMP_SOFT_SOIL },
    { { SCENE_00KEIKOKU, 0, 6 }, RC_ASTRAL_OBSERVATORY_SOFT_SOIL_1 },
    { { SCENE_00KEIKOKU, 0, 7 }, RC_TERMINA_FIELD_SOUTH_WALL_SOFT_SOIL_1 },
    { { SCENE_00KEIKOKU, 0, 8 }, RC_TERMINA_FIELD_PILLAR_SOFT_SOIL },
    { { SCENE_22DEKUCITY, 0, 0 }, RC_DEKU_PALACE_SOFT_SOIL_1 },
    { { SCENE_30GYOSON, 0, 0 }, RC_GREAT_BAY_COAST_LEDGE_SOFT_SOIL },
    { { SCENE_F01_B, 0, 8 }, RC_DOGGY_RACETRACK_SOFT_SOIL },
    { { SCENE_F01, 0, 48 }, RC_ROMANI_RANCH_DAY_2_OR_3_SOFT_SOIL_1 },
    { { SCENE_F01, 0, 49 }, RC_ROMANI_RANCH_DAY_1_SOFT_SOIL },
    { { SCENE_F41, 0, 0 }, RC_STONE_TOWER_INVERTED_LOWER_SOFT_SOIL },
    { { SCENE_F41, 0, 1 }, RC_STONE_TOWER_INVERTED_UPPER_SOFT_SOIL },
    { { SCENE_KAKUSIANA, 12, 9 }, RC_DEKU_PALACE_BEAN_SALESMAN_GROTTO_SOFT_SOIL_1 },
    { { SCENE_KINSTA1, 2, 13 }, RC_SWAMP_SPIDER_HOUSE_GOLD_ROOM_UPPER_SOFT_SOIL },
    { { SCENE_KINSTA1, 3, 14 }, RC_SWAMP_SPIDER_HOUSE_MONUMENT_ROOM_SOFT_SOIL_1 },
    { { SCENE_RANDOM, 0, 19 }, RC_SECRET_SHRINE_SOFT_SOIL_1 },
};
// clang-format on

Actor* SpawnSoilDrop(Vec3f pos, RandoCheckId randoCheckId) {
    EnItem00* collectible = CustomItem::Spawn(
        pos.x, pos.y, pos.z, 0, CustomItem::KILL_ON_TOUCH | CustomItem::TOSS_ON_SPAWN | CustomItem::ABLE_TO_ZORA_RANG,
        randoCheckId,
        [](Actor* actor, PlayState* play) {
            RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
            randoSaveCheck.eligible = true;
        },
        [](Actor* actor, PlayState* play) {
            RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
            Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
            Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, (RandoCheckId)CUSTOM_ITEM_PARAM),
                            (RandoCheckId)CUSTOM_ITEM_PARAM, actor);
        });
    return (Actor*)collectible;
}

void Rando::ActorBehavior::InitObjSwprizeBehavior() {
    bool shouldRegister = IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_SOFT_SOIL];
    COND_VB_SHOULD(VB_OBJ_SWPRIZE_DROP_COLLECTIBLE, shouldRegister, {
        RandoCheckId randoCheckId = RC_UNKNOWN;
        ObjSwprize* objSwprize = va_arg(args, ObjSwprize*);
        // Used to offset check id's when a soil patch produces a trio of items.
        s32 i = va_arg(args, s32);
        Actor** collectible = va_arg(args, Actor**);
        auto it = softSoilMap.find(
            { gPlayState->sceneId, gPlayState->roomCtx.curRoom.num, GetActorListIndex((Actor*)objSwprize) });
        if (it != softSoilMap.end()) {
            randoCheckId = (RandoCheckId)(it->second + i);
        }
        if (randoCheckId != RC_UNKNOWN && RANDO_SAVE_CHECKS[randoCheckId].shuffled &&
            !RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
            *should = false;
            // Pass back the Actor pointer and let z_obj_swprize do the heavy lifting with angles
            *collectible = SpawnSoilDrop(objSwprize->actor.world.pos, randoCheckId);
        }
        if (randoCheckId == RC_ROMANI_RANCH_DAY_1_SOFT_SOIL) {
            // Allows getting both Romani Ranch day 1 and day 2/3 soil checks in one cycle.
            // This does enable the player to get unlimited arrows if they want to repeatedly reload
            // the area and feed the patch bugs on day 1, but that doesn't seem like a big deal.
            Flags_UnsetSwitch(gPlayState, 20);
        }
    });
}