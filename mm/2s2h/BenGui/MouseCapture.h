#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void Mouse_SetForceCapture(bool isEnabled, bool value);
void Mouse_ForceToggleCapture();
void Mouse_UpdateCaptureByState();

#ifdef __cplusplus
} // extern "C"
#endif
