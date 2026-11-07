#ifndef NITRO_MI_MEMORY_H
#define NITRO_MI_MEMORY_H

#include "types.h"

void MIi_CpuClear32(u32 data, void *destp, u32 size);

#define MI_CpuFill32(dest, data, size) MIi_CpuClear32((data), (dest), (size))
#define MI_CpuClear32(dest, size) MIi_CpuClear32(0, (dest), (size))

#endif
