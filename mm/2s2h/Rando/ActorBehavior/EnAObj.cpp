#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/ObjectExtension/ActorListIndex.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

extern "C" {
void EnAObj_Draw(Actor* thisx, PlayState* play);
}

// clang-format off
std::map<std::tuple<s16, s16, s16, s16>, RandoCheckId> enAObjMap = {
    // South Clock Town
    { { ACTOR_EN_A_OBJ, SCENE_CLOCKTOWER, 0, 45  }, RC_CLOCK_TOWN_SOUTH_ARROW_SIGN },
    // Termina Field
    { { ACTOR_EN_A_OBJ, SCENE_00KEIKOKU,  0, 211 }, RC_TERMINA_FIELD_RANCH_ARROW_SIGN },
    // Southern Swamp
    { { ACTOR_EN_A_OBJ, SCENE_20SICHITAI, 0, 12  }, RC_SOUTHERN_SWAMP_HAGS_ARROW_SIGN },
    { { ACTOR_EN_A_OBJ, SCENE_20SICHITAI, 1, 12  }, RC_SOUTHERN_SWAMP_PALACE_ARROW_SIGN },
    // Deku Palace
    { { ACTOR_EN_A_OBJ, SCENE_22DEKUCITY, 0, 22  }, RC_DEKU_PALACE_ENTRY_ARROW_SIGN },
    { { ACTOR_EN_A_OBJ, SCENE_22DEKUCITY, 0, 24  }, RC_DEKU_PALACE_LILY_ARROW_SIGN_01 },
    { { ACTOR_EN_A_OBJ, SCENE_22DEKUCITY, 0, 23  }, RC_DEKU_PALACE_LILY_ARROW_SIGN_02 },
    { { ACTOR_EN_A_OBJ, SCENE_22DEKUCITY, 0, 21  }, RC_DEKU_PALACE_SHORTCUT_ARROW_SIGN },
    // Milk Road
    { { ACTOR_EN_A_OBJ, SCENE_ROMANYMAE,  0, 21  }, RC_MILK_ROAD_TRACK_ARROW_SIGN },
    { { ACTOR_EN_A_OBJ, SCENE_ROMANYMAE,  0, 22  }, RC_MILK_ROAD_RANCH_ARROW_SIGN },
    // Road to Ikana
    { { ACTOR_EN_A_OBJ, SCENE_IKANAMAE,   0, 77  }, RC_ROAD_TO_IKANA_GRAVEYARD_ARROW_SIGN },
    // Great Bay Coast
    { { ACTOR_EN_A_OBJ, SCENE_30GYOSON,   0, 153 }, RC_GREAT_BAY_COAST_UNDERWATER_SIGN },
    // Zora Cape
    { { ACTOR_EN_A_OBJ, SCENE_31MISAKI,   0, 76  }, RC_ZORA_CAPE_UNDERWATER_SIGN },
    // Pinnacle Rock
    { { ACTOR_EN_A_OBJ, SCENE_SINKAI,     0, 14  }, RC_PINNACLE_ROCK_SIGN },
    { { ACTOR_EN_A_OBJ, SCENE_SINKAI,     0, 35  }, RC_PINNACLE_ROCK_ARROW_SIGN_01 },
    { { ACTOR_EN_A_OBJ, SCENE_SINKAI,     0, 36  }, RC_PINNACLE_ROCK_ARROW_SIGN_02 },
    { { ACTOR_EN_A_OBJ, SCENE_SINKAI,     0, 37  }, RC_PINNACLE_ROCK_ARROW_SIGN_03 },
    { { ACTOR_EN_A_OBJ, SCENE_SINKAI,     0, 38  }, RC_PINNACLE_ROCK_ARROW_SIGN_04 },
    { { ACTOR_EN_A_OBJ, SCENE_SINKAI,     0, 39  }, RC_PINNACLE_ROCK_ARROW_SIGN_05 },
    { { ACTOR_EN_A_OBJ, SCENE_SINKAI,     0, 40  }, RC_PINNACLE_ROCK_ARROW_SIGN_06 },
    { { ACTOR_EN_A_OBJ, SCENE_SINKAI,     0, 42  }, RC_PINNACLE_ROCK_ARROW_SIGN_07 },

    // Duplicates in world state (poision/clear)
    // Southern Swamp
    { { ACTOR_EN_A_OBJ, SCENE_20SICHITAI2, 0, 10 }, RC_SOUTHERN_SWAMP_HAGS_ARROW_SIGN },
    { { ACTOR_EN_A_OBJ, SCENE_20SICHITAI2, 1, 4  }, RC_SOUTHERN_SWAMP_PALACE_ARROW_SIGN },
};
// clang-format on

void DrawEnAObj(Actor* actor, PlayState* play) {
    EnAObj_Draw(actor, play);

    OPEN_DISPS(gPlayState->state.gfxCtx);
    RandoCheckId randoCheckId = Rando::ActorBehavior::GetObjectRandoCheckId(actor);
    f32 yOffset = actor->params == 0 ? 70.0f : 62.0f;
    Matrix_Push();
    Matrix_Translate(actor->world.pos.x, actor->world.pos.y + yOffset, actor->world.pos.z, MTXMODE_NEW);
    Matrix_RotateYS(actor->shape.rot.y, MTXMODE_APPLY);
    Matrix_Scale(0.3f, 0.3f, 0.3f, MTXMODE_APPLY);
    Rando::DrawItem(Rando::ConvertItem(RANDO_SAVE_CHECKS[randoCheckId].randoItemId, randoCheckId), randoCheckId, actor);
    Matrix_Pop();
    CLOSE_DISPS(gPlayState->state.gfxCtx);
}

void EnAObjInit(Actor* actor) {
    RandoCheckId randoCheckId = RC_UNKNOWN;
    auto it =
        enAObjMap.find({ actor->id, gPlayState->sceneId, gPlayState->roomCtx.curRoom.num, GetActorListIndex(actor) });
    if (it != enAObjMap.end()) {
        randoCheckId = it->second;
    }

    if (!RANDO_SAVE_CHECKS[randoCheckId].shuffled || RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
        return;
    }

    Rando::ActorBehavior::SetObjectRandoCheckId(actor, randoCheckId);
}

void Rando::ActorBehavior::InitEnAObjBehavior() {
    bool shouldRegister = IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_SIGNPOSTS];

    COND_ID_HOOK(OnActorInit, ACTOR_EN_A_OBJ, shouldRegister, EnAObjInit);
    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_A_OBJ, shouldRegister, [](Actor* actor) {
        RandoCheckId randoCheckId = Rando::ActorBehavior::GetObjectRandoCheckId(actor);
        if (!RANDO_SAVE_CHECKS[randoCheckId].shuffled || RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
            actor->draw = EnAObj_Draw;
            return;
        }

        actor->draw = DrawEnAObj;
    });

    COND_VB_SHOULD(VB_PLAYER_START_TALKING, shouldRegister, {
        Actor* talkActor = GET_PLAYER(gPlayState)->talkActor;
        if (talkActor->id != ACTOR_EN_A_OBJ) {
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
