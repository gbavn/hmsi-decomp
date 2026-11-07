#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include "types.h"

// Every block (used or free) starts with this header; user data follows it.
typedef struct MMBlock {
    u32 size;             // whole block, header and guard word included
    struct MMBlock *next;
    const char *tag;      // "file(line)" of the allocation
    u32 id;               // allocation counter
} MMBlock;

typedef struct MMHeap {
    u32 tailSize;         // bytes still untouched after the header (allocated from the end)
    MMBlock *usedList;
    MMBlock *freeList;
    u32 usedCount;
    u32 param;
    int error;
} MMHeap;

enum {
    MM_OK = 0,
    MM_ERROR_NULL_HEAP = 1,
    MM_ERROR_TOO_SMALL = 2,
    MM_ERROR_NO_SPACE = 2,
    MM_ERROR_NULL_PTR = 3,
    MM_ERROR_NOT_FOUND = 4
};

int MM_InitHeap(void *mem, u32 size, u32 param);
void *MM_Alloc(void *mem, u32 size, const char *tag);
int MM_Free(void *mem, void *ptr, const char *tag);
u32 MM_GetTailSize(void *mem);
int MM_GetFreeBlockCount(void *mem);
u32 MM_GetFreeBlockTotal(void *mem);

MMBlock *MMi_AllocBlock(MMHeap *heap, u32 size);
MMBlock *MMi_TakeFreeBlock(MMHeap *heap, u32 size);
MMBlock *MMi_TakeTail(MMHeap *heap, u32 size);
void MMi_Unlink(MMBlock *prev, MMBlock *block);
MMBlock *MMi_Coalesce(MMHeap *heap, MMBlock *block);
int MMi_CountBlocks(MMBlock *block);
u32 MMi_SumBlockSizes(MMBlock *block);
int MMi_FreeBlock(MMHeap *heap, void *ptr, const char *tag);

#endif
