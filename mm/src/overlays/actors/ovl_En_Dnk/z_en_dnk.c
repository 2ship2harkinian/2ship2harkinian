/*
 * File: z_en_dnk.c
 * Overlay: ovl_En_Dnk
 * Description: Hallucinatory Mad Scrubs (Deku curse and healing cutscenes)
 *   This actor has two alternate objects that would use alternate models, unfinished
 *   Variant 0x2 (encircling link) and 0x6 (big, chases link, cutscene) both use object_dekunuts,
 *     instead of object_dnk and object_hintnuts
 */

#include "z_en_dnk.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnDnk_Init(Actor* thisx, PlayState* play);
void EnDnk_Destroy(Actor* thisx, PlayState* play);
void EnDnk_Update(Actor* thisx, PlayState* play);
void EnDnk_Reset(void);

void EnDnk_HandleCutscene(EnDnk* this, PlayState* play);
void EnDnk_DoNothing(EnDnk* this, PlayState* play);
void EnDnk_Draw(Actor* thisx, PlayState* play);
void EnDnk_UpdateCutscene(EnDnk* this, PlayState* play);

static s16 sScrubCount = 0;

ActorProfile En_Dnk_Profile = {
    /**/ ACTOR_EN_DNK,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnDnk),
    /**/ EnDnk_Init,
    /**/ EnDnk_Destroy,
    /**/ EnDnk_Update,
    /**/ NULL,
    /**/ EnDnk_Reset,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 18, 46, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 1, 0, 0, 0, MASS_IMMOVABLE };

// unfinished
// if spawned out of cutscene, you can indeed attack them and it results in blood but no interaction
static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(1, 0x0),
    /* Deku Stick     */ DMG_ENTRY(1, 0x0),
    /* Horse trample  */ DMG_ENTRY(1, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0x0),
    /* Zora boomerang */ DMG_ENTRY(1, 0x0),
    /* Normal arrow   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(1, 0x0),
    /* Hookshot       */ DMG_ENTRY(1, 0x0),
    /* Goron punch    */ DMG_ENTRY(1, 0x0),
    /* Sword          */ DMG_ENTRY(1, 0x0),
    /* Goron pound    */ DMG_ENTRY(1, 0x0),
    /* Fire arrow     */ DMG_ENTRY(1, 0x0),
    /* Ice arrow      */ DMG_ENTRY(1, 0x0),
    /* Light arrow    */ DMG_ENTRY(1, 0x0),
    /* Goron spikes   */ DMG_ENTRY(1, 0x0),
    /* Deku spin      */ DMG_ENTRY(1, 0x0),
    /* Deku bubble    */ DMG_ENTRY(1, 0x0),
    /* Deku launch    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(1, 0x0),
    /* Zora barrier   */ DMG_ENTRY(1, 0x0),
    /* Normal shield  */ DMG_ENTRY(1, 0x0),
    /* Light ray      */ DMG_ENTRY(1, 0x0),
    /* Thrown object  */ DMG_ENTRY(1, 0x0),
    /* Zora punch     */ DMG_ENTRY(1, 0x0),
    /* Spin attack    */ DMG_ENTRY(1, 0x0),
    /* Sword beam     */ DMG_ENTRY(1, 0x0),
    /* Normal Roll    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(1, 0x0),
    /* Unblockable    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(1, 0x0),
    /* Powder Keg     */ DMG_ENTRY(1, 0x0),
};

typedef enum {
    /* -1 */ ENDNK_ANIM_NONE = -1,
    /*  0 */ ENDNK_ANIM_0,
    /*  1 */ ENDNK_ANIM_1,
    /*  2 */ ENDNK_ANIM_2,
    /*  3 */ ENDNK_ANIM_3,
    /*  4 */ ENDNK_ANIM_4,
    /*  5 */ ENDNK_ANIM_5,
    /*  6 */ ENDNK_ANIM_6,
    /*  7 */ ENDNK_ANIM_7,
    /*  8 */ ENDNK_ANIM_8,
    /*  9 */ ENDNK_ANIM_9,
    /* 10 */ ENDNK_ANIM_10,
    /* 11 */ ENDNK_ANIM_11,
    /* 12 */ ENDNK_ANIM_12,
    /* 13 */ ENDNK_ANIM_13,
    /* 14 */ ENDNK_ANIM_14,
    /* 15 */ ENDNK_ANIM_15,
    /* 16 */ ENDNK_ANIM_16,
    /* 17 */ ENDNK_ANIM_17,
    /* 18 */ ENDNK_ANIM_18,
    /* 19 */ ENDNK_ANIM_19,
    /* 20 */ ENDNK_ANIM_20,
    /* 21 */ ENDNK_ANIM_21,
    /* 22 */ ENDNK_ANIM_22,
    /* 23 */ ENDNK_ANIM_23,
    /* 24 */ ENDNK_ANIM_24,
    /* 25 */ ENDNK_ANIM_25,
    /* 26 */ ENDNK_ANIM_26,
    /* 27 */ ENDNK_ANIM_27,
    /* 28 */ ENDNK_ANIM_28,
    /* 29 */ ENDNK_ANIM_29,
    /* 30 */ ENDNK_ANIM_30,
    /* 31 */ ENDNK_ANIM_31,
    /* 32 */ ENDNK_ANIM_32,
    /* 33 */ ENDNK_ANIM_33,
    /* 34 */ ENDNK_ANIM_34,
    /* 35 */ ENDNK_ANIM_IDLE,
    /* 36 */ ENDNK_ANIM_MAX
} EnDnkAnimation;

// almost none of these animations get used
static AnimationInfoS sAnimationInfo[ENDNK_ANIM_MAX] = {
    { &gDekuPalaceGuardStartAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },    // ENDNK_ANIM_0
    { &gDekuPalaceGuardStartAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -4 },   // ENDNK_ANIM_1
    { &gDekuPalaceGuardWaitAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },    // ENDNK_ANIM_2
    { &gDekuPalaceGuardAttackAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },  // ENDNK_ANIM_3
    { &gDekuPalaceGuardDigAnim, 1.0f, -1, -1, ANIMMODE_ONCE, 0 },     // ENDNK_ANIM_4
    { &gDekuPalaceGuardDigAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -4 },     // ENDNK_ANIM_5
    { &gDekuPalaceGuardJumpAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -4 },    // ENDNK_ANIM_6
    { &gDekuPalaceGuardWalkAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },     // ENDNK_ANIM_7
    { &gDekuPalaceGuardWalkAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },    // ENDNK_ANIM_8
    { &gDekuPalaceGuardHappyAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },   // ENDNK_ANIM_9
    { &object_hintnuts_Anim_000168, 1.0f, 0, -1, ANIMMODE_ONCE, -4 }, // ENDNK_ANIM_10
    { &object_hintnuts_Anim_0024CC, 1.0f, -1, -1, ANIMMODE_ONCE, 0 }, // ENDNK_ANIM_11
    { &object_hintnuts_Anim_0024CC, 1.0f, 0, -1, ANIMMODE_ONCE, -4 }, // ENDNK_ANIM_12
    { &object_hintnuts_Anim_0026C4, 1.0f, 0, -1, ANIMMODE_ONCE, -4 }, // ENDNK_ANIM_13
    { &object_hintnuts_Anim_002894, 1.0f, 0, -1, ANIMMODE_ONCE, -4 }, // ENDNK_ANIM_14
    { &object_hintnuts_Anim_002B90, 1.0f, 0, -1, ANIMMODE_ONCE, -4 }, // ENDNK_ANIM_15
    { &object_hintnuts_Anim_002F7C, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },  // ENDNK_ANIM_16
    { &object_hintnuts_Anim_002F7C, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENDNK_ANIM_17
    { &object_hintnuts_Anim_003128, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },  // ENDNK_ANIM_18
    { &object_hintnuts_Anim_003128, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENDNK_ANIM_19
    { &object_hintnuts_Anim_0029BC, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENDNK_ANIM_20
    { &object_hintnuts_Anim_002E84, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENDNK_ANIM_21
    { &gDekuScrubSpitAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },          // ENDNK_ANIM_22
    { &gDekuScrubDamageAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -4 },        // ENDNK_ANIM_23
    { &gDekuScrubBurrowAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },         // ENDNK_ANIM_24
    { &gDekuScrubBurrowAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -4 },        // ENDNK_ANIM_25
    { &gDekuScrubDieAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -4 },           // ENDNK_ANIM_26
    { &gDekuScrubUnburrowAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -4 },      // ENDNK_ANIM_27
    { &gDekuScrubLookAroundAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },    // ENDNK_ANIM_28
    { &gDekuScrubUpAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -4 },            // ENDNK_ANIM_29
    { &gDekuScrubIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },           // ENDNK_ANIM_30
    { &gDekuScrubIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },          // ENDNK_ANIM_31
    { &gDekuScrubPantingAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },       // ENDNK_ANIM_32
    { &gDekuScrubRunAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },            // ENDNK_ANIM_33
    { &gDekuScrubRunAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },           // ENDNK_ANIM_34
    { &gDekuScrubStandingIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },   // ENDNK_ANIM_IDLE
};

s32 EnDnk_ChangeAnim(SkelAnime* skelAnime, s16 animIndex) {
    s16 startFrame;
    s16 endFrame;
    s32 didAnimChange = false;

    if (animIndex > ENDNK_ANIM_NONE) {
        if (animIndex < ENDNK_ANIM_MAX) {
            didAnimChange = true;
            endFrame = sAnimationInfo[animIndex].frameCount;
            if (endFrame < 0) {
                endFrame = Animation_GetLastFrame(sAnimationInfo[animIndex].animation);
            }
            startFrame = sAnimationInfo[animIndex].startFrame;
            if (startFrame < 0) {
                startFrame = endFrame;
            }
            Animation_Change(skelAnime, sAnimationInfo[animIndex].animation, sAnimationInfo[animIndex].playSpeed,
                             startFrame, endFrame, sAnimationInfo[animIndex].mode,
                             sAnimationInfo[animIndex].morphFrames);
        }
    }
    return didAnimChange;
}

s32 EnDnk_Blink(EnDnk* this) {
    s32 ret = false;

    if (DECR(this->blinkTimer) == 0) {
        this->eyeTexIndex++;
        if (this->eyeTexIndex >= 3) {
            this->blinkTimer = Rand_S16Offset(20, 20);
            this->eyeTexIndex = 0;
        }
        ret = true;
    }
    return ret;
}

void EnDnk_WaitForObject(EnDnk* this, PlayState* play) {
    if (SubS_IsObjectLoaded(this->objectSlot, play) == true) {
        gSegments[0x06] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[this->objectSlot].segment);
        this->actor.draw = EnDnk_Draw;
        this->actor.objectSlot = this->objectSlot;
        ActorShape_Init(&this->actor.shape, 0.0f, NULL, 18.0f);

        switch (ENDNK_GET_TYPE(&this->actor)) {
            case ENDNK_TYPE_GUARD: // unused
                SkelAnime_Init(play, &this->skelAnime, &gDekuPalaceGuardSkel, NULL, this->jointTable, this->morphTable,
                               DEKU_PALACE_GUARD_LIMB_MAX);
                EnDnk_ChangeAnim(&this->skelAnime, ENDNK_ANIM_7);
                break;

            case ENDNK_TYPE_HINT: // unused
                SkelAnime_Init(play, &this->skelAnime, &object_hintnuts_Skel_0023B8, NULL, this->jointTable,
                               this->morphTable, OBJECT_HINTNUTS_LIMB_MAX);
                EnDnk_ChangeAnim(&this->skelAnime, ENDNK_ANIM_18);
                break;

            case ENDNK_TYPE_ATTACK:
                SkelAnime_Init(play, &this->skelAnime, &gDekuScrubSkel, NULL, this->jointTable, this->morphTable,
                               DEKU_SCRUB_LIMB_MAX);
                EnDnk_ChangeAnim(&this->skelAnime, ENDNK_ANIM_IDLE);
                break;
        }

        Collider_InitCylinder(play, &this->collider);
        Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
        CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
        if (ENDNK_GET_CUTSCENE_BEHAVIOR(&this->actor) == ENDNK_CUTSCENE_CONTROL) {
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            this->actor.flags |= (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED);
            this->actionFunc = EnDnk_HandleCutscene;
            Actor_SetScale(&this->actor, 0.1f);
        } else {
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            this->actionFunc = EnDnk_DoNothing;
            Actor_SetScale(&this->actor, 0.01f);
        }
    }
}

void EnDnk_HandleCutscene(EnDnk* this, PlayState* play) {
    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_126)) {
        Cutscene_ActorTranslateAndYaw(&this->actor, play, Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_126));
    }
}

// Regular scrubs in the cutscene have no job but to animate on the spot
void EnDnk_DoNothing(EnDnk* this, PlayState* play) {
}

void EnDnk_Init(Actor* thisx, PlayState* play) {
    EnDnk* this = (EnDnk*)thisx;

    this->objectSlot = OBJECT_SLOT_NONE;

    switch (ENDNK_GET_TYPE(&this->actor)) {
        case ENDNK_TYPE_HINT:
            this->objectSlot = SubS_GetObjectSlot(OBJECT_HINTNUTS, play);
            break;

        case ENDNK_TYPE_GUARD:
            this->objectSlot = SubS_GetObjectSlot(OBJECT_DNK, play);
            break;

        case ENDNK_TYPE_ATTACK:
            this->objectSlot = SubS_GetObjectSlot(OBJECT_DEKUNUTS, play);
            break;
    }

    if (this->objectSlot > OBJECT_SLOT_NONE) {
        this->actionFunc = EnDnk_WaitForObject;
    } else {
        Actor_Kill(&this->actor);
    }

    this->scrubId = sScrubCount;
    sScrubCount++;
}

void EnDnk_Destroy(Actor* thisx, PlayState* play) {
    EnDnk* this = (EnDnk*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnDnk_Update(Actor* thisx, PlayState* play) {
    EnDnk* this = (EnDnk*)thisx;
    s32 pad;

    this->actionFunc(this, play);
    SkelAnime_Update(&this->skelAnime);
    EnDnk_Blink(this);
    Actor_SetFocus(&this->actor, 34.0f);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    EnDnk_UpdateCutscene(this, play);
}

// Guard functions are unused

s32 EnDnk_Guard_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnDnk* this = (EnDnk*)thisx;

    this->limbGfx[limbIndex] = *dList;
    *dList = NULL;
    return false;
}

void EnDnk_Guard_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnDnk* this = (EnDnk*)thisx;
    MtxF mtxFCopy;
    Vec3f zeroVec = gZeroVec3f;
    Vec3f headPos;
    Vec3s headRot;

    if (limbIndex == DEKU_PALACE_GUARD_LIMB_HEAD) {
        Matrix_MultVec3f(&zeroVec, &headPos);
        Matrix_Get(&mtxFCopy);
        Matrix_MtxFToYXZRot(&mtxFCopy, &headRot, false);
        Matrix_Translate(headPos.x, headPos.y, headPos.z, MTXMODE_NEW);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);

        if (this->flags & DNK_FLAG_10) {
            if (this->flags & DNK_FLAG_20) {
                headRot.x = this->unk_296;
                headRot.y = this->unk_298;
                headRot.y += this->actor.shape.rot.y;
                headRot.z = 0;
                Math_SmoothStepToS(&this->currentHeadRot.x, headRot.x, 4, 0x1FFE, 1);
                Math_SmoothStepToS(&this->currentHeadRot.y, headRot.y, 4, 0x1FFE, 1);
                Math_SmoothStepToS(&this->currentHeadRot.z, headRot.z, 4, 0x1FFE, 1);
            } else {
                this->currentHeadRot.x = headRot.x;
                this->currentHeadRot.y = headRot.y;
                this->currentHeadRot.z = headRot.z;
            }
        } else {
            this->flags |= DNK_FLAG_10;
            this->currentHeadRot.x = headRot.x;
            this->currentHeadRot.y = headRot.y;
            this->currentHeadRot.z = headRot.z;
        }

        Matrix_RotateYS(this->currentHeadRot.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->currentHeadRot.x, MTXMODE_APPLY);
        Matrix_RotateZS(this->currentHeadRot.z, MTXMODE_APPLY);
    }

    OPEN_DISPS(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, this->limbGfx[limbIndex]);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnDnk_Guard_Draw(EnDnk* this, PlayState* play) {
    static TexturePtr sDnkEyes[] = {
        gDekuPalaceGuardEyeOpenTex,
        gDekuPalaceGuardEyeHalfTex,
        gDekuPalaceGuardEyeClosedTex,
    };
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sDnkEyes[this->eyeTexIndex]));
    gDPPipeSync(POLY_OPA_DISP++);

    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnDnk_Guard_OverrideLimbDraw,
                      EnDnk_Guard_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 EnDnk_Nuts_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnDnk* this = (EnDnk*)thisx;

    this->limbGfx[limbIndex] = *dList;
    *dList = NULL;
    return false;
}

void EnDnk_Nuts_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnDnk* this = (EnDnk*)thisx;
    MtxF mtxFCopy;
    Vec3f zeroVec = gZeroVec3f;
    Vec3f headPos;
    Vec3s headRot;

    // Note: Also for `limbIndex == OBJECT_HINTNUTS_LIMB_02`
    // Assumes `OBJECT_HINTNUTS_LIMB_02` is the same value as `DEKU_SCRUB_LIMB_HEAD`
    // this is not head tracking, this is hair shaking with the animation to make his movement look more dynamic
    if (limbIndex == DEKU_SCRUB_LIMB_HEAD) {
        Matrix_MultVec3f(&zeroVec, &headPos);
        Matrix_Get(&mtxFCopy);
        Matrix_MtxFToYXZRot(&mtxFCopy, &headRot, false);
        Matrix_Translate(headPos.x, headPos.y, headPos.z, MTXMODE_NEW);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);

        if (this->flags & DNK_FLAG_10) {
            if (this->flags & DNK_FLAG_20) {
                headRot.z = this->unk_296 + 0x4000;
                headRot.y = this->unk_298 + 0x4000;
                headRot.y += this->actor.shape.rot.y;
                headRot.x = 0;
                Math_SmoothStepToS(&this->currentHeadRot.x, headRot.x, 4, 0x1FFE, 1);
                Math_SmoothStepToS(&this->currentHeadRot.y, headRot.y, 4, 0x1FFE, 1);
                Math_SmoothStepToS(&this->currentHeadRot.z, headRot.z, 4, 0x1FFE, 1);
            } else {
                this->currentHeadRot.x = headRot.x;
                this->currentHeadRot.y = headRot.y;
                this->currentHeadRot.z = headRot.z;
            }
        } else {
            this->flags |= DNK_FLAG_10;
            this->currentHeadRot.x = headRot.x;
            this->currentHeadRot.y = headRot.y;
            this->currentHeadRot.z = headRot.z;
        }

        Matrix_RotateYS(this->currentHeadRot.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->currentHeadRot.x, MTXMODE_APPLY);
        Matrix_RotateZS(this->currentHeadRot.z, MTXMODE_APPLY);
    }

    OPEN_DISPS(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, this->limbGfx[limbIndex]);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnDnk_Nuts_Draw(EnDnk* this, PlayState* play) {
    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnDnk_Nuts_OverrideLimbDraw,
                      EnDnk_Nuts_PostLimbDraw, &this->actor);
}

void EnDnk_Draw(Actor* thisx, PlayState* play) {
    EnDnk* this = (EnDnk*)thisx;

    switch (ENDNK_GET_TYPE(thisx)) {
        // palace guard type skeleton
        case ENDNK_TYPE_GUARD:
            EnDnk_Guard_Draw(this, play);
            break;

        // deku nuts type skeleton
        case ENDNK_TYPE_HINT:
        case ENDNK_TYPE_ATTACK:
            EnDnk_Nuts_Draw(this, play);
            break;

        default:
            break;
    }
}

void EnDnk_HandleCSAudio(EnDnk* this, PlayState* play) {
    switch (play->csCtx.curFrame) {
        case 4 * 20:
            Audio_PlaySfx_2(NA_SE_EN_DEKNUTS_DANCE1);
            break;

        case 6 * 20 + 3:
            Audio_PlaySfx_2(NA_SE_EN_DEKNUTS_DANCE2);
            break;

        case 21 * 20 + 18:
            Actor_PlaySfx(&this->actor, NA_SE_EN_DEKNUTS_DANCE_BIG);
            break;

        case 24 * 20 + 13:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_APPEAR);
            break;

        default:
            break;
    }

    // (200 - 2) and (440 - 2)? pre-delay the sfx?
    if ((play->csCtx.curFrame >= 198) && (play->csCtx.curFrame < 438)) {
        Audio_PlaySfx_2(NA_SE_EN_DEKNUTS_DANCE - SFX_FLAG);
    }
}

void EnDnk_UpdateCutscene(EnDnk* this, PlayState* play) {
    // only play the sounds during the curse cutscene, not the healing cutscene
    if ((play->csCtx.state != CS_STATE_IDLE) && (ENDNK_GET_CUTSCENE_BEHAVIOR(&this->actor) == ENDNK_CUTSCENE_CONTROL) &&
        (play->sceneId == SCENE_SPOT00) && (gSaveContext.sceneLayer == 2)) {
        EnDnk_HandleCSAudio(this, play);
    }
}

void EnDnk_Reset(void) {
    sScrubCount = 0;
}
