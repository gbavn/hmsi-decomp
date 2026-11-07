// Memory.cpp: global heap on the main arena and the C++ allocation operators.
// The file name comes from the "Memory.cpp(42)" / "Memory.cpp(106)" tag strings.
#include <nitro/os.h>
#include "memory_manager.h"
#include "Memory.h"

static void *sHeap;

void Memory_Init(void)
{
    u32 hi;

    sHeap = OS_GetArenaLo(OS_ARENA_MAIN);
    (void)OS_GetArenaLo(OS_ARENA_MAIN);
    (void)OS_GetArenaHi(OS_ARENA_MAIN);
    (void)OS_GetArenaLo(OS_ARENA_MAINEX);
    (void)OS_GetArenaHi(OS_ARENA_MAINEX);
    hi = (u32)OS_GetArenaHi(OS_ARENA_MAIN);
    (void)MM_InitHeap(sHeap, hi - (u32)sHeap, "Memory.cpp(42)");
}

void *Memory_Alloc(u32 size)
{
    return MM_Alloc(sHeap, size, NULL);
}

void Memory_Free(void *ptr)
{
    if (ptr != NULL) {
        (void)MM_Free(sHeap, ptr, "Memory.cpp(106)");
    }
}

void *operator new(size_t size)
{
    return Memory_Alloc(size);
}

void *operator new[](size_t size)
{
    return operator new(size);
}

void operator delete(void *ptr)
{
    Memory_Free(ptr);
}

void operator delete[](void *ptr)
{
    operator delete(ptr);
}
