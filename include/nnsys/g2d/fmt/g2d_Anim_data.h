#ifndef NNSYS_G2D_FMT_ANIM_DATA_H
#define NNSYS_G2D_FMT_ANIM_DATA_H

#include "nnsys/g2d/fmt/g2d_Common_data.h"

#define NNS_G2D_BLKSIG_ANIMBANK (u32)('ABNK')

typedef enum NNSG2dAnimationPlayMode {
    NNS_G2D_ANIMATIONPLAYMODE_INVALID = 0,
    NNS_G2D_ANIMATIONPLAYMODE_FORWARD,
    NNS_G2D_ANIMATIONPLAYMODE_FORWARD_LOOP,
    NNS_G2D_ANIMATIONPLAYMODE_REVERSE,
    NNS_G2D_ANIMATIONPLAYMODE_REVERSE_LOOP,
    NNS_G2D_ANIMATIONPLAYMODE_MAX
} NNSG2dAnimationPlayMode;

typedef struct NNSG2dAnimFrameData {
    void *pContent;
    u16 frames;
    u16 pad16;
} NNSG2dAnimFrameData;

typedef struct NNSG2dAnimSequenceData {
    u16 numFrames;
    u16 loopStartFrameIdx;
    u32 animType;
    NNSG2dAnimationPlayMode playMode;
    NNSG2dAnimFrameData *pAnmFrameArray;
} NNSG2dAnimSequenceData;

typedef struct NNSG2dAnimBankData {
    u16 numSequences;
    u16 numTotalFrames;
    NNSG2dAnimSequenceData *pSequenceArrayHead;
    NNSG2dAnimFrameData *pFrameArrayHead;
    void *pAnimContents;
    void *pStringBank;
    void *pExtendedData;
} NNSG2dAnimBankData;

typedef struct NNSG2dAnimBankDataBlock {
    NNSG2dBinaryBlockHeader blockHeader;
    NNSG2dAnimBankData animBankData;
} NNSG2dAnimBankDataBlock;

typedef struct NNSG2dUserExAnimFrameAttr {
    u32 *pAttr;
} NNSG2dUserExAnimFrameAttr;

typedef struct NNSG2dUserExAnimSequenceAttr {
    u16 numFrames;
    u16 pad16;
    u32 *pAttr;
    NNSG2dUserExAnimFrameAttr *pAnmFrmAttrArray;
} NNSG2dUserExAnimSequenceAttr;

typedef struct NNSG2dUserExAnimAttrBank {
    u16 numSequences;
    u16 numAttribute;
    NNSG2dUserExAnimSequenceAttr *pAnmSeqAttrArray;
} NNSG2dUserExAnimAttrBank;

#endif
