#include "2s2h/Mouse.h"

#include "ship/Context.h"

static MouseCoords current = {};

#ifdef __cplusplus
extern "C" {
#endif

void Mouse_Update() {
    Ship::Coords coords = Ship::Context::GetRawInstance()->GetWindow()->GetMouseDelta();
    current.x = coords.x;
    current.y = coords.y;
}

MouseCoords Mouse_GetDelta() {
    return current;
}

MouseCoords Mouse_GetPos() {
    Ship::Coords coords = Ship::Context::GetRawInstance()->GetWindow()->GetMousePos();
    return { coords.x, coords.y };
}

void Mouse_SetCursorPos(s32 x, s32 y) {
    Ship::Context::GetRawInstance()->GetWindow()->SetMousePos({ x, y });
}

bool Mouse_IsCaptured() {
    return Ship::Context::GetRawInstance()->GetWindow()->IsMouseCaptured();
}

#ifdef __cplusplus
} // extern "C"
#endif
