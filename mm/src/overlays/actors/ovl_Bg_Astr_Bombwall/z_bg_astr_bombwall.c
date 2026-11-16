/*
 * File: z_bg_astr_bombwall.c
 * Overlay: ovl_Bg_Astr_Bombwall
 * Description: Astral Observatory - Bombable Wall
 */

#include "z_bg_astr_bombwall.h"
#include "objects/object_astr_obj/object_astr_obj.h"

#define FLAGS 0x00000000

void BgAstrBombwall_Init(Actor* thisx, PlayState* play);
void BgAstrBombwall_Destroy(Actor* thisx, PlayState* play);
void BgAstrBombwall_Update(Actor* thisx, PlayState* play);
void BgAstrBombwall_Draw(Actor* thisx, PlayState* play);

void BgAstrBombwall_SetupStanding(BgAstrBombwall* this);
void BgAstrBombwall_Standing(BgAstrBombwall* this, PlayState* play);
void BgAstrBombwall_SetupExploding(BgAstrBombwall* this, PlayState* play);
void BgAstrBombwall_Exploding(BgAstrBombwall* this, PlayState* play);
void BgAstrBombwall_SetupCollapsing(BgAstrBombwall* this, PlayState* play);
void BgAstrBombwall_Collapsing(BgAstrBombwall* this, PlayState* play);

ActorProfile Bg_Astr_Bombwall_Profile = {
    /**/ ACTOR_BG_ASTR_BOMBWALL,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_ASTR_OBJ,
    /**/ sizeof(BgAstrBombwall),
    /**/ BgAstrBombwall_Init,
    /**/ BgAstrBombwall_Destroy,
    /**/ BgAstrBombwall_Update,
    /**/ BgAstrBombwall_Draw,
};

static ColliderTrisElementInit sTrisElementsInit[2] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000008, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { -70.0f, 0.0f, 3.0f }, { 70.0f, 0.0f, 3.0f }, { -70.0f, 200.0f, 3.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000008, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 70.0f, 0.0f, 3.0f }, { 70.0f, 200.0f, 3.0f }, { -70.0f, 200.0f, 3.0f } } },
    },
};

static ColliderTrisInit sTrisInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    ARRAY_COUNT(sTrisElementsInit),
    sTrisElementsInit,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

void BgAstrBombwall_InitCollider(ColliderTrisInit* init, Vec3f* pos, Vec3s* rot, ColliderTris* collider) {
    s32 i;
    s32 j;
    Vec3f sp54[3];

    Matrix_RotateYS(rot->y, MTXMODE_NEW);
    Matrix_RotateXS(rot->x, MTXMODE_APPLY);
    Matrix_RotateZS(rot->z, MTXMODE_APPLY);

    for (i = 0; i < init->count; i++) {
        for (j = 0; j < 3; j++) {
            Matrix_MultVec3f(init->elements[i].dim.vtx + j, &sp54[j]);
            Math_Vec3f_Sum(&sp54[j], pos, &sp54[j]);
        }
        Collider_SetTrisVertices(collider, i, &sp54[0], &sp54[1], &sp54[2]);
    }
}

void BgAstrBombwall_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgAstrBombwall* this = (BgAstrBombwall*)thisx;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
    DynaPolyActor_LoadMesh(play, &this->dyna, &gAstralObservatoryBombWallCol);
    Collider_InitTris(play, &this->collider);
    if (Flags_GetSwitch(play, BGASTRBOMBWALL_GET_SWITCH_FLAG(thisx))) {
        Actor_Kill(&this->dyna.actor);
        return;
    }
    this->dyna.actor.flags |= ACTOR_FLAG_UCODE_POINT_LIGHT_ENABLED;
    if (!Collider_SetTris(play, &this->collider, &this->dyna.actor, &sTrisInit, this->colliderElements)) {
        Actor_Kill(&this->dyna.actor);
        return;
    }
    BgAstrBombwall_InitCollider(&sTrisInit, &this->dyna.actor.world.pos, &this->dyna.actor.shape.rot, &this->collider);
    SubS_FillCutscenesList(&this->dyna.actor, this->csIdList, ARRAY_COUNT(this->csIdList));
    BgAstrBombwall_SetupStanding(this);
}

void BgAstrBombwall_Destroy(Actor* thisx, PlayState* play) {
    BgAstrBombwall* this = (BgAstrBombwall*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void BgAstrBombwall_SpawnFragments(BgAstrBombwall* this, PlayState* play) {
    s32 i;
    Vec3f vec;
    Vec3f pos;
    s16 spinRate;
    Vec3f velocity;
    f32 rand;

    Matrix_RotateYS(this->dyna.actor.shape.rot.y, MTXMODE_NEW);
    for (i = 0; i < 30; i++) {
        vec.x = Rand_Centered() * 140.0f;
        vec.y = Rand_ZeroOne() * 200.0f;
        vec.z = 0.0f;
        Matrix_MultVec3f(&vec, &pos);
        Math_Vec3f_Sum(&this->dyna.actor.world.pos, &pos, &pos);
        func_800BBFB0(play, &pos, 50.0f, 2, Rand_ZeroOne() * 120.0f + 20.0f, Rand_ZeroOne() * 240.0f + 20.0f, 0);
        velocity.x = Rand_ZeroOne() * 2.5f;
        velocity.y = (Rand_ZeroOne() * 2.5f) + 1.0f;
        velocity.z = Rand_ZeroOne() * 2.5f;
        rand = Rand_ZeroOne();

        if (rand < 0.2f) {
            spinRate = 0x60;
        } else if (rand < 0.6f) {
            spinRate = 0x40;
        } else {
            spinRate = 0x20;
        }
        EffectSsKakera_Spawn(play, &pos, &velocity, &pos, -260, spinRate, 20, 0, 0, 10, 0, 0, 50, -1, OBJECT_ASTR_OBJ,
                             gAstralObservatoryBombWallFragmentDL);
    }
}

void BgAstrBombwall_SetupStanding(BgAstrBombwall* this) {
    this->actionFunc = BgAstrBombwall_Standing;
}

void BgAstrBombwall_Standing(BgAstrBombwall* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        Flags_SetSwitch(play, BGASTRBOMBWALL_GET_SWITCH_FLAG(&this->dyna.actor));
        BgAstrBombwall_SetupExploding(this, play);
    } else {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void BgAstrBombwall_SetupExploding(BgAstrBombwall* this, PlayState* play) {
    this->actionFunc = BgAstrBombwall_Exploding;
}

void BgAstrBombwall_Exploding(BgAstrBombwall* this, PlayState* play) {
    if (SubS_StartCutscene(&this->dyna.actor, this->csIdList[0], CS_ID_NONE, SUBS_CUTSCENE_WITH_PLAYER)) {
        BgAstrBombwall_SetupCollapsing(this, play);
    }
}

void BgAstrBombwall_SetupCollapsing(BgAstrBombwall* this, PlayState* play) {
    DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
    this->dyna.actor.draw = NULL;
    BgAstrBombwall_SpawnFragments(this, play);
    Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_WALL_BROKEN);
    this->actionFunc = BgAstrBombwall_Collapsing;
}

void BgAstrBombwall_Collapsing(BgAstrBombwall* this, PlayState* play) {
}

void BgAstrBombwall_Update(Actor* thisx, PlayState* play) {
    BgAstrBombwall* this = (BgAstrBombwall*)thisx;

    this->actionFunc(this, play);
}

void BgAstrBombwall_Draw(Actor* thixs, PlayState* play) {
    {
        Gfx* opa;

        OPEN_DISPS(play->state.gfxCtx);

        opa = POLY_OPA_DISP;
        gSPDisplayList(&opa[0], gSetupDLs[SETUPDL_25]);
        MATRIX_FINALIZE_AND_LOAD(&opa[1], play->state.gfxCtx);
        gSPSetGeometryMode(&opa[2], G_LIGHTING_POSITIONAL);
        gSPDisplayList(&opa[3], gAstralObservatoryBombWallDL);
        POLY_OPA_DISP = &opa[4];

        CLOSE_DISPS(play->state.gfxCtx);
    }
    {
        Gfx* xlu;

        OPEN_DISPS(play->state.gfxCtx);

        xlu = POLY_XLU_DISP;
        gSPDisplayList(&xlu[0], gSetupDLs[SETUPDL_25]);
        MATRIX_FINALIZE_AND_LOAD(&xlu[1], play->state.gfxCtx);
        gSPSetGeometryMode(&xlu[2], G_LIGHTING_POSITIONAL);
        gSPDisplayList(&xlu[3], gAstralObservatoryBombWallCrackDL);
        POLY_XLU_DISP = &xlu[4];

        CLOSE_DISPS(play->state.gfxCtx);
    }
}
