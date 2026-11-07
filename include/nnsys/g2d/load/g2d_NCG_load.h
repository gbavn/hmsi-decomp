#ifndef NNSYS_G2D_LOAD_NCG_LOAD_H
#define NNSYS_G2D_LOAD_NCG_LOAD_H

#include "nnsys/g2d/fmt/g2d_Character_data.h"

BOOL NNS_G2dGetUnpackedCharacterData(void *pNcgrFile, NNSG2dCharacterData **ppCharData);
void NNS_G2dUnpackNCG(NNSG2dCharacterData *pCharData);

#endif
