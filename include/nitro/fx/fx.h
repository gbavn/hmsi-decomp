#ifndef NITRO_FX_FX_H
#define NITRO_FX_FX_H

#include "types.h"

typedef s32 fx32;
typedef s16 fx16;
typedef s64 fx64;

#define FX32_SHIFT 12
#define FX32_ONE ((fx32)0x00001000L)

typedef struct {
    fx32 x;
    fx32 y;
    fx32 z;
} VecFx32;

typedef struct {
    fx16 x;
    fx16 y;
    fx16 z;
} VecFx16;

typedef struct {
    fx32 _00, _01, _10, _11;
} MtxFx22;

typedef struct {
    fx32 _00, _01, _02;
    fx32 _10, _11, _12;
    fx32 _20, _21, _22;
    fx32 _30, _31, _32;
} MtxFx43;

typedef struct {
    fx32 _00, _01, _02, _03;
    fx32 _10, _11, _12, _13;
    fx32 _20, _21, _22, _23;
    fx32 _30, _31, _32, _33;
} MtxFx44;

#endif
