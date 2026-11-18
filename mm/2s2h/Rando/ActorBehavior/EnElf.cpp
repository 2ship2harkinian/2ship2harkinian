#include "EnElf.h"
#include "Rando/Rando.h"
#include "ActorBehavior.h"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"
}

bool TryGiveFairyCheck(EnElf* enElf) {
    RandoCheckId randoCheckId = Rando::ActorBehavior::GetObjectRandoCheckId(enElf);
    if (randoCheckId != RC_UNKNOWN) {
        RandoSaveCheck* randoSaveCheck = &RANDO_SAVE_CHECKS[randoCheckId];
        if (randoSaveCheck->shuffled && !randoSaveCheck->cycleObtained) {
            return randoSaveCheck->eligible = true;
        }
    }
    return false;
}

void EnElf_DrawCustom(Actor* thisx, PlayState* play) {
    EnElf* enElf = (EnElf*)thisx;
    Matrix_Scale(40.0f, 40.0f, 40.0f, MTXMODE_APPLY);

    RandoCheckId randoCheckId = Rando::ActorBehavior::GetObjectRandoCheckId(thisx);

    if (randoCheckId == RC_UNKNOWN) {
        return;
    }

    RandoSaveCheck randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];

    Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, randoCheckId), randoCheckId, thisx);
}

void SpawnRandoFairy(f32 posX, f32 posY, f32 posZ, RandoCheckId randoCheckId, s32 fairyType) {
    Actor* actor = Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_ELF, posX, posY, posZ, 0, 0, 0,
                               FAIRY_PARAMS(fairyType, false, 0));
    if (randoCheckId != RC_UNKNOWN && RANDO_SAVE_CHECKS[randoCheckId].shuffled &&
        !RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
        Rando::ActorBehavior::SetObjectRandoCheckId(actor, randoCheckId);
        actor->draw = EnElf_DrawCustom;
    }
}

void Rando::ActorBehavior::InitEnElfBehavior() {
    bool shouldRegister = IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_MISC_FAIRIES];

    COND_VB_SHOULD(VB_COLLECT_ELF, shouldRegister, {
        if (TryGiveFairyCheck(va_arg(args, EnElf*))) {
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_SPAWN_FOUNTAIN_ELF, shouldRegister, {
        *should = false;
        RandoCheckId randoCheckId = RC_BENEATH_THE_WELL_FOUNTAIN_FAIRY_1;
        Actor* actor = va_arg(args, Actor*);
        Vec3f pos = actor->world.pos;
        s32 i;
        for (i = 0; i < 8; i++) {
            SpawnRandoFairy(pos.x, pos.y - 30.0f, pos.z, (RandoCheckId)(randoCheckId + i), FAIRY_TYPE_6);
        }
    });
}