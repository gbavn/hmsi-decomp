#ifndef NITRO_FX_FX_INLINE_H
#define NITRO_FX_FX_INLINE_H

#include "nitro/fx/fx.h"

static inline fx32 FX_Mul(fx32 v1, fx32 v2) {
    return (fx32)(((fx64)v1 * v2 + 0x800LL) >> FX32_SHIFT);
}

#endif
