#ifndef NITRO_MI_MEMORY_H
#define NITRO_MI_MEMORY_H

#include "types.h"

void MIi_CpuClear16(u16 data, void *destp, u32 size);
void MIi_CpuCopy16(const void *srcp, void *destp, u32 size);
void MIi_CpuClear32(u32 data, void *destp, u32 size);

#define MI_CpuFill16(dest, data, size) MIi_CpuClear16((data), (dest), (size))
#define MI_CpuClear16(dest, size) MIi_CpuClear16(0, (dest), (size))
#define MI_CpuCopy16(src, dest, size) MIi_CpuCopy16((src), (dest), (size))
#define MI_CpuFill32(dest, data, size) MIi_CpuClear32((data), (dest), (size))
#define MI_CpuClear32(dest, size) MIi_CpuClear32(0, (dest), (size))

void MI_DmaFill32(u32 dmaNo, void *dest, u32 data, u32 size);

#endif
