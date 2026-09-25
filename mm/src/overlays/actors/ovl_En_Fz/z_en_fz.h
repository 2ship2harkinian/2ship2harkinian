#ifndef Z_EN_FZ_H
#define Z_EN_FZ_H

#include "global.h"

struct EnFz;

typedef void (*EnFzActionFunc)(struct EnFz*, PlayState*);
typedef void (*EnFzUnkFunc)(struct EnFz*);

#define ENFZ_GET_POWER(thisx) ((thisx)->params & 0xF)
#define ENFZ_GET_ROTATION_LIMIT(thisx) (((thisx)->params & 0x3000) >> 0xC)
// appears out of the ground imediately
#define ENFZ_GET_APPEAR_TYPE(thisx) ((thisx)->params & 0x4000)
// if ENFZ_GET_TRACK_TYPE and NOT ENFZ_GET_APPEAR_TYPE (0x8000)
// appears from the ground if player approaches, skates after them
#define ENFZ_GET_TRACK_TYPE(thisx) ((thisx)->params & 0x8000)
// Track and Appear have a mixed behavior if both are enabled:
// if ENFZ_GET_TRACK_TYPE AND ENFZ_GET_APPEAR_TYPE (0xC000)
// appears from the ground immediately, stays still, 
// but follows player to aim, while ignoring rotation limit

// Clock specifies where the internal clock starts, zero is random
// used to force specific attack timing
// used in Snowhead Temple: 0x3, 0x7, 0xD in room 02
// causing them to breath at the bridge in series so the player has to time the jump
// the value passed is 1/10 final value in frames
#define ENFZ_GETZ_CLOCK(thisx) ((thisx)->shape.rot.z)

// 0,1,2 are how powerful (distance) the attack is
// F is treated like 0 (else case)
typedef enum EnFzPower {
    /* 0 */ FZ_POWER_0, // +  0
    /* 1 */ FZ_POWER_1, // + 10
    /* 2 */ FZ_POWER_2, // + 20
    /* 3 */ FZ_POWER_PASSIVE, // unfinished empty actionfunc
    /* F */ FZ_POWER_F = 0xF // snowhead map room
} EnFzPower;

typedef enum EnFzEffectType {
    /* 0 */ FZ_EFFECT_DISABLED,
    /* 1 */ FZ_EFFECT_MIST_AURA,
    /* 2 */ FZ_EFFECT_BREATH
} EnFzEffectType;

typedef struct EnFzEffect {
    /* 0x00 */ u8 type;
    /* 0x01 */ u8 timer;
    /* 0x04 */ Vec3f pos;
    /* 0x10 */ Vec3f velocity;
    /* 0x1C */ Vec3f accel;
    /* 0x28 */ UNK_TYPE1 unk28[0x4];
    /* 0x2C */ s16 primAlpha;
    /* 0x2E */ s16 primAlphaState;
    /* 0x30 */ f32 xyScale;
    /* 0x34 */ f32 xyScaleTarget;
    /* 0x38 */ u8 damaging;
} EnFzEffect; // size = 0x3C

// indexes the different mist functions
typedef enum EnFzState {
    /* 0 */ FZ_STATE_HIDDEN,   // OOT, waiting for player approach, ice cavern
    /* 1 */ FZ_STATE_FULLSIZE,
    /* 2 */ FZ_STATE_CHANGING, // OOT, growing and disappearing, ice cavern
    /* 3 */ FZ_STATE_MELTING, // fire arrows
} EnFzState;

typedef struct EnFz {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnFzActionFunc actionFunc;
    /* 0x148 */ ColliderCylinder collider1;
    /* 0x194 */ ColliderCylinder collider2; // might be hard protection ac
    /* 0x1E0 */ ColliderCylinder collider3; // at for breath
    /* 0x22C */ Vec3f wallHitPos;
    /* 0x238 */ f32 distToTargetSq;
    /* 0x23C */ EnFzEffect effects[40];
    /* 0xB9C */ f32 drawDmgEffAlpha;
    /* 0xBA0 */ f32 drawDmgEffScale;
    /* 0xBA4 */ s16 drawDmgEffTimer;
    /* 0xBA8 */ Vec3f originPos;
    /* 0xBB4 */ f32 originPosY; 
    /* 0xBB8 */ f32 unkBB8; // set (135.0f), never read
    /* 0xBBC */ f32 speedXZ;
    /* 0xBC0 */ u32 envAlpha;
    // we set it, and read it, but never the same value
    //   assumed unfinished shared variable with wizrobe
    /* 0xBC4 */ s16 wizrobeFlag;
    /* 0xBC6 */ s16 internalClock;
    /* 0xBC8 */ s16 unusedTimer; // set (0), never read
    /* 0xBCA */ s16 mainTimer;
    /* 0xBCC */ u8 isBgEnabled;
    /* 0xBCD */ u8 isMoving;
    /* 0xBCE */ u8 isColliderActive;
    /* 0xBCF */ u8 hitCounter; /// set, never read
    /* 0xBD0 */ s16 unkBD0; // set (0), never read
    /* 0xBD2 */ s16 unkBD2; // set (0,4000), never read
    /* 0xBD4 */ UNK_TYPE1 unkBD4[2];
    /* 0xBD6 */ u8 state;
    /* 0xBD7 */ u8 drawBody; 
    /* 0xBD8 */ u8 isDying;
    /* 0xBD9 */ u8 attackTimer; // never set, always zero
} EnFz; /* size = 0xBDC */

#endif // Z_EN_FZ_H
