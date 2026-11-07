#include "nnsys/fnd/frameheap.h"
#include "nitro/mi/memory.h"

#define NNSI_FRMHEAP_SIGNATURE ('FRMH')
#define MIN_ALIGNMENT 4

#define RoundUp(value, align) ((((align) - 1) + (value)) & ~((align) - 1))
#define RoundDown(value, align) ((value) & ~((align) - 1))
#define RoundUpPtr(ptr, align) ((void *)RoundUp((u32)(ptr), (align)))
#define RoundDownPtr(ptr, align) ((void *)RoundDown((u32)(ptr), (align)))

static inline void *AddU32ToPtr(void *ptr, u32 val) {
    return (void *)(val + (u32)ptr);
}

static inline void *SubU32ToPtr(void *ptr, u32 val) {
    return (void *)((u32)ptr - val);
}

static inline u32 GetOffsetFromPtr(void *start, void *end) {
    return (u32)end - (u32)start;
}

static inline void FillAllocMemory(NNSiFndHeapHead *pHeapHd, void *address, u32 size) {
    if (NNSi_FndGetHeapOpt(pHeapHd) & NNS_FND_HEAP_OPT_0_CLEAR) {
        MI_CpuFill32(address, 0, size);
    }
}

#define GetFrmHeapHeadPtrFromHeapHead(pHHead) ((NNSiFndFrmHeapHead *)((u32)(pHHead) + sizeof(NNSiFndHeapHead)))
#define GetHeapHeadPtrFromFrmHeapHead(pFHHead) ((NNSiFndHeapHead *)((u32)(pFHHead) - sizeof(NNSiFndHeapHead)))

static NNSiFndHeapHead *InitFrameHeap(void *startAddress, void *endAddress, u16 optFlag) {
    NNSiFndHeapHead *pHeapHd = startAddress;
    NNSiFndFrmHeapHead *pFrmHeapHd = (NNSiFndFrmHeapHead *)((u32)pHeapHd + sizeof(NNSiFndHeapHead));

    NNSi_FndInitHeapHead(pHeapHd, NNSI_FRMHEAP_SIGNATURE,
                         (void *)((u32)pFrmHeapHd + sizeof(NNSiFndFrmHeapHead)), endAddress, optFlag);

    pFrmHeapHd->headAllocator = pHeapHd->heapStart;
    pFrmHeapHd->tailAllocator = pHeapHd->heapEnd;
    pFrmHeapHd->pState = NULL;
    return pHeapHd;
}

static void *AllocFromHead(NNSiFndFrmHeapHead *pFrmHeapHd, u32 size, int alignment) {
    void *newBlock = RoundUpPtr(pFrmHeapHd->headAllocator, alignment);
    void *endAddress = AddU32ToPtr(newBlock, size);

    if ((u32)endAddress > (u32)pFrmHeapHd->tailAllocator) {
        return NULL;
    }

    FillAllocMemory(GetHeapHeadPtrFromFrmHeapHead(pFrmHeapHd), pFrmHeapHd->headAllocator,
                    GetOffsetFromPtr(pFrmHeapHd->headAllocator, endAddress));

    pFrmHeapHd->headAllocator = endAddress;
    return newBlock;
}

static void *AllocFromTail(NNSiFndFrmHeapHead *pFrmHeapHd, u32 size, int alignment) {
    void *newBlock = RoundDownPtr(SubU32ToPtr(pFrmHeapHd->tailAllocator, size), alignment);

    if ((u32)newBlock < (u32)pFrmHeapHd->headAllocator) {
        return NULL;
    }

    FillAllocMemory(GetHeapHeadPtrFromFrmHeapHead(pFrmHeapHd), newBlock,
                    GetOffsetFromPtr(newBlock, pFrmHeapHd->tailAllocator));

    pFrmHeapHd->tailAllocator = newBlock;
    return newBlock;
}

static void FreeHeadBlock(NNSiFndHeapHead *pHeapHd) {
    NNSiFndFrmHeapHead *pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(pHeapHd);

    pFrmHeapHd->headAllocator = pHeapHd->heapStart;
    pFrmHeapHd->pState = NULL;
}

static void FreeTailBlock(NNSiFndHeapHead *pHeapHd) {
    NNSiFndFrmHeapHead *pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(pHeapHd);
    NNSiFndFrmHeapState *pState;

    for (pState = pFrmHeapHd->pState; pState; pState = pState->pPrevState) {
        pState->tailAllocator = pHeapHd->heapEnd;
    }
    pFrmHeapHd->tailAllocator = pHeapHd->heapEnd;
}

NNSFndHeapHandle NNS_FndCreateFrmHeapEx(void *startAddress, u32 size, u16 optFlag) {
    void *endAddress = RoundDownPtr(AddU32ToPtr(startAddress, size), MIN_ALIGNMENT);
    startAddress = RoundUpPtr(startAddress, MIN_ALIGNMENT);

    if ((u32)startAddress > (u32)endAddress ||
        GetOffsetFromPtr(startAddress, endAddress) < sizeof(NNSiFndHeapHead) + sizeof(NNSiFndFrmHeapHead)) {
        return NULL;
    }
    return InitFrameHeap(startAddress, endAddress, optFlag);
}

void NNS_FndDestroyFrmHeap(NNSFndHeapHandle heap) {
    NNSi_FndFinalizeHeap(heap);
}

void *NNS_FndAllocFromFrmHeapEx(NNSFndHeapHandle heap, u32 size, int alignment) {
    void *memory = NULL;
    NNSiFndFrmHeapHead *pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(heap);

    if (size == 0) {
        size = 1;
    }
    size = RoundUp(size, MIN_ALIGNMENT);

    if (alignment >= 0) {
        memory = AllocFromHead(pFrmHeapHd, size, alignment);
    } else {
        memory = AllocFromTail(pFrmHeapHd, size, -alignment);
    }
    return memory;
}

void NNS_FndFreeToFrmHeap(NNSFndHeapHandle heap, int mode) {
    if (mode & NNS_FND_FRMHEAP_FREE_HEAD) {
        FreeHeadBlock(heap);
    }
    if (mode & NNS_FND_FRMHEAP_FREE_TAIL) {
        FreeTailBlock(heap);
    }
}

BOOL NNS_FndRecordStateForFrmHeap(NNSFndHeapHandle heap, u32 tagName) {
    NNSiFndFrmHeapHead *pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(heap);
    void *oldHeadAllocator = pFrmHeapHd->headAllocator;
    NNSiFndFrmHeapState *pState = AllocFromHead(pFrmHeapHd, sizeof(NNSiFndFrmHeapState), MIN_ALIGNMENT);

    if (!pState) {
        return FALSE;
    }

    pState->tagName = tagName;
    pState->headAllocator = oldHeadAllocator;
    pState->tailAllocator = pFrmHeapHd->tailAllocator;
    pState->pPrevState = pFrmHeapHd->pState;

    pFrmHeapHd->pState = pState;
    return TRUE;
}

BOOL NNS_FndFreeByStateToFrmHeap(NNSFndHeapHandle heap, u32 tagName) {
    NNSiFndFrmHeapHead *pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(heap);
    NNSiFndFrmHeapState *pState = pFrmHeapHd->pState;

    if (tagName != 0) {
        for (; pState; pState = pState->pPrevState) {
            if (pState->tagName == tagName) {
                break;
            }
        }
    }

    if (!pState) {
        return FALSE;
    }

    pFrmHeapHd->headAllocator = pState->headAllocator;
    pFrmHeapHd->tailAllocator = pState->tailAllocator;
    pFrmHeapHd->pState = pState->pPrevState;
    return TRUE;
}
