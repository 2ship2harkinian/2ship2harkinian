#ifndef Z_EN_BOMBERS_H
#define Z_EN_BOMBERS_H

#include "global.h"
#include "objects/object_cs/object_cs.h"

struct EnBombers;

typedef void (*EnBombersActionFunc)(struct EnBombers*, PlayState*);

#define ENBOMBERS_GET_NUMBER(thisx) ((thisx)->params & 0xF)
#define ENBOMBERS_GET_TYPE(thisx) (((thisx)->params >> 4) & 0xF)

typedef enum {
    /* 0 */ ENBOMBERS_NO_1,
    /* 1 */ ENBOMBERS_NO_2,
    /* 2 */ ENBOMBERS_NO_3,
    /* 3 */ ENBOMBERS_NO_4,
    /* 4 */ ENBOMBERS_NO_5
} EnBombersNumber;

#define ENBOMBERS_TYPE_NPC 0

typedef struct EnBombers {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[OBJECT_CS_LIMB_MAX];
    /* 0x206 */ Vec3s morphTable[OBJECT_CS_LIMB_MAX];
    /* 0x284 */ EnBombersActionFunc actionFunc;
    /* 0x288 */ s16 headRotZ;
    /* 0x28A */ s16 headRotX;
    /* 0x28C */ UNK_TYPE1 unk28C[2];
    /* 0x28E */ s16 headRotTargetZ;
    /* 0x290 */ s16 headRotTargetX;
    /* 0x294 */ Vec3f walkTarget;
    /* 0x2A0 */ s16 npcState;
    /* 0x2A2 */ UNK_TYPE1 unk2A2[2];
    /* 0x2A4 */ s16 targetYaw;
    /* 0x2A6 */ s16 talkState;
    /* 0x2A8 */ s16 idleTimer;
    /* 0x2AA */ s16 walkTimer;
    /* 0x2AC */ s16 unk_2AC;
    /* 0x2B0 */ f32 animEndFrame;
    /* 0x2B4 */ f32 walkMaxStep;
    /* 0x2B8 */ s16 eyeIndex;
    /* 0x2BA */ s16 blinkTimer;
    /* 0x2BC */ s16 type;
    /* 0x2BE */ s16 number;
    /* 0x2C0 */ s16 action;
    /* 0x2C4 */ s32 animIndex;
    /* 0x2C8 */ ColliderCylinder collider;
} EnBombers; // size = 0x314

#endif // Z_EN_BOMBERS_H
