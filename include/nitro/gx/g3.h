#ifndef NITRO_GX_G3_H
#define NITRO_GX_G3_H

#include "nitro/fx/fx.h"
#include "nitro/gx/gx_vramcnt.h"

#define reg_G3_MTX_MODE (*(vu32 *)0x04000440)
#define reg_G3_MTX_PUSH (*(vu32 *)0x04000444)
#define reg_G3_MTX_POP (*(vu32 *)0x04000448)
#define reg_G3_MTX_STORE (*(vu32 *)0x0400044c)
#define reg_G3_VIEWPORT (*(vu32 *)0x04000580)
#define reg_G3_MTX_SCALE (*(vu32 *)0x0400046c)
#define reg_G3_MTX_TRANS (*(vu32 *)0x04000470)
#define reg_G3_TEXCOORD (*(vu32 *)0x04000488)
#define reg_G3_VTX_10 (*(vu32 *)0x04000490)
#define reg_G3_TEXIMAGE_PARAM (*(vu32 *)0x040004a8)
#define reg_G3_TEXPLTT_BASE (*(vu32 *)0x040004ac)
#define reg_G3_BEGIN_VTXS (*(vu32 *)0x04000500)
#define reg_G3_END_VTXS (*(vu32 *)0x04000504)

#define FX32_DEC_SIZE 12
#define GX_VTX10_DEC_SIZE 6
#define GX_VTX10_MASK 0x03ff
#define GX_VECVTX10(x, y, z)                                                                   ((u32)(((((x) >> (FX32_DEC_SIZE - GX_VTX10_DEC_SIZE)) & GX_VTX10_MASK) << 0) |                     ((((y) >> (FX32_DEC_SIZE - GX_VTX10_DEC_SIZE)) & GX_VTX10_MASK) << 10) |                    ((((z) >> (FX32_DEC_SIZE - GX_VTX10_DEC_SIZE)) & GX_VTX10_MASK) << 20)))
#define GX_FX16ST(x) ((s16)((x) >> 8))
#define GX_ST(s, t) ((u32)((u16)GX_FX16ST(s) | ((u16)GX_FX16ST(t) << 16)))

#define GX_PACK_TEXIMAGE_PARAM(texFmt, texGen, s, t, repeat, flip, pltt0, addr)                   ((u32)(((addr) >> 3) | ((texFmt) << 26) | ((texGen) << 30) | ((s) << 20) | ((t) << 23) |              ((repeat) << 16) | ((flip) << 18) | ((pltt0) << 29)))
#define GX_PACK_TEXPLTTBASE_PARAM(addr, texFmt) ((u32)((addr) >> (4 - ((texFmt) == GX_TEXFMT_PLTT4))))

#define GX_TEXGEN_TEXCOORD 1
#define GX_TEXREPEAT_NONE 0
#define GX_TEXFLIP_NONE 0

typedef enum {
    GX_BEGIN_TRIANGLES = 0,
    GX_BEGIN_QUADS = 1,
    GX_BEGIN_TRIANGLE_STRIP = 2,
    GX_BEGIN_QUAD_STRIP = 3
} GXBegin;

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

static inline void G3_Scale(fx32 x, fx32 y, fx32 z) {
    reg_G3_MTX_SCALE = (u32)x;
    reg_G3_MTX_SCALE = (u32)y;
    reg_G3_MTX_SCALE = (u32)z;
}

static inline void G3_Translate(fx32 x, fx32 y, fx32 z) {
    reg_G3_MTX_TRANS = (u32)x;
    reg_G3_MTX_TRANS = (u32)y;
    reg_G3_MTX_TRANS = (u32)z;
}

static inline void G3_TexCoord(fx32 s, fx32 t) {
    reg_G3_TEXCOORD = GX_ST(s, t);
}

static inline void G3_Vtx10(fx16 x, fx16 y, fx16 z) {
    reg_G3_VTX_10 = GX_VECVTX10(x, y, z);
}

static inline void G3_Begin(GXBegin primitive) {
    reg_G3_BEGIN_VTXS = (u32)primitive;
}

static inline void G3_End(void) {
    reg_G3_END_VTXS = 0;
}

void G3_LoadMtx43(const MtxFx43 *m);

static inline void G3_PushMtx(void) {
    reg_G3_MTX_PUSH = 0;
}

static inline void G3_PopMtx(int num) {
    reg_G3_MTX_POP = (u32)num;
}

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

static inline void G3_TexPlttBase(u32 addr, GXTexFmt texfmt) {
    u32 param = GX_PACK_TEXPLTTBASE_PARAM(addr, texfmt);
    reg_G3_TEXPLTT_BASE = param;
}

#endif
