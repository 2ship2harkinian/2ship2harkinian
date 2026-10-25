/*
 * File: z_en_maruta.c
 * Overlay: ovl_En_Maruta
 * Description: Swordsman's School - Practice Log
 */

#include "z_en_maruta.h"
#include "overlays/actors/ovl_En_Kendo_Js/z_en_kendo_js.h"
#include "objects/object_maruta/object_maruta.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnMaruta_Init(Actor* thisx, PlayState* play);
void EnMaruta_Destroy(Actor* thisx, PlayState* play);
void EnMaruta_Update(Actor* thisx, PlayState* play);
void EnMaruta_Draw(Actor* thisx, PlayState* play);

void EnMaruta_SetInPositionWhole(EnMaruta* this);
void EnMaruta_SittingWhole(EnMaruta* this, PlayState* play);
void EnMaruta_SetupRiseThroughFloor(EnMaruta* this);
void EnMaruta_RiseThroughFloor(EnMaruta* this, PlayState* play);
void EnMaruta_StartRetracting(EnMaruta* this);
void EnMaruta_RetractWhole(EnMaruta* this, PlayState* play);
void EnMaruta_RetractAfterCut(EnMaruta* this, PlayState* play);
void EnMaruta_SetupRecoilAfterCut(EnMaruta* this, PlayState* play);
void EnMaruta_Recoil(EnMaruta* this, PlayState* play);
void EnMaruta_FindRestingOrientation(EnMaruta* this, PlayState* play);
void EnMaruta_SetupWaitToFlatten(EnMaruta* this);
void EnMaruta_WaitToFlatten(EnMaruta* this, PlayState* play);
void EnMaruta_Flatten(EnMaruta* this, PlayState* play);
void EnMaruta_LerpSpinAngleToFlat(s16* curSpinAngle);
void EnMaruta_Bounce(EnMaruta* this, PlayState* play);
void EnMaruta_AdjustBounceAngle(EnMaruta* this, Vec3f* lowestPoint);
void EnMaruta_RotateVector(Vec3f* src, Vec3f* dest, s16 rotX, s16 rotY, s32 rotZ);
void EnMaruta_SpawnDustClouds(PlayState* play, Vec3f srcPoint);

ActorProfile En_Maruta_Profile = {
    /**/ ACTOR_EN_MARUTA,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_MARUTA,
    /**/ sizeof(EnMaruta),
    /**/ EnMaruta_Init,
    /**/ EnMaruta_Destroy,
    /**/ EnMaruta_Update,
    /**/ EnMaruta_Draw,
};

/**
 * These chunks of log are arranged like so (indices in array included):
 *
 * -------
 * |6 | 7|
 * |  |  |
 * |\ | /|
 * |4\|/5|
 * -------
 * |2/|\3|
 * |/ | \|
 * |  |  |
 * |0 | 1|
 * -------
 */
Gfx* sDisplayLists[] = {
    gPracticeLogBottomLeftBigChunkDL,     gPracticeLogBottomRightBigChunkDL, gPracticeLogBottomLeftCornerChunkDL,
    gPracticeLogBottomRightCornerChunkDL, gPracticeLogTopLeftCornerChunkDL,  gPracticeLogTopRightCornerChunkDL,
    gPracticeLogTopLeftBigChunkDL,        gPracticeLogTopRightBigChunkDL,
};

u8 sFragmentsByShape[] = {
    0xFF, 0x2B, 0xD4, 0x17, 0xE8, 0x55, 0xAA, 0x0F, 0xF0,
};

typedef enum EnMarutaShape {
    /* 0 */ ENMARUTA_SHAPE_WHOLE,
    /* 1 */ ENMARUTA_SHAPE_DIAGONAL_LEFT_CUT_BOTTOM_HALF,
    /* 2 */ ENMARUTA_SHAPE_DIAGONAL_LEFT_CUT_TOP_HALF,
    /* 3 */ ENMARUTA_SHAPE_DIAGONAL_RIGHT_CUT_BOTTOM_HALF,
    /* 4 */ ENMARUTA_SHAPE_DIAGONAL_RIGHT_CUT_TOP_HALF,
    /* 5 */ ENMARUTA_SHAPE_VERTICAL_CUT_LEFT_HALF,
    /* 6 */ ENMARUTA_SHAPE_VERTICAL_CUT_RIGHT_HALF,
    /* 7 */ ENMARUTA_SHAPE_HORIZONTAL_CUT_BOTTOM_HALF,
    /* 8 */ ENMARUTA_SHAPE_HORIZONTAL_CUT_TOP_HALF,
} EnMarutaShape;

s32 sShapesAfterMeleeWeaponAnimations[] = {
    ENMARUTA_SHAPE_VERTICAL_CUT_LEFT_HALF,         // PLAYER_MWA_FORWARD_SLASH_1H
    ENMARUTA_SHAPE_VERTICAL_CUT_LEFT_HALF,         // PLAYER_MWA_FORWARD_SLASH_2H
    ENMARUTA_SHAPE_DIAGONAL_RIGHT_CUT_BOTTOM_HALF, // PLAYER_MWA_FORWARD_COMBO_1H
    ENMARUTA_SHAPE_DIAGONAL_RIGHT_CUT_BOTTOM_HALF, // PLAYER_MWA_FORWARD_COMBO_2H
    ENMARUTA_SHAPE_HORIZONTAL_CUT_BOTTOM_HALF,     // PLAYER_MWA_RIGHT_SLASH_1H
    ENMARUTA_SHAPE_HORIZONTAL_CUT_BOTTOM_HALF,     // PLAYER_MWA_RIGHT_SLASH_2H
    ENMARUTA_SHAPE_HORIZONTAL_CUT_BOTTOM_HALF,     // PLAYER_MWA_RIGHT_COMBO_1H
    ENMARUTA_SHAPE_HORIZONTAL_CUT_BOTTOM_HALF,     // PLAYER_MWA_RIGHT_COMBO_2H
    ENMARUTA_SHAPE_DIAGONAL_RIGHT_CUT_BOTTOM_HALF, // PLAYER_MWA_LEFT_SLASH_1H
    ENMARUTA_SHAPE_DIAGONAL_RIGHT_CUT_BOTTOM_HALF, // PLAYER_MWA_LEFT_SLASH_2H
    ENMARUTA_SHAPE_DIAGONAL_RIGHT_CUT_BOTTOM_HALF, // PLAYER_MWA_LEFT_COMBO_1H
    ENMARUTA_SHAPE_DIAGONAL_RIGHT_CUT_BOTTOM_HALF, // PLAYER_MWA_LEFT_COMBO_2H
    ENMARUTA_SHAPE_HORIZONTAL_CUT_BOTTOM_HALF,     // PLAYER_MWA_STAB_1H
    ENMARUTA_SHAPE_HORIZONTAL_CUT_BOTTOM_HALF,     // PLAYER_MWA_STAB_2H
    ENMARUTA_SHAPE_DIAGONAL_RIGHT_CUT_BOTTOM_HALF, // PLAYER_MWA_STAB_COMBO_1H
    ENMARUTA_SHAPE_DIAGONAL_RIGHT_CUT_BOTTOM_HALF, // PLAYER_MWA_STAB_COMBO_2H
    ENMARUTA_SHAPE_WHOLE,                          // PLAYER_MWA_FLIPSLASH_START
    ENMARUTA_SHAPE_WHOLE,                          // PLAYER_MWA_JUMPSLASH_START
    ENMARUTA_SHAPE_WHOLE,                          // PLAYER_MWA_ZORA_JUMPKICK_START
    ENMARUTA_SHAPE_WHOLE,                          // PLAYER_MWA_FLIPSLASH_FINISH
    ENMARUTA_SHAPE_VERTICAL_CUT_LEFT_HALF,         // PLAYER_MWA_JUMPSLASH_FINISH
    ENMARUTA_SHAPE_WHOLE,                          // PLAYER_MWA_ZORA_JUMPKICK_FINISH
    ENMARUTA_SHAPE_WHOLE,                          // PLAYER_MWA_BACKSLASH_RIGHT
    ENMARUTA_SHAPE_WHOLE,                          // PLAYER_MWA_BACKSLASH_LEFT
    ENMARUTA_SHAPE_WHOLE,                          // PLAYER_MWA_GORON_PUNCH_LEFT
    ENMARUTA_SHAPE_WHOLE,                          // PLAYER_MWA_GORON_PUNCH_RIGHT
    ENMARUTA_SHAPE_WHOLE,                          // PLAYER_MWA_GORON_PUNCH_BUTT
    ENMARUTA_SHAPE_WHOLE,                          // PLAYER_MWA_ZORA_PUNCH_LEFT
    ENMARUTA_SHAPE_WHOLE,                          // PLAYER_MWA_ZORA_PUNCH_COMBO
    ENMARUTA_SHAPE_WHOLE,                          // PLAYER_MWA_ZORA_PUNCH_KICK
    ENMARUTA_SHAPE_HORIZONTAL_CUT_BOTTOM_HALF,     // PLAYER_MWA_SPIN_ATTACK_1H
    ENMARUTA_SHAPE_HORIZONTAL_CUT_BOTTOM_HALF,     // PLAYER_MWA_SPIN_ATTACK_2H
    ENMARUTA_SHAPE_HORIZONTAL_CUT_BOTTOM_HALF,     // PLAYER_MWA_BIG_SPIN_1H
    ENMARUTA_SHAPE_HORIZONTAL_CUT_BOTTOM_HALF      // PLAYER_MWA_BIG_SPIN_2H
};

Vec3f sDiagonalLeftCutTopHalfVelocity = { -2.0f, 3.0f, 0.0f };
UNUSED Vec3f sDiagonalLeftCutBottomHalfVelocity = { -2.0f, 3.0f, 0.0f };
Vec3f sDiagonalRightCutTopHalfVelocitySlideOff = { 2.0f, 3.0f, 0.0f };
Vec3f sDiagonalRightCutTopHalfVelocityUpAndOver = { -2.0f, 10.0f, 0.0f };
Vec3f sVerticalCutLeftHalfVelocityForwardSlash = { -2.5f, 5.0f, 0.0f };
Vec3f sVerticalCutLeftHalfVelocityJumpSlash = { -3.0f, 10.0f, 0.0f };
Vec3f sVerticalCutRightHalfVelocityForwardSlash = { 2.5f, 5.0f, 0.0f };
Vec3f sVerticalCutRightHalfVelocityJumpSlash = { 3.0f, 10.0f, 0.0f };
Vec3f sHorizontalCutTopHalfVelocityLeft = { -1.0f, 7.0f, -1.0f };
Vec3f sHorizontalCutTopHalfVelocityRight = { 1.0f, 7.0f, -1.0f };
Vec3f sHorizontalCutBottomHalfVelocityLeft = { 0.0f, 8.0f, -1.5f };
UNUSED Vec3f sHorizontalCutBottomHalfVelocityRight = { 0.0f, 8.0f, -1.5f };

Vec3f sTranslationVectorsToRecoilAxis[] = {
    { 0.0f, 0.0f, 0.0f },    { 0.0f, 0.0f, 0.0f },    { -10.0f, 460.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },    { 10.0f, 460.0f, 0.0f }, { -40.0f, 315.0f, 0.0f },
    { 40.0f, 315.0f, 0.0f }, { 0.0f, 140.0f, 0.0f },  { 0.0f, 445.0f, 0.0f },
};

Vec3f sRecoilSpinAxesForShapes[] = {
    { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f },
};

Vec3f sDiagonalLeftCutTopHalfHitboxPoints[] = {
    { 90.0f, 630.0f, -78.0f }, { -90.0f, 630.0f, -78.0f }, { -90.0f, 630.0f, 78.0f }, { 90.0f, 630.0f, 78.0f },
    { 90.0f, 350.0f, -78.0f }, { -90.0f, 170.0f, -78.0f }, { -90.0f, 170.0f, 78.0f }, { 90.0f, 350.0f, 78.0f },
};

Vec3f sDiagonalRightCutTopHalfHitboxPoints[] = {
    { 90.0f, 630.0f, -78.0f }, { -90.0f, 630.0f, -78.0f }, { -90.0f, 630.0f, 78.0f }, { 90.0f, 630.0f, 78.0f },
    { 90.0f, 170.0f, -78.0f }, { -90.0f, 350.0f, -78.0f }, { -90.0f, 350.0f, 78.0f }, { 90.0f, 170.0f, 78.0f },
};

Vec3f sVerticalCutLeftHalfHitboxPoints[] = {
    { 0.0f, 630.0f, -104.0f }, { -90.0f, 630.0f, -52.0f }, { -90.0f, 630.0f, 52.0f }, { 0.0f, 630.0f, 104.0f },
    { 0.0f, 0.0f, -104.0f },   { -90.0f, 0.0f, -52.0f },   { -90.0f, 0.0f, 52.0f },   { 0.0f, 0.0f, 104.0f },
};

Vec3f sVerticalCutRightHalfHitboxPoints[] = {
    { 0.0f, 630.0f, -104.0f }, { 90.0f, 630.0f, -52.0f }, { 90.0f, 630.0f, 52.0f }, { 0.0f, 630.0f, 104.0f },
    { 0.0f, 0.0f, -104.0f },   { 90.0f, 0.0f, -52.0f },   { 90.0f, 0.0f, 52.0f },   { 0.0f, 0.0f, 104.0f },
};

Vec3f sHorizontalCutTopHalfHitboxPoints[] = {
    { 90.0f, 630.0f, -78.0f }, { -90.0f, 630.0f, -78.0f }, { -90.0f, 630.0f, 78.0f }, { 90.0f, 630.0f, 78.0f },
    { 90.0f, 260.0f, -78.0f }, { -90.0f, 260.0f, -78.0f }, { -90.0f, 260.0f, 78.0f }, { 90.0f, 260.0f, 78.0f },
};

Vec3f sHorizontalCutBottomHalfHitboxPoints[] = {
    { 90.0f, 260.0f, -78.0f }, { -90.0f, 260.0f, -78.0f }, { -90.0f, 260.0f, 78.0f }, { 90.0f, 260.0f, 78.0f },
    { 90.0f, 20.0f, -78.0f },  { -90.0f, 20.0f, -78.0f },  { -90.0f, 20.0f, 78.0f },  { 90.0f, 20.0f, 78.0f },
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
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
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 12, 65, 0, { 0, 0, 0 } },
};

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x0),
    /* Deku Stick     */ DMG_ENTRY(0, 0xF),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(0, 0x0),
    /* Zora boomerang */ DMG_ENTRY(0, 0x0),
    /* Normal arrow   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(2, 0xF),
    /* Hookshot       */ DMG_ENTRY(0, 0x1),
    /* Goron punch    */ DMG_ENTRY(2, 0xF),
    /* Sword          */ DMG_ENTRY(1, 0xF),
    /* Goron pound    */ DMG_ENTRY(1, 0xF),
    /* Fire arrow     */ DMG_ENTRY(0, 0x0),
    /* Ice arrow      */ DMG_ENTRY(0, 0x0),
    /* Light arrow    */ DMG_ENTRY(0, 0x0),
    /* Goron spikes   */ DMG_ENTRY(1, 0xF),
    /* Deku spin      */ DMG_ENTRY(0, 0x1),
    /* Deku bubble    */ DMG_ENTRY(0, 0x0),
    /* Deku launch    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x0),
    /* Zora barrier   */ DMG_ENTRY(0, 0x0),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(0, 0xF),
    /* Zora punch     */ DMG_ENTRY(0, 0x0),
    /* Spin attack    */ DMG_ENTRY(1, 0xF),
    /* Sword beam     */ DMG_ENTRY(0, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(0, 0x0),
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 8, 0, 0, 0, MASS_HEAVY };

Vec3f sZeroVec = { 0.0f, 0.0f, 0.0f };

void EnMaruta_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnMaruta* this = (EnMaruta*)thisx;
    s32 i;

    Actor_SetScale(&this->actor, 0.1f);

    this->actor.attentionRangeType = ATTENTION_RANGE_6;

    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 50.0f;

    this->actor.colChkInfo.health = 1;
    this->actor.world.pos.y -= 4.0f;
    this->actor.home.pos.y -= 4.0f;

    this->shape = ENMARUTA_GET_SHAPE(&this->actor);
    this->recoilSpinAngle = 0;
    this->recoilSpinRate = 0;
    this->relativeHitboxPoints = NULL;
    this->endingBounces = 0;

    this->recoilSpinAxis.x = 0.0f;
    this->recoilSpinAxis.y = 0.0f;
    this->recoilSpinAxis.z = 1.0f;

    for (i = 0; i < ARRAY_COUNT(this->hitboxPoints); i++) {
        this->hitboxPoints[i] = sZeroVec;
    }

    this->lowestPointIndex = -1;
    this->isRetracting = false;

    if (this->shape == ENMARUTA_SHAPE_WHOLE) {
        Collider_InitCylinder(play, &this->collider);
        Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
        CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    }

    if (this->shape == ENMARUTA_SHAPE_WHOLE) {
        if (ENMARUTA_GET_TYPE(&this->actor) == ENMARUTA_INIT_ON_FLOOR) {
            // Happens in theory, never seen in practice
            EnMaruta_SetInPositionWhole(this);
        } else {
            EnMaruta_SetupRiseThroughFloor(this);
        }
    } else {
        EnMaruta_SetupRecoilAfterCut(this, play);
    }
}

void EnMaruta_Destroy(Actor* thisx, PlayState* play) {
    EnMaruta* this = (EnMaruta*)thisx;

    if (this->shape == ENMARUTA_SHAPE_WHOLE) {
        Collider_DestroyCylinder(play, &this->collider);
    }
}

void EnMaruta_SetInPositionWhole(EnMaruta* this) {
    this->actionFunc = EnMaruta_SittingWhole;
}

void EnMaruta_SittingWhole(EnMaruta* this, PlayState* play) {
    s16 yawDiff = BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.shape.rot.y);

    if (yawDiff > 0x1555) {
        this->actor.shape.rot.y += 0x2AAA;
    } else if (yawDiff < -0x1555) {
        this->actor.shape.rot.y -= 0x2AAA;
    }

    if (this->isRetracting == true) {
        EnMaruta_StartRetracting(this);
    }

    if ((this->actor.parent != NULL) && (this->actor.parent->id == ACTOR_EN_KENDO_JS)) {
        EnKendoJs* kendoJs = (EnKendoJs*)this->actor.parent;

        kendoJs->isPlayerLockedOn = this->actor.isLockedOn;
    }
}

void EnMaruta_SetupRiseThroughFloor(EnMaruta* this) {
    this->actor.world.pos.y = this->actor.home.pos.y - 630.0f;
    this->actionFunc = EnMaruta_RiseThroughFloor;
}

void EnMaruta_RiseThroughFloor(EnMaruta* this, PlayState* play) {
    if (Math_SmoothStepToF(&this->actor.world.pos.y, this->actor.home.pos.y, 0.4f, 100.0f, 10.0f) == 0.0f) {
        EnMaruta_SetInPositionWhole(this);
    }
}

void EnMaruta_StartRetracting(EnMaruta* this) {
    this->collider.base.acFlags |= AC_HIT;
    this->actor.velocity.y = 0.0f;
    this->actor.gravity = -2.0f;
    this->actionFunc = EnMaruta_RetractWhole;
}

void EnMaruta_RetractWhole(EnMaruta* this, PlayState* play) {
    if ((this->actor.floorHeight - 630.0f) < this->actor.world.pos.y) {
        this->actor.velocity.y += this->actor.gravity;
        this->actor.world.pos.y += this->actor.velocity.y;
    } else {
        if ((this->actor.parent != NULL) && (this->actor.parent->id == ACTOR_EN_KENDO_JS)) {
            EnKendoJs* kendoJs = (EnKendoJs*)this->actor.parent;

            kendoJs->numLogs--;
        }
        Actor_Kill(&this->actor);
    }
}

void EnMaruta_BottomHalfSittingOnFloor(EnMaruta* this) {
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    if (this->actionFunc != EnMaruta_RetractWhole) {
        this->timer = 0;
        this->actor.gravity = -2.0f;
        this->actionFunc = EnMaruta_RetractAfterCut;
    }
}

void EnMaruta_RetractAfterCut(EnMaruta* this, PlayState* play) {
    if (this->timer == 40) {
        Actor_Kill(&this->actor);
        return;
    }

    if (((this->actor.floorHeight - 630.0f) < this->actor.world.pos.y) && (this->timer > 30)) {
        this->actor.velocity.y += this->actor.gravity;
        this->actor.world.pos.y += this->actor.velocity.y;
    }

    this->timer++;
}

void EnMaruta_SetupRecoilAfterCut(EnMaruta* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f initialVelocity;
    s16 rotY;
    Vec3f spinAxis;

    this->timer = 0;
    this->endingBounces = 0;
    this->actor.gravity = -2.0f;

    switch (this->shape) {
        case ENMARUTA_SHAPE_DIAGONAL_LEFT_CUT_TOP_HALF:
            initialVelocity = sDiagonalLeftCutTopHalfVelocity;
            this->relativeHitboxPoints = sDiagonalLeftCutTopHalfHitboxPoints;
            break;

        case ENMARUTA_SHAPE_DIAGONAL_RIGHT_CUT_TOP_HALF:
            if (player->meleeWeaponAnimation == PLAYER_MWA_LEFT_SLASH_1H) {
                initialVelocity = sDiagonalRightCutTopHalfVelocitySlideOff;
            } else {
                initialVelocity = sDiagonalRightCutTopHalfVelocityUpAndOver;
            }
            this->relativeHitboxPoints = sDiagonalRightCutTopHalfHitboxPoints;
            break;

        case ENMARUTA_SHAPE_VERTICAL_CUT_LEFT_HALF:
            if (player->meleeWeaponAnimation == PLAYER_MWA_FORWARD_SLASH_1H) {
                initialVelocity = sVerticalCutLeftHalfVelocityForwardSlash;
            } else {
                initialVelocity = sVerticalCutLeftHalfVelocityJumpSlash;
            }
            this->relativeHitboxPoints = sVerticalCutLeftHalfHitboxPoints;
            break;

        case ENMARUTA_SHAPE_VERTICAL_CUT_RIGHT_HALF:
            if (player->meleeWeaponAnimation == PLAYER_MWA_FORWARD_SLASH_1H) {
                initialVelocity = sVerticalCutRightHalfVelocityForwardSlash;
            } else {
                initialVelocity = sVerticalCutRightHalfVelocityJumpSlash;
            }
            this->relativeHitboxPoints = sVerticalCutRightHalfHitboxPoints;
            break;

        case ENMARUTA_SHAPE_HORIZONTAL_CUT_TOP_HALF:
            if (player->meleeWeaponAnimation == PLAYER_MWA_RIGHT_SLASH_1H) {
                initialVelocity = sHorizontalCutTopHalfVelocityLeft;
            } else {
                initialVelocity = sHorizontalCutTopHalfVelocityRight;
            }
            this->relativeHitboxPoints = sHorizontalCutTopHalfHitboxPoints;
            break;

        case ENMARUTA_SHAPE_HORIZONTAL_CUT_BOTTOM_HALF:
            initialVelocity = sHorizontalCutBottomHalfVelocityLeft;
            this->relativeHitboxPoints = sHorizontalCutBottomHalfHitboxPoints;
            break;

        default:
            initialVelocity = sZeroVec;
            break;
    }

    rotY = this->actor.shape.rot.y;
    this->actor.velocity.x = (initialVelocity.x * Math_CosS(rotY) + (Math_SinS(rotY) * initialVelocity.z));
    this->actor.velocity.y = initialVelocity.y;
    this->actor.velocity.z = (-initialVelocity.x * Math_SinS(rotY) + (Math_CosS(rotY) * initialVelocity.z));

    spinAxis = sRecoilSpinAxesForShapes[this->shape];

    EnMaruta_RotateVector(&spinAxis, &this->recoilSpinAxis, Rand_Next() & 0xFFF, Rand_Next() & 0xFFF, 0);
    this->recoilSpinRate = Rand_Next() & 0x7FF;

    if (this->shape == ENMARUTA_SHAPE_HORIZONTAL_CUT_BOTTOM_HALF) {
        this->recoilSpinRate |= 0x3F;
    } else {
        this->recoilSpinRate |= 0xFF;
    }

    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = EnMaruta_Recoil;
}

void EnMaruta_Recoil(EnMaruta* this, PlayState* play) {
    this->recoilSpinAngle += this->recoilSpinRate;
    this->actor.velocity.y += this->actor.gravity;
    EnMaruta_Bounce(this, play);
    Actor_UpdatePos(&this->actor);
}

void EnMaruta_StopBouncing(EnMaruta* this) {
    this->timer = 0;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->recoilSpinRate = 0;
    this->actionFunc = EnMaruta_FindRestingOrientation;
}

void EnMaruta_FindRestingOrientation(EnMaruta* this, PlayState* play) {
    if (this->timer == 40) {
        EnMaruta_SetupWaitToFlatten(this);
    } else {
        this->timer++;
    }
    EnMaruta_LerpSpinAngleToFlat(&this->recoilSpinAngle);
}

void EnMaruta_SetupWaitToFlatten(EnMaruta* this) {
    s16 isRestingOnSide = this->recoilSpinAngle & 0x7FFF;

    if (((this->shape == ENMARUTA_SHAPE_VERTICAL_CUT_LEFT_HALF) ||
         (this->shape == ENMARUTA_SHAPE_VERTICAL_CUT_RIGHT_HALF)) &&
        !isRestingOnSide) {
        this->timer = 100;
    } else {
        this->timer = 0;
    }
    this->actionFunc = EnMaruta_WaitToFlatten;
}

void EnMaruta_WaitToFlatten(EnMaruta* this, PlayState* play) {
    if (this->timer > 100) {
        this->actor.colChkInfo.health = 0;
    } else {
        this->timer++;
    }
}

void EnMaruta_ShouldFlatten(EnMaruta* this) {
    this->actionFunc = EnMaruta_Flatten;
}

/** After this log has rested long enough, flatten and eventually destroy it. */
void EnMaruta_Flatten(EnMaruta* this, PlayState* play) {
    if (this->actor.scale.y == 0.0f) {
        if (this->actor.scale.x == 0.0f) {
            Actor_Kill(&this->actor);
            return;
        }
        Math_SmoothStepToF(&this->actor.scale.x, 0.0f, 0.2f, 0.01f, 0.001f);
        Math_SmoothStepToF(&this->actor.scale.z, 0.0f, 0.2f, 0.01f, 0.001f);
    } else {
        Math_SmoothStepToF(&this->actor.scale.y, 0.0f, 0.2f, 0.003f, 0.001f);
    }
}

s32 EnMaruta_IsHittable(EnMaruta* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 yawDiff = BINANG_SUB(this->actor.yawTowardsPlayer, 0x8000);

    yawDiff = BINANG_SUB(yawDiff, player->actor.shape.rot.y);
    if ((ABS_ALT(yawDiff) < 0x1555) || ((player->meleeWeaponState != PLAYER_MELEE_WEAPON_STATE_0) &&
                                        ((player->meleeWeaponAnimation == PLAYER_MWA_RIGHT_SLASH_1H) ||
                                         (player->meleeWeaponAnimation == PLAYER_MWA_RIGHT_COMBO_1H) ||
                                         (player->meleeWeaponAnimation == PLAYER_MWA_SPIN_ATTACK_1H) ||
                                         (player->meleeWeaponAnimation == PLAYER_MWA_BIG_SPIN_1H)))) {
        return true;
    }
    return false;
}

void EnMaruta_LerpSpinAngleToFlat(s16* curSpinAngle) {
    s16 flatAngle = *curSpinAngle & 0xC000;
    s16 mod = *curSpinAngle & 0x3FFF;

    if (mod > 0x2000) {
        flatAngle += 0x4000;
    }
    Math_SmoothStepToS(curSpinAngle, flatAngle, 1, 0xAAA, 0xB6);
}

/** Checks whether it's time to remove this broken piece of log. */
void EnMaruta_CheckShouldFlatten(EnMaruta* this) {
    if ((this->actionFunc != EnMaruta_Flatten) && (this->actor.colChkInfo.health == 0)) {
        EnMaruta_ShouldFlatten(this);
    }
}

void EnMaruta_UpdateCollider(EnMaruta* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((this->actionFunc == EnMaruta_SittingWhole) || (this->actionFunc == EnMaruta_RiseThroughFloor) ||
        (this->actionFunc == EnMaruta_RetractAfterCut) || (this->actionFunc == EnMaruta_Flatten) ||
        ((this->actionFunc == EnMaruta_RetractWhole) &&
         !(this->actor.world.pos.y < (this->actor.floorHeight - 20.0f)))) {
        if ((this->collider.base.acFlags & AC_HIT) && (this->actionFunc == EnMaruta_SittingWhole)) {
            this->collider.base.acFlags &= ~AC_HIT;
            Actor_PlaySfx(&this->actor, NA_SE_IT_SWORD_STRIKE);

            if (sShapesAfterMeleeWeaponAnimations[player->meleeWeaponAnimation] != 0) {
                s32 spawnParams = sShapesAfterMeleeWeaponAnimations[player->meleeWeaponAnimation] + 1;

                spawnParams = (spawnParams << 8) & 0xFF00;
                this->shape = sShapesAfterMeleeWeaponAnimations[player->meleeWeaponAnimation];
                Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_MARUTA, this->actor.world.pos.x,
                                   this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0,
                                   spawnParams);
                this->actor.world.rot.y = this->actor.shape.rot.y;
                if ((this->shape == ENMARUTA_SHAPE_VERTICAL_CUT_LEFT_HALF) ||
                    ((this->shape == ENMARUTA_SHAPE_HORIZONTAL_CUT_BOTTOM_HALF) &&
                     (player->meleeWeaponAnimation == PLAYER_MWA_STAB_1H))) {
                    EnMaruta_SetupRecoilAfterCut(this, play);
                } else {
                    EnMaruta_BottomHalfSittingOnFloor(this);
                }

                if ((this->actor.parent != NULL) && (this->actor.parent->id == ACTOR_EN_KENDO_JS)) {
                    EnKendoJs* kendoJs = (EnKendoJs*)this->actor.parent;

                    kendoJs->isSlashingLog = true;
                    kendoJs->numLogs--;
                }
                return;
            }
        }

        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);

        if (EnMaruta_IsHittable(this, play) && (this->actionFunc == EnMaruta_SittingWhole)) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }
    }
}

void EnMaruta_Bounce(EnMaruta* this, PlayState* play) {
    Vec3f lowestPoint = this->centerPos;
    s32 lowestPointIndex = -1;
    s32 i;
    f32 distanceBelowFloor;

    for (i = 0; i < ARRAY_COUNT(this->hitboxPoints); i++) {
        if (this->hitboxPoints[i].y < lowestPoint.y) {
            lowestPoint = this->hitboxPoints[i];
            lowestPointIndex = i;
        }
        this->lowestPointIndex = lowestPointIndex;
    }

    if (lowestPoint.y < this->actor.floorHeight) {
        this->recoilSpinAngle -= this->recoilSpinRate;

        distanceBelowFloor = this->actor.floorHeight - lowestPoint.y;
        this->actor.velocity.y -= this->actor.gravity;
        this->actor.velocity.x *= 0.6f;
        this->actor.velocity.z *= 0.6f;
        this->actor.world.pos.y += distanceBelowFloor;

        if (this->actor.velocity.y < -this->actor.gravity) {
            EnMaruta_SpawnDustClouds(play, lowestPoint);
            Actor_PlaySfx(&this->actor, NA_SE_EV_LOG_BOUND);
            this->actor.velocity.y *= -0.6f;
            EnMaruta_AdjustBounceAngle(this, &lowestPoint);
        }
    }
}

f32 EnMaruta_CalculateSpinDeterminantOnBounce(Vec3f* axis, Vec3f* centerPoint, Vec3f* outerPoint) {
    f32 x0 = axis->x;
    f32 z0 = axis->z;
    f32 dx = outerPoint->x - centerPoint->x;
    f32 dz = outerPoint->z - centerPoint->z;

    return z0 * dx - x0 * dz;
}

void EnMaruta_AdjustBounceAngle(EnMaruta* this, Vec3f* lowestPoint) {
    Vec3f spinAxis;
    s32 pad;
    f32 det;
    f32 bound;

    EnMaruta_RotateVector(&this->recoilSpinAxis, &spinAxis, 0, this->actor.shape.rot.y, 0);
    det = EnMaruta_CalculateSpinDeterminantOnBounce(&spinAxis, &this->centerPos, lowestPoint);

    if ((this->shape == ENMARUTA_SHAPE_VERTICAL_CUT_LEFT_HALF) ||
        (this->shape == ENMARUTA_SHAPE_VERTICAL_CUT_RIGHT_HALF)) {
        bound = 8.0f;
    } else if (this->shape == ENMARUTA_SHAPE_DIAGONAL_RIGHT_CUT_TOP_HALF) {
        bound = 5.0f;
    } else {
        bound = 2.0f;
    }

    if ((det < bound) && (-bound < det)) {
        bound = 3.0f;
    } else {
        bound = 1.2f;
    }

    if (det > 0.0f) {
        if (this->recoilSpinRate > 0) {
            this->recoilSpinRate *= bound;
        } else {
            this->recoilSpinRate *= -0.8f;
        }
    } else {
        if (this->recoilSpinRate > 0) {
            this->recoilSpinRate *= -0.8f;
        } else {
            this->recoilSpinRate *= bound;
        }
    }

    if ((ABS_ALT(this->recoilSpinRate) < 0x38E) &&
        (((this->recoilSpinAngle & 0x3FFF) < 0x71C) || ((this->recoilSpinAngle & 0x3FFF) >= 0x38E4))) {
        this->actor.gravity *= 0.8f;
        this->endingBounces++;
        if (this->endingBounces == 2) {
            EnMaruta_StopBouncing(this);
        }
    } else if (ABS_ALT(this->recoilSpinRate) > 0x38E) {
        if (this->recoilSpinRate < 0) {
            this->recoilSpinRate = -0x38E;
        } else {
            this->recoilSpinRate = 0x38E;
        }
    }
}

void EnMaruta_RotateVector(Vec3f* src, Vec3f* dest, s16 rotX, s16 rotY, s32 rotZ) {
    Matrix_Push();
    Matrix_RotateZYX(rotX, rotY, rotZ, MTXMODE_NEW);
    Matrix_MultVec3f(src, dest);
    Matrix_Pop();
}

void EnMaruta_SpawnDustClouds(PlayState* play, Vec3f srcPoint) {
    Vec3f pos = srcPoint;
    Vec3f velocity;
    Vec3f accel;
    Color_RGBA8 primColor = { 170, 130, 90, 255 };
    Color_RGBA8 envColor = { 100, 60, 20, 255 };
    s32 i;

    accel.y = 0.0f;
    pos.y += 15.0f;

    for (i = 0; i < 10; i++) {
        velocity.x = Rand_Centered() * 10.0f;
        velocity.y = 2.0f * Rand_ZeroOne();
        velocity.z = Rand_Centered() * 10.0f;
        accel.x = -0.2f * velocity.x;
        accel.z = -0.2f * velocity.z;
        func_800B0EB0(play, &pos, &velocity, &accel, &primColor, &envColor, 60, 20, 10);
    }
}

void EnMaruta_Update(Actor* thisx, PlayState* play) {
    EnMaruta* this = (EnMaruta*)thisx;

    this->actionFunc(this, play);

    EnMaruta_UpdateCollider(this, play);
    EnMaruta_CheckShouldFlatten(this);
}

void EnMaruta_Draw(Actor* thisx, PlayState* play) {
    EnMaruta* this = (EnMaruta*)thisx;
    Vec3f translate;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    if (this->shape == ENMARUTA_SHAPE_WHOLE) {
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, gPracticeLogWholeDL);
    } else {
        translate = sTranslationVectorsToRecoilAxis[this->shape];

        Matrix_Push();
        Matrix_Translate(translate.x, translate.y, translate.z, MTXMODE_APPLY);
        Matrix_RotateAxisS(this->recoilSpinAngle, &this->recoilSpinAxis, MTXMODE_APPLY);
        Matrix_Translate(-translate.x, -translate.y, -translate.z, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

        for (i = 0; i < 8; i++) {
            if (sFragmentsByShape[this->shape] & (1 << i)) {
                gSPDisplayList(POLY_OPA_DISP++, sDisplayLists[i]);
            }
        }

        if (this->relativeHitboxPoints != NULL) {
            for (i = 0; i < ARRAY_COUNT(this->hitboxPoints); i++) {
                Matrix_MultVec3f(&this->relativeHitboxPoints[i], &this->hitboxPoints[i]);
            }
            Matrix_MultVec3f(&translate, &this->centerPos);
        }

        Matrix_Pop();
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
