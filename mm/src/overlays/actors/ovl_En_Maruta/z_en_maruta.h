#ifndef Z_EN_MARUTA_H
#define Z_EN_MARUTA_H

#include "global.h"

struct EnMaruta;

typedef void (*EnMarutaActionFunc)(struct EnMaruta*, PlayState*);

#define ENMARUTA_GET_TYPE(thisx) ((thisx)->params & 0xFF)
#define ENMARUTA_GET_SHAPE(thisx) (((thisx)->params & 0xFF00) >> 8)

#define ENMARUTA_INIT_ON_FLOOR 0xFF // Hypothetical only

typedef struct EnMaruta {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnMarutaActionFunc actionFunc;
    /* 0x148 */ ColliderCylinder collider;
    /* 0x194 */ Vec3f recoilSpinAxis;
    /* 0x1A0 */ Vec3f* relativeHitboxPoints;
    /* 0x1A4 */ Vec3f hitboxPoints[8];
    /* 0x204 */ Vec3f centerPos;
    /* 0x210 */ s32 shape;
    /* 0x214 */ s32 lowestPointIndex;
    /* 0x218 */ s16 recoilSpinAngle;
    /* 0x21A */ s16 recoilSpinRate;
    /* 0x21C */ s16 endingBounces;
    /* 0x21E */ s16 timer;
    /* 0x220 */ s16 isRetracting;
} EnMaruta; // size = 0x224

#endif // Z_EN_MARUTA_H
