#ifndef GAME_INTERACTOR_ACTION_H
#define GAME_INTERACTOR_ACTION_H

#ifdef __cplusplus

#include <cstdint>
#include <functional>

#include "GameInteractorParams.h"

typedef enum {
    GI_AVAILABILITY_READY,   // Apply it now.
    GI_AVAILABILITY_NOT_YET, // Temporarily blocked. Leave it queued and ask again next frame.
    GI_AVAILABILITY_NEVER,   // It will never apply. Drop it and tell the emitter.
} GIActionAvailability;

// Reported back to whoever queued the action, via onComplete.
typedef enum {
    GI_STATUS_APPLIED,    // An instant action ran, a timed one started, or a blocking one delivered.
    GI_STATUS_FINISHED,   // A timed action's duration elapsed.
    GI_STATUS_CANCELLED,  // Ended or dropped before it could finish, by a cancel or a save load.
    GI_STATUS_EXPIRED,    // The game was never ready within expiresAfter. Nothing happened; retry.
    GI_STATUS_IMPOSSIBLE, // A gate said it can never apply. Nothing happened; don't retry.
} GIActionStatus;

typedef enum {
    GI_LIFETIME_SAVE,    // Dropped when a save is loaded.
    GI_LIFETIME_SESSION, // Survives everything. Only its duration, a cancel, or the emitter ends it.
} GIActionLifetime;

typedef enum {
    // Anonymous: carries its own closures rather than coming from a registered definition.
    GI_ACTION_NONE,

    // Core mechanisms
    GI_ACTION_GIVE_ITEM,
    GI_ACTION_TRANSITION,
    GI_ACTION_SPAWN_ACTOR,
    GI_ACTION_SHOW_MESSAGE,
    GI_ACTION_SHOW_TITLE_CARD,

    // Effects
    GI_ACTION_FREEZE,
    GI_ACTION_BLAST,
    GI_ACTION_SHOCK,
    GI_ACTION_JINX,
    GI_ACTION_EMPTY_WALLET,
    GI_ACTION_SPAWN_LIKE_LIKE,
    GI_ACTION_SKIP_TIME,
    GI_ACTION_BURN,
    GI_ACTION_KNOCKBACK,
    GI_ACTION_SCALE_LINK,
    GI_ACTION_PAPER_LINK,
    GI_ACTION_SQUISH_LINK,
    GI_ACTION_MIRROR_LINK,
    GI_ACTION_DRUNK_LINK,
    GI_ACTION_BUTTON_ROULETTE,
    GI_ACTION_MOTION_BLUR,
    GI_ACTION_SHRINK_SCREEN,
    GI_ACTION_MIRROR_CAMERA,
    GI_ACTION_CAMERA_TILT,
    GI_ACTION_UPSIDE_DOWN_CAMERA,
    GI_ACTION_RANDOM_SKYBOX,
    GI_ACTION_FOG,
    GI_ACTION_BOUNCING_B_BUTTON,
    GI_ACTION_DAMAGE_MULTIPLIER,
    GI_ACTION_INVINCIBILITY,
    GI_ACTION_SPEED,
    GI_ACTION_SLIPPERY_FLOOR,
    GI_ACTION_RANDOM_WIND,
    GI_ACTION_HEALTH,
    GI_ACTION_RUPEES,
    GI_ACTION_KILL_LINK,
    GI_ACTION_RANDOMIZE_COSMETICS,
    GI_ACTION_REVERSE_CONTROLS,
    GI_ACTION_CLEAR_BUTTONS,
} GIActionId;

// Metadata for remotes and UI only.
typedef enum {
    GI_VALENCE_NEUTRAL,
    GI_VALENCE_POSITIVE,
    GI_VALENCE_NEGATIVE,
} GIValence;

// What to do when a timed action is requested while an instance of it is already running.
typedef enum {
    GI_STACK_QUEUE,   // Wait for the running one to end (NOT_YET).
    GI_STACK_REFRESH, // Restart the running one with the new request's duration and params.
} GIStacking;

// Actions in the same group write the same game state, so only one of them runs at a time.
typedef enum {
    GI_EXCLUSION_NONE,
    GI_EXCLUSION_PLAYER_SCALE,
    GI_EXCLUSION_CAMERA_ROLL,
} GIExclusionGroup;

struct GIAction;

using GIActionCallback = std::function<void(GIActionStatus)>;
using GIActionGate = std::function<GIActionAvailability(const GIAction&)>;
using GIActionFunc = std::function<void(GIAction&)>;

struct GIAction {
    // Identity for stacking and cancellation; the callbacks below are what actually runs.
    GIActionId id = GI_ACTION_NONE;
    GIParams params = {};
    // Frames the action runs for. 0 means onStart is the whole thing.
    uint32_t duration = 0;
    // Holds the queue until the action calls GameInteractor::FinishBlocking(). An onStart that
    // fails to get going should clear this so it doesn't hold the queue forever.
    bool blocking = false;
    GIActionLifetime lifetime = GI_LIFETIME_SAVE;
    // Frames to wait for READY before giving up and reporting EXPIRED. 0 waits forever.
    uint32_t expiresAfter = 0;

    // Reports APPLIED when the action lands, FINISHED/CANCELLED later if it was timed, or exactly
    // one of CANCELLED/EXPIRED/IMPOSSIBLE if it never ran.
    GIActionCallback onComplete = nullptr;
    // Checked every frame while queued, on top of GameInteractor::CanProcessActions().
    GIActionGate canApply = nullptr;

    GIActionFunc onStart = nullptr;
    GIActionFunc onTick = nullptr; // Timed only. Every frame, including the frame it started.
    GIActionFunc onEnd = nullptr;  // Timed only. When the duration elapses or it's cancelled.

    // Runtime state, written by the processor.
    uint32_t elapsed = 0;       // Frames since onStart ran.
    uint32_t framesWaiting = 0; // Frames spent queued while not READY.

    // Rvalue-only so chaining onto a factory result moves rather than copies:
    //   Queue(GIActions::GiveItem({ ... }).OnComplete([](GIActionStatus s) { ... }));
    GIAction&& OnComplete(GIActionCallback callback) && {
        onComplete = std::move(callback);
        return std::move(*this);
    }
};

#endif // __cplusplus

#endif // GAME_INTERACTOR_ACTION_H
