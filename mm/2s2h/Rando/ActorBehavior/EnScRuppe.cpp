#include "ActorBehavior.h"
#include "2s2h/CustomItem/CustomItem.h"

extern "C" {
#include "functions.h"
}

// clang-format off
std::map<std::tuple<s16, s16>, RandoCheckId> enScRuppeMap = {
    { { SCENE_00KEIKOKU, 0x0922 }, RC_TERMINA_FIELD_TELESCOPE_NORTH_GUAY },
    { { SCENE_00KEIKOKU, 0x05A2 }, RC_TERMINA_FIELD_TELESCOPE_SOUTH_GUAY },
};
// clang-format on

void SpawnRandoHugeRupee(Vec3f pos, RandoCheckId randoCheckId) {
    EnItem00* enItem00 = CustomItem::Spawn(
        pos.x, pos.y, pos.z, 0, CustomItem::KILL_ON_TOUCH | CustomItem::TOSS_ON_SPAWN | CustomItem::ABLE_TO_ZORA_RANG,
        randoCheckId,
        [](Actor* actor, PlayState* play) {
            RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
            // This does have the unfortunate side effect of despawning the other telescope guay drop if the player
            // leaves before picking up both, but it can just be respawned through the telescope again.
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_54_01);
            randoSaveCheck.eligible = true;
        },
        [](Actor* actor, PlayState* play) {
            auto& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
            Matrix_Scale(40.0f, 40.0f, 40.0f, MTXMODE_APPLY);
            Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, (RandoCheckId)CUSTOM_ITEM_PARAM),
                            (RandoCheckId)CUSTOM_ITEM_PARAM, actor);
        });
    // Make the drops visible from the telescope.
    enItem00->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED;
}

void Rando::ActorBehavior::InitEnScRuppeBehavior() {
    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_SC_RUPPE, IS_RANDO, [](Actor* actor, bool* should) {
        RandoCheckId randoCheckId = RC_UNKNOWN;
        auto it = enScRuppeMap.find({ gPlayState->sceneId, actor->params });
        if (it != enScRuppeMap.end()) {
            randoCheckId = it->second;
        }

        if (randoCheckId != RC_UNKNOWN && RANDO_SAVE_CHECKS[randoCheckId].shuffled &&
            !RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
            SpawnRandoHugeRupee(actor->world.pos, randoCheckId);
            *should = false;
        }
    });
}