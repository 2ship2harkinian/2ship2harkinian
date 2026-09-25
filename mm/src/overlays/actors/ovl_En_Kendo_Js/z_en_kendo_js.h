#ifndef Z_EN_KENDO_JS_H
#define Z_EN_KENDO_JS_H

#include "global.h"
#include "objects/object_js/object_js.h"

struct EnKendoJs;

typedef void (*EnKendoJsActionFunc)(struct EnKendoJs*, PlayState*);

#define ENKENDOJS_GET_LOCATION(thisx) ((thisx)->params & 0xFF)
#define ENKENDOJS_GET_PATH_INDEX(thisx) (((thisx)->params & 0xFF00) >> 8)

#define ENKENDOJS_IN_BACK_ROOM 1

typedef struct EnKendoJs {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ SkelAnime skelAnime;
    /* 0x1D4 */ EnKendoJsActionFunc actionFunc;
    /* 0x1D8 */ Vec3s jointTable[OBJECT_JS_LIMB_MAX];
    /* 0x226 */ Vec3s morphTable[OBJECT_JS_LIMB_MAX];
    /* 0x274 */ Vec3s* pathPoints;
    /* 0x278 */ Vec3s headRot;
    /* 0x27E */ Vec3s torsoRot;
    /* 0x284 */ union {
        s16 noviceRound;
        s16 expertRound;
    } minigameRound;
    /* 0x286 */ s16 courseState;
    /* 0x288 */ s16 curTextId;
    /* 0x28A */ s16 hasSpoken;
    /* 0x28C */ s16 numLogs;
    /* 0x28E */ s16 isSlashingLog;
    /* 0x290 */ s16 timer;
    /* 0x292 */ u8 isPlayerLockedOn;
} EnKendoJs; // size = 0x294

#endif // Z_EN_KENDO_JS_H
