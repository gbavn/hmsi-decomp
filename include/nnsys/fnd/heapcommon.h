#ifndef NNSYS_FND_HEAPCOMMON_H
#define NNSYS_FND_HEAPCOMMON_H

#include "types.h"
#include "nnsys/fnd/list.h"

#define NNS_FND_HEAP_OPT_0_CLEAR (1 << 0)
#define NNS_FND_HEAP_OPT_DEBUG_FILL (1 << 1)

typedef struct NNSiFndHeapHead NNSiFndHeapHead;

struct NNSiFndHeapHead {
    u32 signature;          // 0x00
    NNSFndLink link;        // 0x04
    NNSFndList childList;   // 0x0c
    void *heapStart;        // 0x18
    void *heapEnd;          // 0x1c
    u32 attribute;          // 0x20
};

typedef NNSiFndHeapHead *NNSFndHeapHandle;

#define NNSi_FndGetBitValue(data, st, bits) (((data) >> (st)) & ((1 << (bits)) - 1))
#define NNSi_FndSetBitValue(data, st, bits, val)                                   \
    do {                                                                           \
        u32 maskBits = (u32)((1 << (bits)) - 1);                                   \
        u32 newVal = (val) & maskBits;                                             \
        (void)(maskBits <<= (st));                                                 \
        (data) &= ~maskBits;                                                       \
        (data) |= newVal << (st);                                                  \
    } while (FALSE)

#define NNSi_FndGetHeapOpt(pHeapHd) ((u16)NNSi_FndGetBitValue((pHeapHd)->attribute, 0, 8))

void NNSi_FndInitHeapHead(NNSiFndHeapHead *pHeapHd, u32 signature, void *heapStart, void *heapEnd, u16 optFlag);
void NNSi_FndFinalizeHeap(NNSiFndHeapHead *pHeapHd);

#endif
