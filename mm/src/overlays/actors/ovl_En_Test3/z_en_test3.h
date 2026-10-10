#ifndef Z_EN_TEST3_H
#define Z_EN_TEST3_H

#include "global.h"

struct EnTest3;

typedef void (*EnTest3ActionFunc)(struct EnTest3*, PlayState*);

#define KAFEI_GET_PATH_INDEX(thisx) ((thisx)->params & 0x1F)
#define KAFEI_GET_PARAM_1E0(thisx) (((thisx)->params >> 5) & 0xF)

typedef struct {
    /* 0x0 */ u8 talkActionIndex; // EnTest3TalkState
    /* 0x1 */ u8 argument; // used for various purposes, including a timer; 0 means no next message
    /* 0x2 */ u16 textId;
} EnTest3SpeakData; // size = 0x4

typedef struct EnTest3 {
    /* 0x000 */ Player player;
    /* 0xD78 */ EnTest3SpeakData* speakData;
    /* 0xD7C */ Path* currentPath;
    /* 0xD80 */ s32 timeSpeed;
    /* 0xD84 */ f32 waypointProgress;
    /* 0xD88 */ s8 scheduleResult;
    /* 0xD89 */ u8 isTimePathStarted; // only lowest bit is used
    /* 0xD8A */ s16 bellTimer;
    /* 0xD8C */ u8 nextMsgTimer;
    /* 0xD8D */ s8 csId;
    /* 0xD8E */ s16 subCamId;
    /* 0xD90 */ Player* link;
    /* 0xD94 */ EnTest3ActionFunc mainActionFunc;
    /* 0xD98 */ Vec3f targetPos;
    /* 0xDA4 */ f32 schPathProgress;
    /* 0xDA8 */ s32 schPathRemainingTime;
    /* 0xDAC */ s32 waypointRemainingTime;
    /* 0xDB0 */ s32 waypointIndex;
    /* 0xDB4 */ s32 schPathElapsedTime;
} EnTest3; // size = 0xDB8

#endif // Z_EN_TEST3_H
