#include "nnsys/g2d/g2d_OAM.h"
#include "nitro/mi/memory.h"
#include "nitro/os/cache.h"

extern u32 GXi_DmaId;

const u16 NNSi_objSizeHTbl[3][4] = {
    { 8, 16, 32, 64 },
    { 8, 8, 16, 32 },
    { 16, 32, 32, 64 },
};

const u16 NNSi_objSizeWTbl[3][4] = {
    { 8, 16, 32, 64 },
    { 16, 32, 32, 64 },
    { 8, 8, 16, 32 },
};

#define OAM_SETTING_INVISIBLE 192
#define NUM_HW_OAM_ATTR 128
#define NUM_HW_OAM_AFFINE 32
#define NUM_OAM_TYPES 3
#define OAM_NOT_USED 0xFFFF

#define GX_AFFINE_SIZE sizeof(GXOamAffine)
#define GX_OAMATTR_SIZE sizeof(GXOamAttr)
#define OAMATTR_SIZE sizeof(u16) * 3

#define GX_DMA_NOT_USE 0xffffffff

typedef void(OBJLoadFunction)(const void *pSrc, u32 offset, u32 szByte);

typedef struct OamAttributeCache {
    u16 reservationTable[NUM_HW_OAM_ATTR];
    GXOamAttr oamBuffer[NUM_HW_OAM_ATTR];
} OamAttributeCache;

typedef struct OamAffineCache {
    u16 reservationTable[NUM_HW_OAM_AFFINE];
} OamAffineCache;

typedef struct OamCache {
    OamAttributeCache attributeCache;
    OamAffineCache affineCache;
} OamCache;

static OamCache oamCache_[NUM_OAM_TYPES];
static u16 numRegisterdInstance_ = 0x0;

static inline void NNSi_G2dDmaFill32(u32 dmaNo, void *dest, u32 data, u32 size) {
    if (dmaNo != GX_DMA_NOT_USE) {
        MI_DmaFill32(dmaNo, dest, data, size);
    } else {
        MI_CpuFill32(dest, data, size);
    }
}

static inline BOOL IsManageAreaValid_(const NNSG2dOAMManageArea *pArea) {
    return (BOOL)((pArea->currentIdx <= pArea->toIdx + 1) && (pArea->fromIdx <= pArea->toIdx));
}

static inline OamCache *GetOamCachePtr_(NNSG2dOamType type) {
    return &oamCache_[type];
}

static inline u16 *GetOamReservationTblPtr_(NNSG2dOamType type) {
    OamCache *pOamCache = GetOamCachePtr_(type);
    return pOamCache->attributeCache.reservationTable;
}

static inline u16 *GetAffineReservationTblPtr_(NNSG2dOamType type) {
    OamCache *pOamCache = GetOamCachePtr_(type);
    return pOamCache->affineCache.reservationTable;
}

static inline GXOamAttr *GetOamBufferPtr_(NNSG2dOamType type, u16 index) {
    OamCache *pOamCache = GetOamCachePtr_(type);
    return pOamCache->attributeCache.oamBuffer + index;
}

static inline GXOamAffine *GetAffineBufferPtr_(NNSG2dOamType type, u16 index) {
    GXOamAffine *pAff = (GXOamAffine *)GetOamBufferPtr_(type, 0);
    return pAff + index;
}

static inline BOOL IsOamNotUsed_(u16 *pResvTblHead, u16 from, u16 to) {
    const u16 *pCursor = pResvTblHead + from;
    const u16 *pEnd = pResvTblHead + to;

    while (pCursor <= pEnd) {
        if (*pCursor != OAM_NOT_USED) {
            return FALSE;
        }
        pCursor++;
    }
    return TRUE;
}

static inline u16 GetNewGUID_(void) {
    return numRegisterdInstance_++;
}

static inline void DoReserveArea_(u16 *pResvTblHead, u16 from, u16 to, u16 ownerGUID) {
    MI_CpuFill16(pResvTblHead + from, ownerGUID, sizeof(u16) * (u32)(to - from + 1));
}

static inline void DoRestoreArea_(u16 *pResvTblHead, u16 from, u16 to) {
    MI_CpuFill16(pResvTblHead + from, OAM_NOT_USED, sizeof(u16) * (u32)(to - from + 1));
}

static inline u16 GetCapacity_(const NNSG2dOAMManageArea *pArea) {
    if (IsManageAreaValid_(pArea)) {
        return (u16)((int)pArea->toIdx - pArea->currentIdx + 1);
    } else {
        return 0;
    }
}

static inline BOOL HasEnoughCapacity_(const NNSG2dOAMManageArea *pArea, u16 num) {
    return (BOOL)(GetCapacity_(pArea) >= num);
}

static inline GXOamAttr *GetOamCurrentPtr_(NNSG2dOamManagerInstance *pMan) {
    GXOamAttr *pret = GetOamBufferPtr_(pMan->type, pMan->managedAttrArea.currentIdx);
    return pret;
}

static inline GXOamAttr *GetOamFromPtr_(NNSG2dOamManagerInstance *pMan) {
    GXOamAttr *pFrom = GetOamBufferPtr_(pMan->type, pMan->managedAttrArea.fromIdx);
    return pFrom;
}

static inline u32 GetSizeOfManageArea_(const NNSG2dOAMManageArea *pArea) {
    if (IsManageAreaValid_(pArea)) {
        return (u32)(GX_OAMATTR_SIZE * (u16)(pArea->toIdx - pArea->fromIdx + 1));
    } else {
        return 0;
    }
}

static inline void SetAffineParams_(NNSG2dOamType type, const MtxFx22 *mtx, u16 idx) {
    GXOamAffine *pAff = GetAffineBufferPtr_(type, idx);
    G2_SetOBJAffine(pAff, mtx);
}

static inline void ClearOamByDefaultValue_(NNSG2dOamType type) {
    MI_CpuFill16(GetOamBufferPtr_(type, 0), OAM_SETTING_INVISIBLE, GX_OAMATTR_SIZE * NUM_HW_OAM_ATTR);
}

static inline void SetOamReservationTblNotUsed_(NNSG2dOamType type) {
    DoRestoreArea_(GetOamReservationTblPtr_(type), 0, NUM_HW_OAM_ATTR - 1);
}

static inline void SetAffineReservationTblNotUsed_(NNSG2dOamType type) {
    DoRestoreArea_(GetAffineReservationTblPtr_(type), 0, NUM_HW_OAM_AFFINE - 1);
}

static void CpuLoadOAMSub_(const void *pSrc, u32 offset, u32 szByte) {
    MI_CpuCopy16(pSrc, (void *)(HW_DB_OAM + offset), szByte);
}

static void CpuLoadOAMMain_(const void *pSrc, u32 offset, u32 szByte) {
    MI_CpuCopy16(pSrc, (void *)(HW_OAM + offset), szByte);
}

static inline OBJLoadFunction *GetOBJLoadFunction_(NNSG2dOamType type) {
    static OBJLoadFunction *funcTbl[] = {
        CpuLoadOAMMain_,
        CpuLoadOAMSub_,
        NULL,
        NULL,
        NULL,
    };
    return funcTbl[type];
}

static inline void LoadOneAffine_(const GXOamAffine *pAff, u32 offset, OBJLoadFunction *pOBJLoadFunc) {
    offset += OAMATTR_SIZE;
    (*pOBJLoadFunc)(&pAff->PA, offset + GX_OAMATTR_SIZE * 0, sizeof(u16));
    (*pOBJLoadFunc)(&pAff->PB, offset + GX_OAMATTR_SIZE * 1, sizeof(u16));
    (*pOBJLoadFunc)(&pAff->PC, offset + GX_OAMATTR_SIZE * 2, sizeof(u16));
    (*pOBJLoadFunc)(&pAff->PD, offset + GX_OAMATTR_SIZE * 3, sizeof(u16));
}

static inline void LoadOamAndAffineFast_(NNSG2dOamType type, u16 fromIdx, u16 toIdx) {
    GXOamAttr *pFrom = GetOamBufferPtr_(type, fromIdx);
    const u16 szByte = (u16)(GX_OAMATTR_SIZE * (toIdx - fromIdx + 1));
    u16 offset = (u16)(GX_OAMATTR_SIZE * fromIdx);

    DC_FlushRange(pFrom, szByte);

    switch (type) {
    case NNS_G2D_OAMTYPE_MAIN:
        GX_LoadOAM(pFrom, offset, szByte);
        break;
    case NNS_G2D_OAMTYPE_SUB:
        GXS_LoadOAM(pFrom, offset, szByte);
        break;
    default:
        break;
    }
}

static inline void LoadOam_(NNSG2dOamType type, u16 fromIdx, u16 toIdx) {
    GXOamAttr *pFrom = GetOamBufferPtr_(type, fromIdx);
    const u16 numArea = (u16)(toIdx - fromIdx + 1);
    u16 offset = (u16)(GX_OAMATTR_SIZE * fromIdx);
    u16 i;
    OBJLoadFunction *pOBJLoadFunc = GetOBJLoadFunction_(type);

    for (i = 0; i < numArea; i++) {
        (*pOBJLoadFunc)(pFrom, offset, OAMATTR_SIZE);
        offset += GX_OAMATTR_SIZE;
        pFrom++;
    }
}

static inline void LoadAffine_(NNSG2dOamType type, u16 fromIdx, u16 toIdx) {
    GXOamAffine *pFrom = GetAffineBufferPtr_(type, fromIdx);
    const u16 numArea = (u16)(toIdx - fromIdx + 1);
    u16 offset = (u16)(GX_AFFINE_SIZE * fromIdx);
    u16 i;
    OBJLoadFunction *pOBJLoadFunc = GetOBJLoadFunction_(type);

    for (i = 0; i < numArea; i++) {
        LoadOneAffine_(pFrom, offset, pOBJLoadFunc);
        offset += GX_AFFINE_SIZE;
        pFrom++;
    }
}

static inline void ResetOam_(NNSG2dOamType type, u16 fromIdx, u16 toIdx) {
    GXOamAttr *pFrom = GetOamBufferPtr_(type, fromIdx);
    const u16 numArea = (u16)(toIdx - fromIdx + 1);
    u16 i;

    for (i = 0; i < numArea; i++) {
        *((u16 *)pFrom) = OAM_SETTING_INVISIBLE;
        pFrom++;
    }
}

void NNS_G2dInitOamManagerModule(void) {
    ClearOamByDefaultValue_(NNS_G2D_OAMTYPE_MAIN);
    ClearOamByDefaultValue_(NNS_G2D_OAMTYPE_SUB);
    ClearOamByDefaultValue_(NNS_G2D_OAMTYPE_SOFTWAREEMULATION);

    SetOamReservationTblNotUsed_(NNS_G2D_OAMTYPE_MAIN);
    SetOamReservationTblNotUsed_(NNS_G2D_OAMTYPE_SUB);
    SetOamReservationTblNotUsed_(NNS_G2D_OAMTYPE_SOFTWAREEMULATION);

    SetAffineReservationTblNotUsed_(NNS_G2D_OAMTYPE_MAIN);
    SetAffineReservationTblNotUsed_(NNS_G2D_OAMTYPE_SUB);
    SetAffineReservationTblNotUsed_(NNS_G2D_OAMTYPE_SOFTWAREEMULATION);
}

BOOL NNS_G2dGetNewManagerInstance(NNSG2dOamManagerInstance *pMan, u16 from, u16 to, NNSG2dOamType type) {
    u16 *pReserveTbl = GetOamReservationTblPtr_(type);

    if (IsOamNotUsed_(pReserveTbl, from, to)) {
        pMan->GUID = GetNewGUID_();

        pMan->managedAttrArea.fromIdx = from;
        pMan->managedAttrArea.toIdx = to;
        pMan->managedAttrArea.currentIdx = from;

        pMan->managedAffineArea.fromIdx = NUM_HW_OAM_AFFINE;
        pMan->managedAffineArea.toIdx = 0;

        pMan->type = type;
        pMan->bFastTransferEnable = FALSE;

        DoReserveArea_(pReserveTbl, from, to, pMan->GUID);
        return TRUE;
    } else {
        return FALSE;
    }
}

BOOL NNS_G2dEntryOamManagerOamWithAffineIdx(NNSG2dOamManagerInstance *pMan, const GXOamAttr *pOam, u16 affineIdx) {
    if (HasEnoughCapacity_(&pMan->managedAttrArea, 1)) {
        GXOamAttr *pOamAttr = GetOamCurrentPtr_(pMan);

        pOamAttr->attr0 = pOam->attr0;
        pOamAttr->attr1 = pOam->attr1;
        pOamAttr->attr2 = pOam->attr2;

        if (NNS_G2D_OAM_AFFINE_IDX_NONE != affineIdx) {
            if (pOamAttr->rsMode & 0x1) {
                pOamAttr->rsParam = affineIdx;
            }
        }

        pMan->managedAttrArea.currentIdx++;
        return TRUE;
    } else {
        return FALSE;
    }
}

u16 NNS_G2dEntryOamManagerAffine(NNSG2dOamManagerInstance *pMan, const MtxFx22 *mtx) {
    if (HasEnoughCapacity_(&pMan->managedAffineArea, 1)) {
        const u16 currentAffineIdx = pMan->managedAffineArea.currentIdx;

        SetAffineParams_(pMan->type, mtx, currentAffineIdx);
        pMan->managedAffineArea.currentIdx++;
        return currentAffineIdx;
    } else {
        return NNS_G2D_OAM_AFFINE_IDX_NONE;
    }
}

void NNS_G2dApplyOamManagerToHW(NNSG2dOamManagerInstance *pMan) {
    if (pMan->bFastTransferEnable) {
        LoadOamAndAffineFast_(pMan->type, pMan->managedAttrArea.fromIdx, pMan->managedAttrArea.toIdx);
    } else {
        LoadOam_(pMan->type, pMan->managedAttrArea.fromIdx, pMan->managedAttrArea.toIdx);

        if (IsManageAreaValid_(&pMan->managedAffineArea)) {
            LoadAffine_(pMan->type, pMan->managedAffineArea.fromIdx, pMan->managedAffineArea.toIdx);
        }
    }
}

void NNS_G2dResetOamManagerBuffer(NNSG2dOamManagerInstance *pMan) {
    if (pMan->bFastTransferEnable) {
        const u32 szByte = GetSizeOfManageArea_(&pMan->managedAttrArea);
        void *pData = GetOamFromPtr_(pMan);

        DC_InvalidateRange(pData, szByte);
        NNSi_G2dDmaFill32(GXi_DmaId, pData, OAM_SETTING_INVISIBLE, szByte);
    } else {
        ResetOam_(pMan->type, pMan->managedAttrArea.fromIdx, pMan->managedAttrArea.toIdx);
    }

    pMan->managedAttrArea.currentIdx = pMan->managedAttrArea.fromIdx;
    pMan->managedAffineArea.currentIdx = pMan->managedAffineArea.fromIdx;
}
