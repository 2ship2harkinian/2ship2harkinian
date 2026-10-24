#include <cmath>
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "functions.h"
#include "variables.h"
}

#include "2s2h/Enhancements/Graphics/Graphics.h"

// 2S2H [Enhancement] Telescope style stars: a round core with four diffraction spikes, drawn as geometry
// whose vertex alpha carries the shape. A static vertex array keeps it safe for display list replay.
#define CVAR_TELESCOPE_STARS "gEnhancements.Graphics.TelescopeStars"
#define CVAR_STAR_SIZE "gEnhancements.Graphics.TelescopeStarSize"
#define CVAR_STAR_INTENSITY "gEnhancements.Graphics.TelescopeStarIntensity"
#define CVAR_STAR_TWINKLE "gEnhancements.Graphics.StarTwinkle"
#define CVAR_STAR_TWINKLE_FREQUENCY "gEnhancements.Graphics.StarTwinkleFrequency"
#define CVAR_STAR_COUNT "gEnhancements.Graphics.StarCountMultiplier"

#define STAR_CORE_SEGMENTS 12
#define STAR_CORE_CENTER_ALPHA 235.0f
#define STAR_CORE_INNER_RADIUS 0.16f
#define STAR_CORE_INNER_ALPHA 105.0f
#define STAR_CORE_RADIUS 0.48f
#define STAR_ARM_MID_T 0.55f // ring position along the spike
#define STAR_ARM_LENGTH_H 0.55f
#define STAR_ARM_LENGTH_V 0.47f
#define STAR_ARM_WIDTH_BASE 0.060f
#define STAR_ARM_WIDTH_MID 0.038f
#define STAR_ARM_WIDTH_TIP 0.020f
#define STAR_ARM_ALPHA_H 130.0f
#define STAR_ARM_ALPHA_V 110.0f
#define STAR_ARM_MID_ALPHA 0.45f // ring alpha, relative to the base
#define STAR_VTX_SCALE 1000.0f   // Vtx positions are s16, hence pre-multiplied
// Intensity setting the shape alphas above are tuned for
#define STAR_REFERENCE_INTENSITY 0.5f
// Twinkle rate in Hz: each star gets its own frequency between MIN_RATIO * slider and the slider value
#define STAR_TWINKLE_MAX_FREQUENCY 2.0f
#define STAR_TWINKLE_MIN_RATIO 0.15f
// How far the twinkle dips: 0.5 goes down to half brightness, 1.0 would reach invisible
#define STAR_TWINKLE_DEPTH 0.5f
// osGetTime() counts N64 cycles
#define STAR_CYCLES_PER_SECOND 46875000.0
// Star count multiplier range
#define STAR_COUNT_MIN 1.0f
#define STAR_COUNT_MAX 3.0f
// Free bytes the polyOpa buffer must keep before the star draw stops
#define STAR_MIN_FREE_BYTES 8192

#define STAR_CENTER_VTX 0
#define STAR_CORE_INNER_VTX(i) (1 + (i))
#define STAR_CORE_OUTER_VTX(i) ((1 + STAR_CORE_SEGMENTS) + (i))
#define STAR_ARM_VTX(arm, v) ((1 + (2 * STAR_CORE_SEGMENTS)) + (6 * (arm)) + (v))
// The core and its two rings
#define STAR_CORE_VTX_COUNT ((1 + (2 * STAR_CORE_SEGMENTS)))
// Whole star, which needs two G_VTX commands on real hardware
#define STAR_VTX_COUNT (STAR_CORE_VTX_COUNT + (4 * 6))

static Vtx sStarVtx[STAR_VTX_COUNT];
static bool sStarVtxBuilt = false;
// Alpha scale the vertices were last built with, -1 forces the first build
static f32 sStarAlphaScale = -1.0f;
static MtxF sStarBillboardMtxF;

static f32 StarSizeScale() {
    f32 intensity = CLAMP(CVarGetFloat(CVAR_STAR_INTENSITY, STAR_REFERENCE_INTENSITY), 0.0f, 1.0f);

    return intensity / STAR_REFERENCE_INTENSITY;
}

// Same seed the star layout is built from, so a save always gets the same twinkle
static u32 StarSeed() {
    const u8* name = (const u8*)gSaveContext.save.saveInfo.playerData.playerName;

    return ((u32)name[0] << 0x18) ^ ((u32)name[1] << 0x14) ^ ((u32)name[2] << 0x10) ^ ((u32)name[3] << 0xC) ^
           ((u32)name[4] << 8) ^ ((u32)name[5] << 4) ^ ((u32)name[6] << 0) ^ ((u32)name[7] >> 4) ^
           ((u32)name[7] << 0x1C);
}

// Per star hash, kept out of the randInt stream of the star loop so the layout can never change
static u32 StarHash(s32 starNum) {
    u32 hash = StarSeed() ^ ((u32)starNum * 0x9E3779B1);

    hash ^= hash >> 16;
    hash *= 0x7FEB352D;
    hash ^= hash >> 15;
    hash *= 0x846CA68B;
    hash ^= hash >> 16;

    return hash;
}

// Per star twinkle factor, oscillating between (1 - STAR_TWINKLE_DEPTH) and 1
static f32 StarTwinkle(s32 starNum) {
    f32 maxFrequency = CLAMP(CVarGetFloat(CVAR_STAR_TWINKLE_FREQUENCY, 1.0f), 0.0f, STAR_TWINKLE_MAX_FREQUENCY);
    u32 hash;
    f32 frequency;
    f32 turns;
    f32 fraction;
    f32 halfDepth;

    // A maximum of 0 means no twinkling
    if (maxFrequency <= 0.0f) {
        return 1.0f;
    }

    hash = StarHash(starNum);
    frequency =
        maxFrequency * (STAR_TWINKLE_MIN_RATIO + ((1.0f - STAR_TWINKLE_MIN_RATIO) * ((f32)(hash & 0xFF) / 255.0f)));
    // Random phase, in turns
    turns = (frequency * (f32)(osGetTime() / STAR_CYCLES_PER_SECOND)) + ((f32)((hash >> 8) & 0xFFFF) / 65536.0f);
    fraction = turns - (f32)std::floor(turns);
    halfDepth = STAR_TWINKLE_DEPTH * 0.5f;

    // Math_SinS takes a 16 bit angle where a full turn is 0x10000
    return (1.0f - halfDepth) + (halfDepth * Math_SinS((s16)(s32)(fraction * 65536.0f)));
}

static s16 StarVtxCoord(f32 value) {
    f32 scaled = value * STAR_VTX_SCALE;

    // Round away from zero to keep the star symmetrical
    return (s16)((scaled >= 0.0f) ? (scaled + 0.5f) : (scaled - 0.5f));
}

// Applies the intensity setting to a shape alpha
static u8 StarAlpha(f32 baseAlpha, f32 alphaScale) {
    return (u8)(CLAMP(baseAlpha * alphaScale, 0.0f, 255.0f) + 0.5f);
}

static void SetStarVtx(Vtx* vtx, f32 x, f32 y, u8 alpha) {
    vtx->v.ob[0] = StarVtxCoord(x);
    vtx->v.ob[1] = StarVtxCoord(y);
    vtx->v.ob[2] = 0;
    vtx->v.flag = 0;
    vtx->v.tc[0] = 0;
    vtx->v.tc[1] = 0;
    vtx->v.cn[0] = 255;
    vtx->v.cn[1] = 255;
    vtx->v.cn[2] = 255;
    vtx->v.cn[3] = alpha;
}

// Builds one spike. The perpendicular is derived from the direction so every arm keeps the same winding.
static void BuildStarArm(s32 arm, f32 alongX, f32 alongY, f32 length, f32 alpha, f32 alphaScale) {
    f32 acrossX = -alongY;
    f32 acrossY = alongX;
    f32 midX = alongX * length * STAR_ARM_MID_T;
    f32 midY = alongY * length * STAR_ARM_MID_T;
    f32 tipX = alongX * length;
    f32 tipY = alongY * length;
    u8 baseAlpha = StarAlpha(alpha, alphaScale);
    u8 midAlpha = StarAlpha(alpha * STAR_ARM_MID_ALPHA, alphaScale);

    SetStarVtx(&sStarVtx[STAR_ARM_VTX(arm, 0)], acrossX * STAR_ARM_WIDTH_BASE, acrossY * STAR_ARM_WIDTH_BASE,
               baseAlpha);
    SetStarVtx(&sStarVtx[STAR_ARM_VTX(arm, 1)], -acrossX * STAR_ARM_WIDTH_BASE, -acrossY * STAR_ARM_WIDTH_BASE,
               baseAlpha);
    SetStarVtx(&sStarVtx[STAR_ARM_VTX(arm, 2)], midX - (acrossX * STAR_ARM_WIDTH_MID),
               midY - (acrossY * STAR_ARM_WIDTH_MID), midAlpha);
    SetStarVtx(&sStarVtx[STAR_ARM_VTX(arm, 3)], midX + (acrossX * STAR_ARM_WIDTH_MID),
               midY + (acrossY * STAR_ARM_WIDTH_MID), midAlpha);
    SetStarVtx(&sStarVtx[STAR_ARM_VTX(arm, 4)], tipX - (acrossX * STAR_ARM_WIDTH_TIP),
               tipY - (acrossY * STAR_ARM_WIDTH_TIP), 0);
    SetStarVtx(&sStarVtx[STAR_ARM_VTX(arm, 5)], tipX + (acrossX * STAR_ARM_WIDTH_TIP),
               tipY + (acrossY * STAR_ARM_WIDTH_TIP), 0);
}

static void BuildStarGeometry(f32 alphaScale) {
    const s32 angleStep = 0x10000 / STAR_CORE_SEGMENTS;
    s32 i;

    SetStarVtx(&sStarVtx[STAR_CENTER_VTX], 0.0f, 0.0f, StarAlpha(STAR_CORE_CENTER_ALPHA, alphaScale));

    for (i = 0; i < STAR_CORE_SEGMENTS; i++) {
        s16 angle = (s16)(angleStep * i);
        f32 cosA = Math_CosS(angle);
        f32 sinA = Math_SinS(angle);

        SetStarVtx(&sStarVtx[STAR_CORE_INNER_VTX(i)], cosA * STAR_CORE_INNER_RADIUS, sinA * STAR_CORE_INNER_RADIUS,
                   StarAlpha(STAR_CORE_INNER_ALPHA, alphaScale));
        SetStarVtx(&sStarVtx[STAR_CORE_OUTER_VTX(i)], cosA * STAR_CORE_RADIUS, sinA * STAR_CORE_RADIUS, 0);
    }

    // The horizontal spikes are longer and brighter than the vertical ones
    BuildStarArm(0, 1.0f, 0.0f, STAR_ARM_LENGTH_H, STAR_ARM_ALPHA_H, alphaScale);
    BuildStarArm(1, -1.0f, 0.0f, STAR_ARM_LENGTH_H, STAR_ARM_ALPHA_H, alphaScale);
    BuildStarArm(2, 0.0f, 1.0f, STAR_ARM_LENGTH_V, STAR_ARM_ALPHA_V, alphaScale);
    BuildStarArm(3, 0.0f, -1.0f, STAR_ARM_LENGTH_V, STAR_ARM_ALPHA_V, alphaScale);

    sStarAlphaScale = alphaScale;
    sStarVtxBuilt = true;
}

// The intensity lives in the vertex alphas, so rebuild when it changes
static void EnsureStarGeometry(f32 alphaScale) {
    if (!sStarVtxBuilt || (sStarAlphaScale != alphaScale)) {
        BuildStarGeometry(alphaScale);
    }
}

static void DrawTelescopeStar(Gfx** gfxP, PlayState* play, s32 starNum, Vec3f* pos, u32 size) {
    Gfx* gfx = *gfxP;
    f32 alphaScale = StarSizeScale();
    f32 sizeMultiplier = CLAMP(CVarGetFloat(CVAR_STAR_SIZE, 2.0f), 1.0f, 3.0f);
    f32 eyeDist;
    f32 distRatio;
    f32 x;
    f32 y;
    f32 z;
    f32 scaleMultiplier;
    f32 starScale;
    f32 spikeScaleX;
    f32 spikeScaleY;
    u32 variation;
    s32 i;

    // One time logic before drawing the first star
    if (starNum == 0) {
        EnsureStarGeometry(alphaScale);

        // billboardMtxF is already overwritten here, so derive it from the original Mtx
        Matrix_MtxToMtxF(play->billboardMtx, &sStarBillboardMtxF);

        gSPLoadGeometryMode(gfx++, G_CULL_BACK);
    }

    eyeDist = Math3D_Vec3f_DistXYZ(pos, &play->view.eye);

    if ((size >= 2) && (eyeDist > 0.0f)) {
        // Pull the star to 5500 units from the eye so the far plane cannot clip it
        distRatio = 5500.0f / eyeDist;

        x = play->view.eye.x + ((pos->x - play->view.eye.x) * distRatio);
        y = play->view.eye.y + ((pos->y - play->view.eye.y) * distRatio);
        z = play->view.eye.z + ((pos->z - play->view.eye.z) * distRatio);

        // Cancel the field of view so the apparent size stays the same
        scaleMultiplier = 3.0f / (60.0f / play->view.fovy);

        // Vary the spike lengths from star to star
        variation = (u32)starNum * 0x9E3779B1;
        spikeScaleX = 0.9f + ((variation >> 4) & 0x3) * 0.1f;
        spikeScaleY = 0.9f + ((variation >> 8) & 0x3) * 0.1f;

        // Undo the vertex pre-multiplication
        starScale = ((f32)size * sizeMultiplier * scaleMultiplier) / STAR_VTX_SCALE;

        Matrix_Push();

        Matrix_Translate(x, y, z, MTXMODE_NEW);
        Matrix_Scale(starScale * spikeScaleX, starScale * spikeScaleY, 1.0f, MTXMODE_APPLY);
        Matrix_ReplaceRotation(&sStarBillboardMtxF);

        MATRIX_FINALIZE_AND_LOAD(gfx++, play->state.gfxCtx);

        // Too many vertices for one G_VTX command, so load them in two batches
        gSPVertex(gfx++, (uintptr_t)sStarVtx, STAR_CORE_VTX_COUNT, 0);
        gSPVertex(gfx++, (uintptr_t)(sStarVtx + STAR_CORE_VTX_COUNT), STAR_VTX_COUNT - STAR_CORE_VTX_COUNT,
                  STAR_CORE_VTX_COUNT);

        // Core fan
        for (i = 0; i < STAR_CORE_SEGMENTS; i += 2) {
            s32 next = (i + 1) % STAR_CORE_SEGMENTS;
            s32 nextNext = (i + 2) % STAR_CORE_SEGMENTS;

            gSP2Triangles(gfx++, STAR_CENTER_VTX, STAR_CORE_INNER_VTX(i), STAR_CORE_INNER_VTX(next), 0, STAR_CENTER_VTX,
                          STAR_CORE_INNER_VTX(next), STAR_CORE_INNER_VTX(nextNext), 0);
        }

        // Core outer band, which softens the glow edge
        for (i = 0; i < STAR_CORE_SEGMENTS; i++) {
            s32 next = (i + 1) % STAR_CORE_SEGMENTS;

            gSP2Triangles(gfx++, STAR_CORE_INNER_VTX(i), STAR_CORE_OUTER_VTX(i), STAR_CORE_OUTER_VTX(next), 0,
                          STAR_CORE_INNER_VTX(i), STAR_CORE_OUTER_VTX(next), STAR_CORE_INNER_VTX(next), 0);
        }

        // Spikes, base band then tip band
        for (i = 0; i < 4; i++) {
            gSP2Triangles(gfx++, STAR_ARM_VTX(i, 0), STAR_ARM_VTX(i, 1), STAR_ARM_VTX(i, 2), 0, STAR_ARM_VTX(i, 0),
                          STAR_ARM_VTX(i, 2), STAR_ARM_VTX(i, 3), 0);
            gSP2Triangles(gfx++, STAR_ARM_VTX(i, 3), STAR_ARM_VTX(i, 2), STAR_ARM_VTX(i, 4), 0, STAR_ARM_VTX(i, 3),
                          STAR_ARM_VTX(i, 4), STAR_ARM_VTX(i, 5), 0);
        }

        Matrix_Pop();
    }

    *gfxP = gfx;
}

extern "C" s32 SkyboxStars_GetCount(s32 vanillaCount) {
    f32 multiplier = CLAMP(CVarGetFloat(CVAR_STAR_COUNT, STAR_COUNT_MIN), STAR_COUNT_MIN, STAR_COUNT_MAX);

    return (s32)(vanillaCount * multiplier);
}

extern "C" SkyboxStarDraw SkyboxStars_PrepareStar(Gfx** gfxP, PlayState* play, s32 index, Vec3f* pos, u32 width,
                                                  Color_RGBA8* color, f32 fade) {
    Gfx* gfx = *gfxP;
    bool telescope = CVarGetInteger(CVAR_TELESCOPE_STARS, 0) != 0;
    f32 tint = (CVarGetInteger(CVAR_STAR_TWINKLE, 0) != 0) ? StarTwinkle(index) : 1.0f;

    if (telescope) {
        // Telescope stars cost far more display list and matrix space than vanilla ones, so stop before
        // running out of polyOpa buffer instead of capping the star count up front
        if (((u8*)play->state.gfxCtx->polyOpa.d - (u8*)gfx) < STAR_MIN_FREE_BYTES) {
            return SKYBOX_STAR_DRAW_STOP;
        }

        if (index == 0) {
            // RGB from the per star primitive color, alpha from the vertex color that holds the shape
            gDPSetCombineLERP(gfx++, 0, 0, 0, PRIMITIVE, SHADE, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, SHADE, 0,
                              PRIMITIVE, 0);
        }

        // The telescope combine mode drops the environment color, so fold the dawn and dusk fade in here
        gDPSetPrimColor(gfx++, 0, 0, color->r, color->g, color->b,
                        (u8)CLAMP((f32)color->a * fade * tint, 0.0f, 255.0f));
        *gfxP = gfx;

        DrawTelescopeStar(gfxP, play, index, pos, width);

        return SKYBOX_STAR_DRAW_OVERRIDE;
    }

    if (tint != 1.0f) {
        gDPSetPrimColor(gfx++, 0, 0, color->r, color->g, color->b, (u8)CLAMP((f32)color->a * tint, 0.0f, 255.0f));
        *gfxP = gfx;

        return SKYBOX_STAR_DRAW_TINTED;
    }

    return SKYBOX_STAR_DRAW_VANILLA;
}
