#ifndef NITRO_GX_GX_OAM_H
#define NITRO_GX_GX_OAM_H

#include "nitro/fx/fx.h"

#define HW_OAM 0x07000000
#define HW_DB_OAM 0x07000400

typedef struct {
    union {
        u32 attr01;
        struct {
            u16 attr0;
            u16 attr1;
        };
        struct {
            u32 y : 8;
            u32 rsMode : 2;
            u32 objMode : 2;
            u32 mosaic : 1;
            u32 colorMode : 1;
            u32 shape : 2;

            u32 x : 9;
            u32 rsParam : 5;
            u32 size : 2;
        };
        struct {
            u32 _0 : 28;
            u32 flipH : 1;
            u32 flipV : 1;
            u32 _1 : 2;
        };
    };
    union {
        struct {
            u16 attr2;
            u16 _3;
        };
        u32 attr2_3;
        struct {
            u32 charNo : 10;
            u32 priority : 2;
            u32 cParam : 4;
            u32 _2 : 16;
        };
    };
} GXOamAttr;

typedef struct {
    u16 _0;
    u16 _1;
    u16 _2;
    s16 PA;
    u16 _4;
    u16 _5;
    u16 _6;
    s16 PB;
    u16 _8;
    u16 _9;
    u16 _10;
    s16 PC;
    u16 _12;
    u16 _13;
    u16 _14;
    s16 PD;
} GXOamAffine;

#define GX_OAM_ATTR01_RSENABLE_MASK 0x00000300
#define GX_OAM_ATTR01_FLIP_MASK 0x30000000
#define GX_OAM_ATTR01_SHAPE_MASK 0x0000c000
#define GX_OAM_ATTR01_SHAPE_SHIFT 14
#define GX_OAM_ATTR01_SIZE_MASK 0xc0000000
#define GX_OAM_ATTR01_SIZE_SHIFT 30

typedef enum {
    GX_OAM_EFFECT_NONE = 0x00000000,
    GX_OAM_EFFECT_FLIP_H = 0x10000000,
    GX_OAM_EFFECT_FLIP_V = 0x20000000,
    GX_OAM_EFFECT_FLIP_HV = 0x30000000,
    GX_OAM_EFFECT_AFFINE = 0x00000100,
    GX_OAM_EFFECT_NODISPLAY = 0x00000200,
    GX_OAM_EFFECT_AFFINE_DOUBLE = 0x00000300
} GXOamEffect;

typedef int GXOamShape;

static inline GXOamEffect G2_GetOBJEffect(const GXOamAttr *oam) {
    GXOamEffect effect;

    effect = (GXOamEffect)(oam->attr01 & GX_OAM_ATTR01_RSENABLE_MASK);

    if (effect == GX_OAM_EFFECT_AFFINE || effect == GX_OAM_EFFECT_AFFINE_DOUBLE) {
        return effect;
    }

    return (GXOamEffect)(effect | (oam->attr01 & GX_OAM_ATTR01_FLIP_MASK));
}

static inline void G2_SetOBJAffine(GXOamAffine *oam, const MtxFx22 *mtx) {
    oam->PA = (s16)(mtx->_00 >> 4);
    oam->PB = (s16)(mtx->_01 >> 4);
    oam->PC = (s16)(mtx->_10 >> 4);
    oam->PD = (s16)(mtx->_11 >> 4);
}

void GX_LoadOAM(const void *pSrc, u32 offset, u32 szByte);
void GXS_LoadOAM(const void *pSrc, u32 offset, u32 szByte);

#endif
