#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Input;

// Returns ocarina note buttons (BTN_A / BTN_CDOWN / BTN_CRIGHT / BTN_CLEFT / BTN_CUP)
// derived from the microphone, or 0. Only active while the ocarina is open (it is
// called from AudioOcarina_ReadControllerInput) and the enhancement is enabled via
// the "gEnhancements.Playback.MicOcarina" CVar. Notes are matched by their pitch
// intervals against the game's songs (any instrument, any key); a matched, unlocked
// song is played into the input as if its buttons had been pressed.
uint32_t MicOcarina_GetButtons(Input* input);

// Live state for the Mic Ocarina monitor window / debugging.
typedef struct MicOcarinaDebug {
    int enabled;        // enhancement on
    int deviceOpen;     // capture device open
    int callCount;      // times GetButtons ran (proves the ocarina hook is live)
    float rms;          // last input level (int16 units)
    float tonality;     // last autocorrelation peak (0..1)
    float pitchHz;       // last detected pitch (0 if none)
    float pitchSemis;   // pitch in semitones above D4
    int inNote;         // currently tracking a note
    float noteSemis;    // running pitch of the current note
    int noteFrames;     // frames the current note has been held
    int eventCount;     // closed note events recorded
    float eventSemis[16];
    int eventFrames[16];
    int eventIsNote[16]; // 1 if the event was long enough to count as a note
    int playingSong;    // song id being played back (-1 none)
    int playingNote;    // index of the note being played back
    const char* playingName; // name of the song being fed to the game ("" = none)
    int matchPending;   // 1 during the beat between recognition and playback
    int songCount;      // number of known songs
    // for each song: 1 if unlocked, trailing matched interval run
    int songUnlocked[20];
    int songRun[20];
    int songPitchCount[20];
    const char* songNames[20];
    // Rolling input-level history for the HUD waveform: one sample per game
    // frame, written oldest-first (a ring). waveformFrames is the total number
    // written so far; the valid samples are the last 96 of them.
    int waveformFrames;
    float waveform[96];
} MicOcarinaDebug;

void MicOcarina_GetDebug(MicOcarinaDebug* out);

// Re-opens the capture device after the input device setting changed.
void MicOcarina_ReloadDevice(void);

// Closes the capture device when the ocarina hook has been quiet for a
// while (ocarina put away / enhancement off), so the microphone isn't held
// open. Call regularly, e.g. from the HUD's per-frame draw.
void MicOcarina_IdleCheck(void);

#ifdef __cplusplus
}
#endif
