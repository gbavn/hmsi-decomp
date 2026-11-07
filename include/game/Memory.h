#ifndef MEMORY_H
#define MEMORY_H

#include <nitro/types.h>

// Global game heap (Memory.cpp). C++ linkage.
void Memory_Init(void);
void *Memory_Alloc(u32 size);
void Memory_Free(void *ptr);

#endif
