#include "nnsys/fnd/heapcommon.h"
#include "nitro/mi/memory.h"

// mwcc emits .bss in reverse declaration order
static NNSFndList sRootList;
BOOL sRootListInitialized; // global: BuildInfo references this address (start of .bss)

static NNSiFndHeapHead *FindContainHeap(NNSFndList *pList, const void *memBlock) {
    NNSiFndHeapHead *pHeapHd = NULL;

    while ((pHeapHd = NNS_FndGetNextListObject(pList, pHeapHd)) != NULL) {
        if ((u32)pHeapHd->heapStart <= (u32)memBlock && (u32)memBlock < (u32)pHeapHd->heapEnd) {
            NNSiFndHeapHead *pChildHeapHd = FindContainHeap(&pHeapHd->childList, memBlock);
            if (pChildHeapHd) {
                return pChildHeapHd;
            }
            return pHeapHd;
        }
    }
    return NULL;
}

static NNSFndList *FindListContainHeap(NNSiFndHeapHead *pHeapHd) {
    NNSFndList *pList = &sRootList;
    NNSiFndHeapHead *pContainHeap = FindContainHeap(&sRootList, pHeapHd);

    if (pContainHeap) {
        pList = &pContainHeap->childList;
    }
    return pList;
}

void NNSi_FndInitHeapHead(NNSiFndHeapHead *pHeapHd, u32 signature, void *heapStart, void *heapEnd, u16 optFlag) {
    pHeapHd->signature = signature;
    pHeapHd->heapStart = heapStart;
    pHeapHd->heapEnd = heapEnd;
    pHeapHd->attribute = 0;
    NNSi_FndSetBitValue(pHeapHd->attribute, 0, 8, optFlag);

    NNS_FndInitList(&pHeapHd->childList, 4);

    if (!sRootListInitialized) {
        NNS_FndInitList(&sRootList, 4);
        sRootListInitialized = TRUE;
    }
    NNS_FndAppendListObject(FindListContainHeap(pHeapHd), pHeapHd);
}

void NNSi_FndFinalizeHeap(NNSiFndHeapHead *pHeapHd) {
    NNS_FndRemoveListObject(FindListContainHeap(pHeapHd), pHeapHd);
}
