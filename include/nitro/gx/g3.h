#ifndef NITRO_GX_G3_H
#define NITRO_GX_G3_H

#include "nitro/fx/fx.h"

#define reg_G3_MTX_MODE (*(vu32 *)0x04000440)
#define reg_G3_MTX_STORE (*(vu32 *)0x0400044c)
#define reg_G3_VIEWPORT (*(vu32 *)0x04000580)

typedef enum {
    GX_MTXMODE_PROJECTION = 0,
    GX_MTXMODE_POSITION = 1,
    GX_MTXMODE_POSITION_VECTOR = 2,
    GX_MTXMODE_TEXTURE = 3
} GXMtxMode;

#define GX_PACK_VIEWPORT_PARAM(x1, y1, x2, y2) \
    ((u32)(x1) | ((u32)(y1) << 8) | ((u32)(x2) << 16) | ((u32)(y2) << 24))

void G3i_OrthoW_(fx32 t, fx32 b, fx32 l, fx32 r, fx32 n, fx32 f, fx32 scaleW, BOOL draw, MtxFx44 *mtx);
void G3i_LookAt_(const VecFx32 *camPos, const VecFx32 *camUp, const VecFx32 *target, BOOL draw, MtxFx43 *mtx);

static inline void G3_MtxMode(GXMtxMode mode) {
    reg_G3_MTX_MODE = (u32)mode;
}

static inline void G3_StoreMtx(int num) {
    reg_G3_MTX_STORE = (u32)num;
}

static inline void G3_ViewPort(int x1, int y1, int x2, int y2) {
    reg_G3_VIEWPORT = GX_PACK_VIEWPORT_PARAM(x1, y1, x2, y2);
}

static inline void G3_OrthoW(fx32 t, fx32 b, fx32 l, fx32 r, fx32 n, fx32 f, fx32 scaleW, MtxFx44 *mtx) {
    G3i_OrthoW_(t, b, l, r, n, f, scaleW, TRUE, mtx);
}

static inline void G3_LookAt(const VecFx32 *camPos, const VecFx32 *camUp, const VecFx32 *target, MtxFx43 *mtx) {
    G3i_LookAt_(camPos, camUp, target, TRUE, mtx);
}

#endif
