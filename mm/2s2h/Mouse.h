#pragma once

#include "ultratypes.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MouseCoords {
    s32 x;
    s32 y;
} MouseCoords;

void Mouse_Update();
MouseCoords Mouse_GetDelta();
MouseCoords Mouse_GetPos();
void Mouse_SetCursorPos(s32 x, s32 y);
bool Mouse_IsCaptured();

#ifdef __cplusplus
} // extern "C"
#endif
