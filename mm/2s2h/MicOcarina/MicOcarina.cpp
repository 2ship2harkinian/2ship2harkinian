// MicOcarina — play the ocarina with your voice or a real instrument into the microphone.
//
// While the ocarina is open, the microphone listens for notes. Instead of mapping
// absolute pitch onto the five game notes (which only works if your instrument is in
// the game's key), the spaces between your notes — the pitch intervals — are matched
// against Majora's Mask's song interval profiles, so any instrument in any key works:
// play the Song of Healing on an alto C ocarina and the game recognizes it.
//
// When the interval sequence of the last notes you played matches a song you have
// unlocked, the song's button sequence is played into the ocarina input as if you
// had pressed the buttons yourself, so the game accepts it through its own logic.
//
// The capture device can be chosen via the "gEnhancements.Playback.MicOcarinaDevice"
// CVar (empty = system default). Live state is exposed via MicOcarina_GetDebug for
// the Mic Ocarina monitor window.
#include "MicOcarina.h"
#include <chrono>
#include <cmath>
#include <cstring>
#include <atomic>
#include <spdlog/spdlog.h>
#include <SDL2/SDL.h>
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "z64.h"
#include "z64ocarina.h"
}

namespace {

constexpr int kSampleRate = 48000;
constexpr int kWindow = 4096;                // ~85 ms analysis window
constexpr int kMinLag = kSampleRate / 2000;  // 24 -> 2000 Hz
constexpr int kMaxLag = kSampleRate / 60;    // 800 -> 60 Hz
constexpr int kRmsGate = 300;                // silence gate, in int16 units
constexpr double kTonalGate = 0.55;          // autocorrelation peak acceptance
constexpr double kRefHz = 293.66477;         // D4, the reference for semitone values

// All frame counts are in HOOK TICKS, which run at ~20 Hz (50 ms each) — the
// ocarina input scan is not per-render-frame. Calibrated to that rate.
constexpr int kNoteMinFrames = 3;    // ~200 ms: the shortest note that counts
constexpr int kNoteQuietFrames = 2;  // ~100 ms of quiet ends a note
constexpr int kEventGapFrames = 30;  // ~1.5 s of silence forgets the sequence
constexpr double kIntervalTolerance = 1.25; // semitones of error allowed per interval
constexpr double kHoldTol = 1.0;   // semitones: still within the current note (vibrato swings back inside)
constexpr double kGlideTol = 1.25; // clearly a different note than the one being held
constexpr int kGlideFrames = 4;    // ~300 ms of sustained pitch departure before splitting — vibrato
                                  // excursions return within ~150 ms and never reach this; a real note change does
constexpr double kReattackRatio = 0.55; // rms dip below this fraction of the note's peak = a stop/re-tongue
constexpr int kDipFrames = 2;     // ~100 ms dipped ends a note even at the same pitch
                                  // (repeated notes: A-A, D-D, F-F can't be split by pitch change)
constexpr double kMinSemis = -12.0; // pitches outside this range are noise (e.g. the headset's
constexpr double kMaxSemis = 24.0;  // 1846 Hz beep at +31.8) — never commit them as notes

// The buttons each game pitch plays (D4, F4, A4, B4, D5 -> A, C-down, C-right, C-left, C-up).
constexpr uint32_t kPitchButtons[5] = { BTN_A, BTN_CDOWN, BTN_CRIGHT, BTN_CLEFT, BTN_CUP };
// The game's five ocarina pitches in semitones above D4.
constexpr int kPitchSemis[5] = { 0, 3, 7, 9, 12 };

struct SongProfile {
    uint8_t songId;
    uint8_t pitchCount;
    uint8_t pitches[10];
    uint8_t lengths[10];
    uint8_t questSlot; // QUEST_SONG_* to check unlocked, 0xFF = no quest check
    const char* name;
};

constexpr SongProfile kSongs[] = {
    // OCARINA_SONG_SONATA: D5 B4 D5 B4 D4 A4 D4
    { OCARINA_SONG_SONATA, 7, { 4, 3, 4, 3, 0, 2, 0 }, { 19, 19, 19, 38, 39, 76, 77 }, QUEST_SONG_SONATA,
      "Sonata of Awakening" },
    // OCARINA_SONG_GORON_LULLABY: D4 A4 B4 D4 A4 B4 A4 D4
    { OCARINA_SONG_GORON_LULLABY, 8, { 0, 2, 3, 0, 2, 3, 2, 0 }, { 41, 40, 39, 42, 40, 39, 41, 80 },
      QUEST_SONG_LULLABY, "Goron Lullaby" },
    // OCARINA_SONG_NEW_WAVE: B4 D5 B4 A4 F4 B4 A4
    { OCARINA_SONG_NEW_WAVE, 7, { 3, 4, 3, 2, 1, 3, 2 }, { 64, 13, 12, 78, 12, 13, 114 },
      QUEST_SONG_BOSSA_NOVA, "New Wave Bossa Nova" },
    // OCARINA_SONG_ELEGY: A4 B4 A4 F4 A4 D5 B4
    { OCARINA_SONG_ELEGY, 7, { 2, 3, 2, 1, 2, 4, 3 }, { 85, 43, 43, 21, 21, 43, 85 }, QUEST_SONG_ELEGY,
      "Elegy of Emptiness" },
    // OCARINA_SONG_OATH: A4 F4 D4 F4 A4 D5
    { OCARINA_SONG_OATH, 6, { 2, 1, 0, 1, 2, 4 }, { 97, 48, 49, 49, 48, 97 }, QUEST_SONG_OATH,
      "Oath to Order" },
    // OCARINA_SONG_SARIAS: F4 A4 B4 F4 A4 B4
    { OCARINA_SONG_SARIAS, 6, { 1, 2, 3, 1, 2, 3 }, { 17, 17, 34, 17, 17, 136 }, QUEST_SONG_SARIA,
      "Saria's Song" },
    // OCARINA_SONG_TIME: A4 D4 F4 A4 D4 F4
    { OCARINA_SONG_TIME, 6, { 2, 0, 1, 2, 0, 1 }, { 32, 65, 33, 32, 65, 99 }, QUEST_SONG_TIME,
      "Song of Time" },
    // OCARINA_SONG_HEALING: B4 A4 F4 B4 A4 F4
    { OCARINA_SONG_HEALING, 6, { 3, 2, 1, 3, 2, 1 }, { 32, 33, 33, 32, 33, 121 }, QUEST_SONG_HEALING,
      "Song of Healing" },
    // OCARINA_SONG_EPONAS: D5 B4 A4 D5 B4 A4
    { OCARINA_SONG_EPONAS, 6, { 4, 3, 2, 4, 3, 2 }, { 18, 18, 72, 18, 18, 144 }, QUEST_SONG_EPONA,
      "Epona's Song" },
    // OCARINA_SONG_SOARING: F4 B4 D5 F4 B4 D5
    { OCARINA_SONG_SOARING, 6, { 1, 3, 4, 1, 3, 4 }, { 18, 18, 36, 18, 18, 108 }, QUEST_SONG_SOARING,
      "Song of Soaring" },
    // OCARINA_SONG_STORMS: D4 F4 D5 D4 F4 D5
    { OCARINA_SONG_STORMS, 6, { 0, 1, 4, 0, 1, 4 }, { 11, 11, 45, 11, 11, 90 }, QUEST_SONG_STORMS,
      "Song of Storms" },
    // OCARINA_SONG_SUNS: A4 F4 D5 A4 F4 D5
    { OCARINA_SONG_SUNS, 6, { 2, 1, 4, 2, 1, 4 }, { 12, 13, 29, 12, 13, 120 }, QUEST_SONG_SUN, "Sun's Song" },
    // OCARINA_SONG_INVERTED_TIME: F4 D4 A4 F4 D4 A4
    { OCARINA_SONG_INVERTED_TIME, 6, { 1, 0, 2, 1, 0, 2 }, { 32, 65, 33, 32, 65, 99 }, QUEST_SONG_TIME,
      "Inverted Song of Time" },
    // OCARINA_SONG_DOUBLE_TIME: A4 A4 D4 D4 F4 F4
    { OCARINA_SONG_DOUBLE_TIME, 6, { 2, 2, 0, 0, 1, 1 }, { 29, 30, 29, 30, 29, 99 }, QUEST_SONG_TIME,
      "Song of Double Time" },
    // OCARINA_SONG_GORON_LULLABY_INTRO: D4 A4 B4 D4 A4 B4
    { OCARINA_SONG_GORON_LULLABY_INTRO, 6, { 0, 2, 3, 0, 2, 3 }, { 32, 33, 33, 32, 33, 130 },
      QUEST_SONG_LULLABY_INTRO, "Goron Lullaby Intro" },
    // OCARINA_SONG_ZELDAS_LULLABY: B4 D5 A4 B4 D5 A4 (no quest item; free play only)
    { OCARINA_SONG_ZELDAS_LULLABY, 6, { 3, 4, 2, 3, 4, 2 }, { 15, 15, 45, 15, 15, 105 }, 0xFF,
      "Zelda's Lullaby" },
};
constexpr int kSongCount = sizeof(kSongs) / sizeof(kSongs[0]);
constexpr int kMaxSongs = 20;
static_assert(kSongCount <= kMaxSongs, "debug song arrays too small");

// ---- shared capture state ----
SDL_AudioDeviceID gDevice = 0;
std::atomic<int16_t> gRing[kWindow];
std::atomic<uint64_t> gWritten{ 0 }; // total frames captured

void SDLCALL CaptureCallback(void* /*userdata*/, uint8_t* stream, int len) {
    const int frames = len / 2; // mono s16
    const uint64_t start = gWritten.load(std::memory_order_relaxed);
    for (int i = 0; i < frames; i++) {
        int16_t s;
        memcpy(&s, stream + i * 2, 2);
        gRing[(start + i) % kWindow].store(s, std::memory_order_relaxed);
    }
    gWritten.store(start + frames, std::memory_order_release);
}

// True if `name` is an available capture device (nullptr/empty = default, always ok).
bool DeviceExists(const char* name) {
    if (name == nullptr || name[0] == '\0') {
        return true;
    }
    const int n = SDL_GetNumAudioDevices(1);
    for (int i = 0; i < n; i++) {
        const char* d = SDL_GetAudioDeviceName(i, 1);
        if (d != nullptr && strcmp(d, name) == 0) {
            return true;
        }
    }
    return false;
}

bool EnsureDevice() {
    if (gDevice != 0) {
        return true;
    }
    char* wanted = CVarGetString("gEnhancements.Playback.MicOcarinaDevice", "");
    const char* open = nullptr;
    if (wanted != nullptr && wanted[0] != '\0' && DeviceExists(wanted)) {
        open = wanted; // chosen device
    }
    SDL_AudioSpec want{}, have{};
    want.freq = kSampleRate;
    want.format = AUDIO_S16SYS;
    want.channels = 1;
    want.samples = 1024;
    want.callback = CaptureCallback;
    gDevice = SDL_OpenAudioDevice(open, 1, &want, &have, 0);
    if (gDevice == 0) {
        static bool warned = false;
        if (!warned) {
            spdlog::error("[mic-ocarina] cannot open capture device '{}': {}", open ? open : "(default)",
                          SDL_GetError());
            warned = true;
        }
        return false;
    }
    SDL_PauseAudioDevice(gDevice, 0);
    spdlog::info("[mic-ocarina] capture device '{}' opened ({} Hz, {} ch)", SDL_GetAudioDeviceName(gDevice, 1) ? SDL_GetAudioDeviceName(gDevice, 1) : "(default)", have.freq, have.channels);
    return true;
}

// Autocorrelation pitch detection; returns 0 if no confident pitch.
//
// The correlation for each lag is normalized by the FULL window's energy
// (e0), and a lag only sums the terms it actually has (kWindow - lag). The
// normalized score therefore decays with lag, so the shortest true period
// always wins — a peak at 2x/3x the period cannot beat the fundamental the
// way it can with a fixed-term partial correlation (which produced octave
// errors, e.g. hearing a 928 Hz whistle as 464 Hz).
double DetectPitch(double* outTonality, double* outRms) {
    *outTonality = 0;
    *outRms = 0;
    const uint64_t total = gWritten.load(std::memory_order_acquire);
    if (total < (uint64_t)kWindow) {
        return 0;
    }
    static thread_local double x[kWindow];
    const uint64_t end = total;
    double mean = 0;
    for (int i = 0; i < kWindow; i++) {
        x[i] = (double)gRing[(end + i - kWindow) % kWindow].load(std::memory_order_relaxed);
        mean += x[i];
    }
    mean /= kWindow;
    double e0 = 0;
    for (int i = 0; i < kWindow; i++) {
        x[i] -= mean;
        e0 += x[i] * x[i];
    }
    const double rms = std::sqrt(e0 / kWindow);
    *outRms = rms;
    if (rms < kRmsGate || e0 < 1.0) {
        return 0;
    }

    double best = 0, bestLag = 0;
    bool firstDone = false;
    double first = 0;
    for (int lag = kMinLag; lag <= kMaxLag; lag++) {
        double sum = 0;
        const int n = kWindow - lag;
        for (int i = 0; i < n; i++) {
            sum += x[i] * x[i + lag];
        }
        const double norm = sum / e0;
        if (!firstDone && norm > 0.85 && lag > kMinLag + 1) { // first strong peak = the period
            firstDone = true;
            first = lag;
        }
        if (firstDone && norm > best) {
            best = norm;
            bestLag = lag;
        }
        if (firstDone && best > 0.9 && lag > first * 2 && best - norm > 0.1) { // past the peak
            break;
        }
    }
    if (bestLag > kMinLag && bestLag < kMaxLag) { // parabolic refine
        double s1 = 0, s2 = 0, s3 = 0;
        const int l = (int)bestLag;
        for (int i = 0; i < kMaxLag; i++) {
            s1 += x[i] * x[i + l - 1];
            s2 += x[i] * x[i + l];
            s3 += x[i] * x[i + l + 1];
        }
        s1 /= e0, s2 /= e0, s3 /= e0;
        const double denom = s1 - 2 * s2 + s3;
        if (denom != 0) {
            const double shift = 0.5 * (s1 - s3) / denom;
            if (shift > -1 && shift < 1) {
                bestLag += shift;
            }
        }
    }
    *outTonality = best;
    if (best < kTonalGate) {
        return 0; // not tonal enough
    }
    return (double)kSampleRate / bestLag;
}

// ---- note event tracking ----
constexpr int kMaxEvents = 16;
struct NoteEvent {
    double semis;
    int frames;
};
NoteEvent sEvents[kMaxEvents];
int sEventCount = 0;
int sSilenceFrames = 0;

bool sInNote = false;
double sNoteMean = 0; // running mean semitones
int sNoteFrames = 0;
int sAwayFrames = 0;   // frames the pitch has been outside the held note's stable range
double sAwayMean = 0;  // where that departure is heading (the next note gliding in)
double sNotePeak = 0;  // loudest rms seen in the current note (slowly decaying)
int sDipFrames = 0;    // consecutive frames well below the note's peak (a stop/re-tongue)

// ---- playback state (after a song matches) ----
const SongProfile* sPlaying = nullptr;
int sPlayNote = 0;
int sPlayGap = 0;
int sPendingFrames = 0; // green "recognized" beat before feeding the song
int sCooldownFrames = 0; // after playback: ignore the mic while the game plays the song

// ---- debug state for the monitor window ----
MicOcarinaDebug gDebug{};
std::chrono::steady_clock::time_point sLastCallTime{}; // last GetButtons call
bool sHookStarted = false;                              // any GetButtons call seen yet

bool IsSongUnlocked(const SongProfile* song) {
    if (song->questSlot == 0xFF) {
        return true;
    }
    return CHECK_QUEST_ITEM(song->questSlot) != 0;
}

// How many of the song's intervals, counted from the end, the given note
// sequence matches. Works on any event list (raw or expanded).
int TrailingRun(const SongProfile* song, const NoteEvent* evs, int evCount) {
    int run = 0;
    for (int t = 1; t < song->pitchCount; t++) {
        const int evB = evCount - t;     // later note (t=1 -> the last event)
        const int evA = evCount - t - 1; // earlier note
        if (evA < 0) {
            break;
        }
        const double actual = evs[evB].semis - evs[evA].semis;
        const int wanted =
            kPitchSemis[song->pitches[song->pitchCount - t]] - kPitchSemis[song->pitches[song->pitchCount - t - 1]];
        if (std::fabs(actual - wanted) <= kIntervalTolerance) {
            run++;
        } else {
            break;
        }
    }
    return run;
}

// Does the chosen event for each song note fit the song once transposed? A whistle
// drifts note to note, so rather than demanding every interval be right (errors
// compound: one flat note spoils two intervals) fit one transposition to the
// whole phrase and require each note to land near its place on it.
constexpr double kFitTolerance = 1.4; // semitones of per-note error after the best transposition

bool FitsSong(const SongProfile* song, const int* idx) {
    double offset = 0;
    for (int k = 0; k < song->pitchCount; k++) {
        offset += sEvents[idx[k]].semis - kPitchSemis[song->pitches[k]];
    }
    offset /= song->pitchCount;
    for (int k = 0; k < song->pitchCount; k++) {
        if (std::fabs(sEvents[idx[k]].semis - kPitchSemis[song->pitches[k]] - offset) > kFitTolerance) {
            return false;
        }
    }
    return true;
}

bool MatchesSong(const SongProfile* song) {
    if (sEventCount < song->pitchCount) {
        return false;
    }
    int idx[10];
    for (int k = 0; k < song->pitchCount; k++) {
        idx[k] = sEventCount - song->pitchCount + k;
    }
    return FitsSong(song, idx);
}

// Forgiving match: a clipped key or a roll into the next note leaves a short
// "ghost" event between (or after) the real notes. Allow up to kMaxSkips such
// short events to be skipped, anywhere in the trailing run, and still match.
// Only short events can be skipped, so a deliberate wrong note never is.
constexpr int kMaxSkips = 2;
constexpr int kGhostFrames = 8; // ~400 ms: longer than this is a real note, not a slip

bool LooseRec(const SongProfile* song, int j, int e, int skipsLeft, int* idx) {
    idx[j] = e;
    if (j == 0) {
        return FitsSong(song, idx);
    }
    const int wanted = kPitchSemis[song->pitches[j]] - kPitchSemis[song->pitches[j - 1]];
    for (int s = 0; s <= skipsLeft; s++) {
        const int prev = e - 1 - s;
        if (prev < 0) {
            return false;
        }
        if (s > 0 && sEvents[prev + 1].frames > kGhostFrames) {
            return false; // the event we'd be skipping is a real note
        }
        if (std::fabs((sEvents[e].semis - sEvents[prev].semis) - wanted) <= 2.0 &&
            LooseRec(song, j - 1, prev, skipsLeft - s, idx)) {
            return true;
        }
    }
    return false;
}

bool MatchesSongLoose(const SongProfile* song, int* skipsUsed) {
    for (int budget = 0; budget <= kMaxSkips; budget++) {
        for (int s = 0; s <= budget; s++) { // trailing ghosts after the song's last note
            const int last = sEventCount - 1 - s;
            if (last < song->pitchCount - 1) {
                break;
            }
            if (s > 0 && sEvents[last + 1].frames > kGhostFrames) {
                break;
            }
            int idx[10];
            if (LooseRec(song, song->pitchCount - 1, last, budget - s, idx)) {
                *skipsUsed = budget;
                return true;
            }
        }
    }
    return false;
}

// A repeated pair (Song of Double Time's A-A, D-D, F-F) is often played as one
// long note with no gap or dip to split it. Last-resort match: let a long note
// stand for the pair. Only used when nothing else matched, and only when the
// phrase has little else in it, and every other song is tried first, so legato Song of Time (six long
// notes, three of which would merge-match) is never read as Double Time.
constexpr int kMergeFrames = 9; // ~450 ms held

bool MergedRec(const SongProfile* song, int j, int e, int* idx, int* endE) {
    if (j < 0) {
        *endE = e;
        return true;
    }
    if (e < 0) {
        return false;
    }
    idx[j] = e;
    if (MergedRec(song, j - 1, e - 1, idx, endE)) {
        return true;
    }
    if (j > 0 && song->pitches[j] == song->pitches[j - 1] && sEvents[e].frames >= kMergeFrames) {
        idx[j] = e;
        idx[j - 1] = e;
        return MergedRec(song, j - 2, e - 1, idx, endE);
    }
    return false;
}

bool MatchesSongMerged(const SongProfile* song) {
    int idx[10];
    int endE = 0;
    if (!MergedRec(song, song->pitchCount - 1, sEventCount - 1, idx, &endE)) {
        return false;
    }
    const int used = sEventCount - 1 - endE;
    if (used >= song->pitchCount || sEventCount > used + 4) { // nothing merged, or too much other playing
        return false;
    }
    return FitsSong(song, idx);
}

void ResetEvents() {
    sEventCount = 0;
    sSilenceFrames = 0;
    sInNote = false;
    sNoteMean = 0;
    sNoteFrames = 0;
    sAwayFrames = 0;
    sAwayMean = 0;
    sDipFrames = 0;
    sPendingFrames = 0;
}

void CloseNote() {
    if (sInNote && sNoteFrames >= kNoteMinFrames && sNoteMean >= kMinSemis && sNoteMean <= kMaxSemis) {
        if (sEventCount == kMaxEvents) { // drop the oldest
            memmove(&sEvents[0], &sEvents[1], sizeof(NoteEvent) * (kMaxEvents - 1));
            sEventCount--;
        }
        sEvents[sEventCount].semis = sNoteMean;
        sEvents[sEventCount].frames = sNoteFrames;
        sEventCount++;
        spdlog::info("[mic-ocarina] note #{}: {:+.2f} semis ({:.0f} Hz), {} frames", sEventCount, sNoteMean,
                     kRefHz * std::pow(2.0, sNoteMean / 12.0), sNoteFrames);
    }
    sInNote = false;
    sNoteMean = 0;
    sNoteFrames = 0;
    sAwayFrames = 0;
    sAwayMean = 0;
    sDipFrames = 0;
}

void UpdateDebug(double f, double tonality, double rms, double semis) {
    gDebug.enabled = CVarGetInteger("gEnhancements.Playback.MicOcarina", 0) != 0;
    gDebug.deviceOpen = gDevice != 0;
    gDebug.rms = (float)rms;
    gDebug.tonality = (float)tonality;
    gDebug.pitchHz = (float)f;
    gDebug.pitchSemis = (float)semis;
    gDebug.inNote = sInNote;
    gDebug.noteSemis = (float)sNoteMean;
    gDebug.noteFrames = sNoteFrames;
    gDebug.eventCount = sEventCount;
    for (int i = 0; i < sEventCount && i < kMaxEvents; i++) {
        gDebug.eventSemis[i] = (float)sEvents[i].semis;
        gDebug.eventFrames[i] = sEvents[i].frames;
        gDebug.eventIsNote[i] = 1;
    }
    gDebug.playingSong = sPlaying != nullptr ? sPlaying->songId : -1;
    gDebug.playingNote = sPlaying != nullptr ? sPlayNote : -1;
    gDebug.matchPending = (sPlaying != nullptr && sPendingFrames > 0) ? 1 : 0;
    gDebug.songCount = kSongCount;
    for (int i = 0; i < kSongCount; i++) {
        gDebug.songUnlocked[i] = IsSongUnlocked(&kSongs[i]) ? 1 : 0;
        gDebug.songRun[i] = TrailingRun(&kSongs[i], sEvents, sEventCount); // raw run for the monitor
        gDebug.songNames[i] = kSongs[i].name;
        gDebug.songPitchCount[i] = kSongs[i].pitchCount;
    }
    // waveform history for the HUD (ring buffer; size must match MicOcarina.h)
    gDebug.waveform[gDebug.waveformFrames % (int)(sizeof(gDebug.waveform) / sizeof(gDebug.waveform[0]))] =
        (float)rms;
    gDebug.waveformFrames++;
    gDebug.playingName = sPlaying != nullptr ? sPlaying->name : "";
}

} // namespace

extern "C" void MicOcarina_ReloadDevice(void) {
    if (gDevice != 0) {
        SDL_CloseAudioDevice(gDevice);
        gDevice = 0;
    }
}

extern "C" void MicOcarina_IdleCheck(void) {
    if (gDevice == 0) {
        return;
    }
    const auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration<double>(now - sLastCallTime).count() > 2.0) {
        SDL_CloseAudioDevice(gDevice);
        gDevice = 0;
        spdlog::info("[mic-ocarina] capture device closed (ocarina put away)");
    }
}

extern "C" void MicOcarina_GetDebug(MicOcarinaDebug* out) {
    if (out == nullptr) {
        return;
    }
    *out = gDebug;
    out->enabled = CVarGetInteger("gEnhancements.Playback.MicOcarina", 0) != 0;
    out->deviceOpen = gDevice != 0;
    // keep song lists fresh even while the ocarina is closed
    out->songCount = kSongCount;
    for (int i = 0; i < kSongCount; i++) {
        out->songUnlocked[i] = IsSongUnlocked(&kSongs[i]) ? 1 : 0;
        out->songRun[i] = 0; // runs are only meaningful while listening
        out->songNames[i] = kSongs[i].name;
        out->songPitchCount[i] = kSongs[i].pitchCount;
    }
}

extern "C" uint32_t MicOcarina_GetButtons(Input* input) {
    gDebug.callCount++;
    const auto now = std::chrono::steady_clock::now();
    const double sinceLast = std::chrono::duration<double>(now - sLastCallTime).count();
    sLastCallTime = now;
    if (sHookStarted && sinceLast > 0.75) {
        // The ocarina was closed since the last call (calls come every game
        // frame while it is open). Never resume an old match or playback.
        sPlaying = nullptr;
        ResetEvents();
        spdlog::info("[mic-ocarina] hook resumed after {:.2}s - state reset", sinceLast);
    }
    sHookStarted = true;

    if (!CVarGetInteger("gEnhancements.Playback.MicOcarina", 0)) {
        if (sPlaying == nullptr) {
            ResetEvents();
        }
        return 0;
    }
    if (!EnsureDevice()) {
        return 0;
    }

    double tonality = 0, rms = 0;
    const double f = DetectPitch(&tonality, &rms);
    const double semis = f > 0 ? 12.0 * std::log2(f / kRefHz) : 0.0;

    if (sPlaying != nullptr) { // a matched song is playing back
        if (sPendingFrames > 0) { // recognized: hold a green beat before feeding it
            sPendingFrames--;
            UpdateDebug(f, tonality, rms, semis);
            return 0;
        }
        if (sPlayGap > 0) {
            sPlayGap--;
            UpdateDebug(f, tonality, rms, semis);
            return 0;
        }
        // Snappy playback: a brief press per note, short gap between them.
        // The game registers a note on the press — no need to hold for the
        // song's full length, so the whole sequence lands in under a second.
        sPlayGap = 3;
        const uint32_t btn = kPitchButtons[sPlaying->pitches[sPlayNote]];
        spdlog::info("[mic-ocarina] feeding note {}/{} btn=0x{:x}", sPlayNote + 1, sPlaying->pitchCount, btn);
        const int id = sPlaying->songId;
        sPlayNote++;
        if (sPlayNote >= sPlaying->pitchCount) { // the whole song has been fed
            sPlaying = nullptr;
            ResetEvents();
            sCooldownFrames = 60; // ~3 s: the speakers play the song back at us; don't re-hear it
            UpdateDebug(f, tonality, rms, semis);
            spdlog::info("[mic-ocarina] finished playing song id {} to the game", id);
        } else {
            UpdateDebug(f, tonality, rms, semis);
        }
        return btn;
    }

    // Listening: segment the mic into note events, watch for a song's interval run.
    if (sCooldownFrames > 0) {
        sCooldownFrames--;
        ResetEvents(); // stay deaf while the game plays the matched song's audio
        UpdateDebug(f, tonality, rms, semis);
        return 0;
    }

    if (f > 0) {
        sSilenceFrames = 0;
        if (!sInNote) {
            sInNote = true;
            sNoteMean = semis;
            sNoteFrames = 1;
            sAwayFrames = 0;
            sAwayMean = 0;
            sNotePeak = rms;
            sDipFrames = 0;
        } else if (rms < sNotePeak * kReattackRatio) {
            // Loudness collapsed to a fraction of the note's peak while the
            // pitch stayed: a stop / re-tongue between two notes at the same
            // pitch (A-A, D-D, F-F in Song of Double Time). Close the note;
            // the still-ongoing sound opens a fresh one.
            sDipFrames++;
            if (sDipFrames >= kDipFrames) {
                CloseNote();
                sInNote = true;
                sNoteMean = semis;
                sNoteFrames = 1;
                sAwayFrames = 0;
                sAwayMean = 0;
                sNotePeak = rms;
                sDipFrames = 0;
            }
        } else if (std::fabs(semis - sNoteMean) <= kHoldTol) {
            sNoteFrames++;
            sNoteMean += (semis - sNoteMean) * 0.15; // refine slowly, vibrato absorbed
            sAwayFrames = 0;
            sDipFrames = 0;
            sNotePeak = std::max(sNotePeak * 0.99, rms); // slow decay so long quiet-ish tails can still dip
        } else {
            // The pitch has left the note's stable range: either an attack
            // transient or a legato glide into the next note. Track where the
            // departure is heading and split after a couple of frames if it
            // is clearly a different note — so you can play legato without
            // stopping between notes.
            sAwayFrames++;
            sAwayMean += (semis - sAwayMean) * 0.5;
            sNoteFrames++; // the held note still counts time while we decide
            if (sAwayFrames >= kGlideFrames && std::fabs(sAwayMean - sNoteMean) > kGlideTol) {
                // Save the departure values first: CloseNote resets the tracker.
                const double newMean = sAwayMean;
                const int newFrames = sAwayFrames;
                CloseNote(); // record the note we were holding
                sInNote = true;
                sNoteMean = newMean; // the glide is the start of the next note
                sNoteFrames = newFrames;
                sAwayFrames = 0;
                sAwayMean = 0;
                sNotePeak = rms;
                sDipFrames = 0;
            }
        }
    } else {
        sAwayFrames = 0;
        sSilenceFrames++;
        if (sInNote && sSilenceFrames >= kNoteQuietFrames) {
            CloseNote();
        } else if (!sInNote && sSilenceFrames >= kEventGapFrames) {
            ResetEvents(); // long silence forgets the sequence
        }
    }

    // after each closed note, look for a song match: exact first, then forgiving
    for (int pass = 0; pass < 3; pass++) {
        for (const SongProfile& song : kSongs) {
            int skips = 0;
            const bool hit = pass == 0   ? MatchesSong(&song)
                             : pass == 1 ? MatchesSongLoose(&song, &skips)
                                         : MatchesSongMerged(&song);
            if (IsSongUnlocked(&song) && hit) {
                spdlog::info("[mic-ocarina] recognized '{}' ({} notes){} - feeding buttons shortly", song.name,
                             song.pitchCount, pass == 0 ? "" : (pass == 1 ? " (forgave a slip)" : " (held note = pair)"));
                sPlaying = &song;
                sPlayNote = 0;
                sPlayGap = 0;
                sPendingFrames = 15; // ~0.75 s of green "recognized" feedback first
                UpdateDebug(f, tonality, rms, semis);
                return 0;
            }
        }
    }
    UpdateDebug(f, tonality, rms, semis);
    return 0;
}
