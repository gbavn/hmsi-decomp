// memory_manager.c: the game's simple heap allocator (file name taken from the
// "memory_manager.c(476)" tag string it stores in free blocks).
//
// A heap lives in a caller-provided buffer. Its header starts at the first word boundary
// strictly after the buffer address (1-4 bytes in). Allocations are carved from the end of
// the remaining space or reused (best fit) from the free list; every block starts with a
// 0x10-byte header and ends with a 0xFEEBDAED guard word.
#include <nitro/os.h>
#include "memory_manager.h"

#define MM_HEAP(mem) ((MMHeap *)((u8 *)(mem) + (4 - ((u32)(mem) & 3))))
#define MM_GUARD 0xFEEBDAED

u32 sMMAllocCount;

int MM_InitHeap(void *mem, u32 size, u32 param)
{
    u32 pad;
    MMHeap *heap;

    if (mem == NULL) {
        return MM_ERROR_NULL_HEAP;
    }
    pad = 4 - ((u32)mem & 3);
    size -= pad;
    if (size < sizeof(MMHeap)) {
        return MM_ERROR_TOO_SMALL;
    }
    size -= sizeof(MMHeap);
    *(u32 *)((u8 *)mem + pad) = size & ~3;
    heap = (MMHeap *)((u8 *)mem + pad);
    heap->usedList = NULL;
    heap->freeList = NULL;
    heap->usedCount = 0;
    heap->param = param;
    heap->error = MM_OK;
    return MM_OK;
}

void *MM_Alloc(void *mem, u32 size, const char *tag)
{
    MMBlock *block = MMi_AllocBlock(MM_HEAP(mem), size);

    if (block != NULL) {
        block->tag = tag;
        return block + 1;
    }
    return NULL;
}

int MM_Free(void *mem, void *ptr, const char *tag)
{
    return MMi_FreeBlock(MM_HEAP(mem), ptr, tag);
}

MMBlock *MMi_AllocBlock(MMHeap *heap, u32 size)
{
    OSIntrMode enabled = OS_DisableInterrupts();

    if (heap != NULL) {
        u32 aligned = (size + 3) & ~3;
        MMBlock *block = MMi_TakeFreeBlock(heap, aligned + sizeof(MMBlock) + sizeof(u32));

        if (block == NULL) {
            block = MMi_TakeTail(heap, aligned + sizeof(MMBlock) + sizeof(u32));
        }
        if (block != NULL) {
            block->id = sMMAllocCount++;
            block->next = NULL;
            *(u32 *)((u8 *)block + block->size - 4) = MM_GUARD;
            block->next = heap->usedList;
            heap->usedList = block;
            heap->error = MM_OK;
            (void)OS_RestoreInterrupts(enabled);
            heap->usedCount++;
            return block;
        }
        heap->error = MM_ERROR_NO_SPACE;
    }
    (void)OS_RestoreInterrupts(enabled);
    return NULL;
}

MMBlock *MMi_TakeFreeBlock(MMHeap *heap, u32 size)
{
    MMBlock *best = NULL;
    MMBlock *bestPrev = NULL;
    MMBlock *block;
    MMBlock *prev = NULL;

    for (block = heap->freeList; block != NULL; prev = block, block = block->next) {
        if (block->size >= size) {
            if (best != NULL) {
                if (best->size > block->size) {
                    best = block;
                    bestPrev = prev;
                }
            } else {
                best = block;
                bestPrev = prev;
            }
        }
    }

    if (best != NULL) {
        if (bestPrev == NULL) {
            heap->freeList = best->next;
        } else {
            MMi_Unlink(bestPrev, best);
        }
        best->next = NULL;
        if (best->size >= size + sizeof(MMBlock)) {
            MMBlock *rest = (MMBlock *)((u8 *)best + size);

            rest->next = NULL;
            rest->size = best->size - size;
            rest->tag = "memory_manager.c(476)";
            rest->next = heap->freeList;
            heap->freeList = rest;
            best->size = size;
        }
    }
    return best;
}

MMBlock *MMi_TakeTail(MMHeap *heap, u32 size)
{
    MMBlock *block;

    if (heap->tailSize < size) {
        return NULL;
    }
    heap->tailSize -= size;
    block = (MMBlock *)((u8 *)(heap + 1) + heap->tailSize);
    block->size = size;
    return block;
}

void MMi_Unlink(MMBlock *prev, MMBlock *block)
{
    prev->next = block->next;
    block->next = NULL;
}

MMBlock *MMi_Coalesce(MMHeap *heap, MMBlock *block)
{
    MMBlock *prev = NULL;
    MMBlock *end = (MMBlock *)((u8 *)block + block->size);
    MMBlock *it;

    for (it = heap->freeList; it != NULL; prev = it, it = it->next) {
        if (it == end) {
            if (prev == NULL) {
                heap->freeList = it->next;
            } else {
                MMi_Unlink(prev, it);
            }
            it->next = NULL;
            block->size += it->size;
            break;
        }
    }

    for (it = heap->freeList; it != NULL; it = it->next) {
        if ((MMBlock *)((u8 *)it + it->size) == block) {
            it->size += block->size;
            return NULL;
        }
    }
    return block;
}

int MMi_CountBlocks(MMBlock *block)
{
    int count = 0;

    for (; block != NULL; block = block->next) {
        count++;
    }
    return count;
}

u32 MMi_SumBlockSizes(MMBlock *block)
{
    u32 total = 0;

    for (; block != NULL; block = block->next) {
        total += block->size;
    }
    return total;
}

int MMi_FreeBlock(MMHeap *heap, void *ptr, const char *tag)
{
    OSIntrMode enabled;
    MMBlock *prev;
    MMBlock *block;

    if (heap != NULL) {
        enabled = OS_DisableInterrupts();
        if (ptr != NULL) {
            prev = NULL;
            for (block = heap->usedList; block != NULL; prev = block, block = block->next) {
                if ((void *)(block + 1) == ptr) {
                    MMBlock *merged;

                    if (prev == NULL) {
                        heap->usedList = block->next;
                    } else {
                        MMi_Unlink(prev, block);
                    }
                    block->next = NULL;
                    merged = MMi_Coalesce(heap, block);
                    if (merged != NULL) {
                        if ((MMBlock *)((u8 *)(heap + 1) + heap->tailSize) == merged) {
                            heap->tailSize += merged->size;
                        } else {
                            merged->tag = tag;
                            merged->next = heap->freeList;
                            heap->freeList = merged;
                        }
                    }
                    (void)OS_RestoreInterrupts(enabled);
                    heap->usedCount--;
                    return heap->error = MM_OK;
                }
            }
            (void)OS_RestoreInterrupts(enabled);
            return heap->error = MM_ERROR_NOT_FOUND;
        }
        (void)OS_RestoreInterrupts(enabled);
        return heap->error = MM_ERROR_NULL_PTR;
    }
    return MM_ERROR_NULL_HEAP;
}

u32 MM_GetTailSize(void *mem)
{
    return MM_HEAP(mem)->tailSize;
}

int MM_GetFreeBlockCount(void *mem)
{
    return MMi_CountBlocks(MM_HEAP(mem)->freeList);
}

u32 MM_GetFreeBlockTotal(void *mem)
{
    return MMi_SumBlockSizes(MM_HEAP(mem)->freeList);
}
