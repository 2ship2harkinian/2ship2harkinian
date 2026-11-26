#pragma once

#include "include/z64math.h"

#ifdef __cplusplus

#include <unordered_map>

std::unordered_map<Mtx*, MtxF> FrameInterpolation_Interpolate(float step);

extern "C" {

#endif

void FrameInterpolation_ShouldInterpolateFrame(bool shouldInterpolate);

void FrameInterpolation_StartRecord(void);

void FrameInterpolation_StopRecord(void);

void FrameInterpolation_RecordOpenChild(const void* a, int b);

void FrameInterpolation_RecordCloseChild(void);

void FrameInterpolation_DontInterpolateCamera(void);

int FrameInterpolation_GetCameraEpoch(void);

void FrameInterpolation_IgnoreActorMtx(void);

void FrameInterpolation_InterpolateWiderAngles();

void FrameInterpolation_RecordActorPosRotMatrix(void);

void FrameInterpolation_RecordMatrixPush(void);

void FrameInterpolation_RecordMatrixPop(void);

void FrameInterpolation_RecordMatrixPut(MtxF* src);

void FrameInterpolation_RecordMatrixMult(MtxF* mf, u8 mode);

void FrameInterpolation_RecordMatrixTranslate(f32 x, f32 y, f32 z, u8 mode);

void FrameInterpolation_RecordMatrixScale(f32 x, f32 y, f32 z, u8 mode);

void FrameInterpolation_RecordMatrixRotate1Coord(u32 coord, f32 value, u8 mode);

void FrameInterpolation_RecordMatrixRotateZYX(s16 x, s16 y, s16 z, u8 mode);

void FrameInterpolation_RecordMatrixTranslateRotateZYX(Vec3f* translation, Vec3s* rotation);

void FrameInterpolation_RecordMatrixSetTranslateRotateYXZ(f32 translateX, f32 translateY, f32 translateZ, Vec3s* rot);

void FrameInterpolation_RecordMatrixMtxFToMtx(MtxF* src, Mtx* dest);

void FrameInterpolation_RecordMatrixToMtx(Mtx* dest, char* file, s32 line);

void FrameInterpolation_RecordMatrixReplaceRotation(MtxF* mf);

void FrameInterpolation_RecordMatrixRotateAxis(f32 angle, Vec3f* axis, u8 mode);

void FrameInterpolation_RecordSkinMatrixMtxFToMtx(MtxF* src, Mtx* dest);

// SKIN_LIMB_TYPE_ANIMATED limbs have no matrix to interpolate, so their vertex poses are blended.
void FrameInterpolation_RecordSkinnedLimb(void* key, void* dest, void* prev, u32 vtxCount);

void FrameInterpolation_UpdateSkinnedVertices(f32 step);

// `pairs` holds `pairCount` (destination, source) vertex indices: each destination collapses
// onto its source at the start of the logical frame. Other vertices are left as the game wrote them.
void FrameInterpolation_RecordRibbonHead(void* key, int index, void* dest, u32 vtxCount, u32 pairCount,
                                         const s16* pairs);

void FrameInterpolation_UpdateRibbonHeads(f32 step);

#ifdef __cplusplus
}
#endif
