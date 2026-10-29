#define _USE_MATH_DEFINES

#include <libultraship/bridge/consolevariablebridge.h>
#include <math.h>

#include "GameInteractor/GameInteractor.h"
#include "ShipInit.hpp"
#include "2s2h/Mouse.h"

// Algorithm Overview:
// 1. Cross products filter: Checks for consistent curvature (filters straight lines)
// 2. Angle step validation: Rejects direction reversals (filters zigzag patterns)
// 3. Size validation: Ensures gesture has sufficient magnitude (filters tiny circles)
// 4. Rotation validation: Ensures sufficient angular change (filters partial circles)
//
// Parameter Relationships:
// - Base window size determines detection window duration
// - Consistency ratio determines how many frames must show valid curvature
// - Rotation threshold scales with window size (larger window = more rotation expected)
// - Displacement threshold scales with base size (larger gestures = bigger movements)

#define MOUSE_ENABLED (Mouse_IsCaptured() && CVarGetInteger("gSettings.EnableMouse", 0))
#define SENSE_X CVarGetFloat("gEnhancements.Camera.RightStick.CameraSensitivity.X", 1.0f)
#define SENSE_Y CVarGetFloat("gEnhancements.Camera.RightStick.CameraSensitivity.Y", 1.0f)

// Base parameters (tune these)
#define QS_WINDOW_SIZE 6          // Number of frames to analyze
#define QS_CONSISTENCY_RATIO 0.6f // Ratio of frames needed with consistent curvature (60%)
#define QS_MIN_CONSISTENT_CROSSES ((s32)(QS_WINDOW_SIZE * QS_CONSISTENCY_RATIO))

#define QS_ROTATION_COVERAGE 0.75f // Fraction of full circle needed (75% = 270deg)
#define QS_MIN_STEP_FRACTION 0.20f // Min step = fraction of avg step (filters jitter)
#define QS_MAX_STEP_FRACTION 3.2f  // Max step = factor * avg step (filters reversals)
#define QS_MIN_ROTATION_RAD (QS_ROTATION_COVERAGE * 2.0f * M_PI)
#define QS_AVG_STEP_RAD (QS_MIN_ROTATION_RAD / QS_WINDOW_SIZE)
#define QS_MIN_ANGLE_STEP_RAD (QS_AVG_STEP_RAD * QS_MIN_STEP_FRACTION)
#define QS_MAX_ANGLE_STEP_RAD (QS_AVG_STEP_RAD * QS_MAX_STEP_FRACTION)

#define QS_BASE_DISPLACEMENT 150.0f // Base displacement threshold in pixels
#define QS_CROSS_MAG_FACTOR 0.003f  // Cross threshold = displacement^2 * factor
#define QS_MIN_DISPLACEMENT (QS_BASE_DISPLACEMENT * (SENSE_X + SENSE_Y) * 0.5f)
#define QS_CROSS_MAG_THRESHOLD (QS_MIN_DISPLACEMENT * QS_MIN_DISPLACEMENT * QS_CROSS_MAG_FACTOR)

#ifdef __cplusplus
extern "C" {
#endif

static f32 deltaX[QS_WINDOW_SIZE] = {};
static f32 deltaY[QS_WINDOW_SIZE] = {};
static u8 writeIndex = 0;
static u8 sampleCount = 0;

static f32 normalizeAngleRad(f32 angleRad) {
    while (angleRad > M_PI) {
        angleRad -= 2 * M_PI;
    }
    while (angleRad < -M_PI) {
        angleRad += 2 * M_PI;
    }
    return angleRad;
}

void CollectMouseVelocity(Input* input) {
    MouseCoords d = Mouse_GetDelta();

    deltaX[writeIndex] = d.x;
    deltaY[writeIndex] = d.y;

    writeIndex = (writeIndex + 1) % QS_WINDOW_SIZE;
    if (sampleCount < QS_WINDOW_SIZE) {
        sampleCount++;
    }
}

bool DetectQuickspin(bool* should, s8* controlAngles) {
    (void)controlAngles;

    if (!MOUSE_ENABLED || sampleCount < QS_WINDOW_SIZE) {
        return *should = false;
    }

    // Validation counters
    s32 positiveCrosses = 0;
    s32 negativeCrosses = 0;
    f32 totalRotation = 0;
    s32 rotationSign = 0;
    f32 prevAngle = NAN;
    f32 maxDisplacement = 0;

    // Analyze consecutive frame pairs in the window
    for (s32 i = 0; i < QS_WINDOW_SIZE; i++) {
        s32 idx0 = (writeIndex + i) % QS_WINDOW_SIZE;
        s32 idx1 = (writeIndex + i + 1) % QS_WINDOW_SIZE;

        f32 d1x = deltaX[idx0];
        f32 d1y = deltaY[idx0];
        f32 d2x = deltaX[idx1];
        f32 d2y = deltaY[idx1];

        // Check 1: Cross product curvature validation
        // Cross product sign indicates turn direction; magnitude indicates sharpness
        f32 cross = d1x * d2y - d1y * d2x;
        if (fabsf(cross) >= QS_CROSS_MAG_THRESHOLD) {
            if (cross > 0) {
                positiveCrosses++;
            } else {
                negativeCrosses++;
            }
        }

        // Track displacement magnitude for size validation
        f32 speed = sqrtf(d1x * d1x + d1y * d1y);
        if (speed > maxDisplacement) {
            maxDisplacement = speed;
        }

        // Check 2: Angle step validation (smooth rotation check)
        if (speed > 0) {
            f32 angle = atan2f(d1y, d1x);

            if (!isnan(prevAngle)) {
                f32 step = normalizeAngleRad(angle - prevAngle);
                // Reject if step is too large (direction reversal instead of smooth turn)
                if (fabsf(step) > QS_MAX_ANGLE_STEP_RAD) {
                    return *should = false;
                }
                // Accumulate steps in consistent direction
                if (fabsf(step) >= QS_MIN_ANGLE_STEP_RAD) {
                    if (rotationSign == 0) {
                        rotationSign = (step > 0) ? 1 : -1;
                    }
                    if (step * rotationSign > 0) {
                        totalRotation += step;
                    }
                }
            }
            prevAngle = angle;
        }
    }

    // Final validation: all four checks must pass
    s32 dominant = (positiveCrosses > negativeCrosses) ? positiveCrosses : negativeCrosses;

    // Check 1: Sufficient consistent curvature
    if (dominant < QS_MIN_CONSISTENT_CROSSES) {
        return *should = false;
    }

    // Check 3: Sufficient gesture size
    if (maxDisplacement < QS_MIN_DISPLACEMENT) {
        return *should = false;
    }

    // Check 4: Sufficient total rotation
    return *should = (fabsf(totalRotation) >= QS_MIN_ROTATION_RAD);
}

void RegisterQuickspinFunc() {
    COND_VB_SHOULD(VB_SHOULD_QUICKSPIN, CVarGetInteger("gEnhancements.Equipment.MouseQuickspin.Enable", 0),
                   { DetectQuickspin(should, va_arg(args, s8*)); });
    COND_HOOK(OnPassPlayerInputs, CVarGetInteger("gEnhancements.Equipment.MouseQuickspin.Enable", 0),
              CollectMouseVelocity);
}

static RegisterShipInitFunc initFunc(RegisterQuickspinFunc, { "gEnhancements.Equipment.MouseQuickspin.Enable" });

#ifdef __cplusplus
} // extern "C"
#endif
