#ifndef GRAPHICS_H
#define GRAPHICS_H

void MotionBlur_RenderMenuOptions();

#ifdef __cplusplus

extern "C" {
#endif

void MotionBlur_Override(u8* status, s32* alpha);

// Outcome of the skybox star hook, telling the caller whether it still owns the draw
typedef enum SkyboxStarDraw {
    /* 0 */ SKYBOX_STAR_DRAW_VANILLA,  // draw it the vanilla way, the prim color still has to be sent
    /* 1 */ SKYBOX_STAR_DRAW_TINTED,   // draw it the vanilla way, the prim color was already sent
    /* 2 */ SKYBOX_STAR_DRAW_OVERRIDE, // the star was already drawn
    /* 3 */ SKYBOX_STAR_DRAW_STOP      // out of display list budget, stop drawing stars
} SkyboxStarDraw;

s32 SkyboxStars_GetCount(s32 vanillaCount);
SkyboxStarDraw SkyboxStars_PrepareStar(Gfx** gfxP, PlayState* play, s32 index, Vec3f* pos, u32 width,
                                       Color_RGBA8* color, f32 fade);

#ifdef __cplusplus
};
#endif

#endif // GRAPHICS_H
