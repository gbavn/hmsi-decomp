#ifndef NITRO_OS_CACHE_H
#define NITRO_OS_CACHE_H

#include "types.h"

void DC_InvalidateRange(void *startAddr, u32 nBytes);
void DC_FlushRange(const void *startAddr, u32 nBytes);

#endif
