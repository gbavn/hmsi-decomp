// Overlay 5, part 8: the title screen's 3D stage (rom/maps/title).
#include <string.h>
#include <nnsys/g3d.h>
#include <nnsys/gfd.h>
#include "ov005_title.h"

#define ANM_END(anmObj) (((const NNSG3dResAnmCommon *)(anmObj)->resAnm)->numFrame << FX32_SHIFT)

TitleStage::TitleStage()
{
    char lot[64];
    char mdl[64];
    char tex[64];
    char ta[64];
    char ca[64];
    char ma[64];
    char ltr[64];
    char bin[64];
    char texs[64];
    char idx[64];
    char wna[64];
    char txa[64];
    char mta[64];
    char cha[64];
    int i;

    mTexKey = 0;
    mPlttKey = 0;
    mShip = -1;
    mBin = NULL;
    unk260 = NULL;
    unk264 = NULL;
    mTxa = NULL;
    unk26C = NULL;
    unk270 = NULL;
    mTexPack = NULL;
    for (i = 0; i < 8; i++) {
        mModels[i] = NULL;
    }
    for (i = 0; i < 32; i++) {
        mObjects[i] = NULL;
        mObjTex[i] = NULL;
        mObjTexKey[i] = 0;
        mObjPlttKey[i] = 0;
    }
    strncpy(lot, "rom/maps/title", 64);
    strncat(lot, ".lot", 4);
    strncpy(mdl, "rom/maps/title", 64);
    strncat(mdl, ".NSBMD", 6);
    strncpy(tex, "rom/maps/title", 64);
    strncat(tex, ".NSBTX", 6);
    strncpy(ta, "rom/maps/title", 64);
    strncat(ta, ".NSBTA", 6);
    strncpy(ca, "rom/maps/title", 64);
    strncat(ca, ".NSBCA", 6);
    strncpy(ma, "rom/maps/title", 64);
    strncat(ma, ".NSBMA", 6);
    strncpy(ltr, "rom/maps/title", 64);
    strncat(ltr, ".ltr", 4);
    strncpy(bin, "rom/maps/title", 64);
    strncat(bin, ".bin", 4);
    strncpy(texs, "rom/maps/title", 64);
    strncat(texs, ".tex", 4);
    strncpy(idx, "rom/maps/title", 64);
    strncat(idx, ".idx", 4);
    strncpy(wna, "rom/maps/title", 64);
    strncat(wna, ".wna", 4);
    strncpy(txa, "rom/maps/title", 64);
    strncat(txa, ".txa", 4);
    strncpy(mta, "rom/maps/title", 64);
    strncat(mta, ".mta", 4);
    strncpy(cha, "rom/maps/title", 64);
    strncat(cha, ".cha", 4);
    LoadLot(lot);
    LoadModels(mdl, ta, ca, ma, tex);
    LoadObjects(idx, bin, ltr, wna, txa, mta, cha, texs);
    if (mTexRes != NULL) {
        delete[] (u8 *)mTexRes;
    }
    if (mTexPack != NULL) {
        delete mTexPack;
    }
}

TitleStage::~TitleStage()
{
    u32 i;
    int j;
    int k;

    delete[] mEntries;
    if (mMdlRes != NULL) {
        NNS_G3dResDefaultRelease(mMdlRes);
        delete[] (u8 *)mMdlRes;
    }
    if (mAnmRes != NULL) {
        delete[] (u8 *)mAnmRes;
    }
    if (unk28 != NULL) {
        delete[] (u8 *)unk28;
    }
    if (unk2C != NULL) {
        delete[] (u8 *)unk2C;
    }
    for (i = 0; i < 8; i++) {
        if (mModels[i] == NULL) {
            break;
        }
        Model3D_Release(mModels[i]);
        delete mModels[i];
    }
    if (mPlttKey != 0) {
        NNS_GfdFreePlttVram(mPlttKey);
    }
    if (mTexKey != 0) {
        NNS_GfdFreeTexVram(mTexKey);
    }
    for (j = 0; j < 32; j++) {
        if (mObjects[j] == NULL) {
            break;
        }
        for (k = 0; k < 5; k++) {
            if (mObjects[j]->anims[k] != NULL) {
                delete (u8 *)mObjects[j]->anims[k]->unk0;
            }
        }
        if (mObjTex[j] != NULL) {
            delete[] mObjTex[j];
        }
        Model3D_Release(mObjects[j]);
        if (!mObjects[j]->unkB1) {
            delete (u8 *)mObjects[j]->unk54;
        }
        delete mObjects[j];
        if (mObjPlttKey[j] != 0) {
            NNS_GfdFreePlttVram(mObjPlttKey[j]);
        }
        if (mObjTexKey[j] != 0) {
            NNS_GfdFreeTexVram(mObjTexKey[j]);
        }
    }
    if (mBin != NULL) {
        delete mBin;
    }
    if (unk260 != NULL) {
        delete unk260;
    }
    if (unk264 != NULL) {
        delete unk264;
    }
    if (mTxa != NULL) {
        delete mTxa;
    }
    if (unk26C != NULL) {
        delete unk26C;
    }
    if (unk270 != NULL) {
        delete unk270;
    }
}

BOOL TitleStage::Update()
{
    u32 i;
    u32 j;

    for (i = 0; i < 8; i++) {
        if (mModels[i] == NULL) {
            break;
        }
        for (j = 0; j < mModels[i]->numAnims; j++) {
            ModelAnim *anim = mModels[i]->anims[j];
            if (anim != NULL) {
                anim->anmObj->frame += FX32_ONE;
                if (mModels[i]->anims[j]->anmObj->frame >=
                    ANM_END(mModels[i]->anims[j]->anmObj)) {
                    mModels[i]->anims[j]->anmObj->frame = 0;
                }
            }
        }
    }
    for (i = 0; i < 32; i++) {
        ModelAnim *anim;
        if (mObjects[i] == NULL) {
            break;
        }
        if (i == mShip) {
            mObjects[i]->trans.z = 0x8000;
        } else {
            mObjects[i]->trans.z = -0x12000;
        }
        anim = mObjects[i]->anims[1];
        if (anim != NULL && anim->loop) {
            anim->anmObj->frame += FX32_ONE;
            anim = mObjects[i]->anims[1];
            if (anim->anmObj->frame >= ANM_END(anim->anmObj)) {
                if (anim->loop == 1) {
                    anim->anmObj->frame = 0;
                } else {
                    anim->anmObj->frame = (ANM_END(anim->anmObj)) - FX32_ONE;
                }
            }
        }
    }
    return TRUE;
}

void TitleStage::Draw()
{
    VecFx32 unused = {0, 0, 0};
    u32 i;

    for (i = 0; i < 8; i++) {
        if (mModels[i] == NULL) {
            break;
        }
        gRenderer3D->Draw(mModels[i]);
    }
    for (i = 0; i < 32; i++) {
        if (mObjects[i] == NULL) {
            return;
        }
        gRenderer3D->Draw(mObjects[i]);
    }
}

BOOL TitleStage::LoadLot(const char *path)
{
    GFileObject file;
    u32 i;

    GFile_Open(file.Get(), path);
    if (GFile_IsClosed(file.Get())) {
        return FALSE;
    }
    GFile_Read(file.Get(), sizeof(LotHeader), &mLot);
    GFile_Seek(file.Get(), mLot.entryOfs, FS_SEEK_SET);
    mEntries = new LotEntry[mLot.numEntries];
    for (i = 0; i < mLot.numEntries; i++) {
        GFile_Read(file.Get(), sizeof(LotEntry), &mEntries[i]);
    }
    return TRUE;
}

BOOL TitleStage::LoadModels(const char *mdl, const char *ta, const char *ca, const char *ma, const char *tex)
{
    NNSG3dResMdlSet *set;
    u8 i;

    mMdlRes = NULL;
    mTexRes = NULL;
    mAnmRes = NULL;
    unk28 = NULL;
    unk2C = NULL;
    mMdlRes = (NNSG3dResFileHeader *)func_0202feb0(mdl);
    mAnmRes = func_0202feb0(ta);
    mTexRes = func_0202ff68(tex);
    {
        NNSG3dResTex *res = NNS_G3dGetTex((NNSG3dResFileHeader *)mTexRes);
        mTexKey = res->texInfo.vramKey;
        mPlttKey = res->plttInfo.vramKey;
    }
    set = NNS_G3dGetMdlSet(mMdlRes);
    NNS_G3dBindMdlSet(set, NNS_G3dGetTex((NNSG3dResFileHeader *)mTexRes));
    for (i = 0; i < set->dict.numEntry; i++) {
        const NNSG3dResName *name = NNS_G3dGetResNameByIdx(&set->dict, i);
        Model3D *model = func_020300b8(mMdlRes, mTexRes, i);
        void *anm;

        if (mAnmRes != NULL && (anm = NNS_G3dGetAnmByName((NNSG3dResFileHeader *)mAnmRes, name)) != NULL) {
            func_02030298(model, anm, 1);
        }
        mModels[i] = model;
    }
    return TRUE;
}

BOOL TitleStage::LoadObjects(const char *idx, const char *bin, const char *ltr, const char *wna, const char *txa,
                             const char *mta, const char *cha, const char *tex)
{
    u8 count;
    u8 sizes[64];
    s8 offsets[64];
    char name[16];
    char prefix[16];
    int i;
    u8 base;
    int n;
    int k;

    MI_CpuFill8(sizes, 0, sizeof(sizes));
    GFileObject file;
    GFile_Open(file.Get(), idx);
    GFile_Read(file.Get(), 1, &count);
    for (i = 0; i < count; i++) {
        GFile_Read(file.Get(), 1, &sizes[i]);
    }
    MI_CpuFill8(offsets, 0, sizeof(offsets));
    mBin = new PackFile(bin);
    mTxa = new PackFile(txa);
    mTexPack = new PackFile(tex);
    base = 0;
    for (k = 0, n = 0; k < count; k++) {
        BOOL skip = FALSE;
        u32 size;
        u32 texSize;
        u8 *mdlData;
        Model3D *model;
        const char *mdlName;

        if (sizes[k] == 0) {
            continue;
        }
        size = mBin->GetSize(base + offsets[k]);
        if (size == 0) {
            skip = TRUE;
        } else {
            mdlData = new u8[size];
            mBin->Read(base + offsets[k], size, mdlData);
            texSize = mTexPack->GetSize(base + offsets[k]);
            if (texSize != 0) {
                u8 *texData = new u8[texSize];
                NNSG3dResMdlSet *set;
                NNSG3dResTex *texRes;

                mTexPack->Read(base + offsets[k], texSize, texData);
                set = NNS_G3dGetMdlSet((NNSG3dResFileHeader *)mdlData);
                func_0203048c(texData);
                texRes = NNS_G3dGetTex((NNSG3dResFileHeader *)texData);
                NNS_G3dBindMdlSet(set, texRes);
                model = func_020301f4(mdlData, texData, 0);
                mObjTexKey[n] = texRes->texInfo.vramKey;
                mObjPlttKey[n] = texRes->plttInfo.vramKey;
                mObjTex[n] = texData;
            } else {
                model = func_02030154(mdlData, 0);
            }
            model->unkB1 = 0;
            mdlName = (const char *)NNS_G3dGetResNameByIdx(&model->resMdl->nodeInfo.dict, 0);
            if (strstr(mdlName, "xship") != NULL || strstr(mdlName, "yship") != NULL ||
                strstr(mdlName, "zship") != NULL) {
                mShip = n;
            }
            MI_CpuFill8(prefix, 0, sizeof(prefix));
            CopyPrefix(prefix, mdlName);
            if (CountLot(prefix) == 0) {
                Model3D_Release(model);
                delete model;
                if (size != 0) {
                    delete[] mdlData;
                }
                base += sizes[k];
                if (texSize != 0) {
                    delete[] mObjTex[n];
                    mObjTex[n] = NULL;
                    if (mObjPlttKey[n] != 0) {
                        NNS_GfdFreePlttVram(mObjPlttKey[n]);
                        mObjPlttKey[n] = 0;
                    }
                    if (mObjTexKey[n] != 0) {
                        NNS_GfdFreeTexVram(mObjTexKey[n]);
                        mObjTexKey[n] = 0;
                    }
                }
                continue;
            }
            strncpy(name, mdlName, sizeof(name));
            PlaceModel(model, prefix, FALSE);
        }
        if (!skip) {
            u32 anmIdx = base + offsets[k];
            if (anmIdx < mTxa->GetCount()) {
                u32 anmSize = mTxa->GetSize(anmIdx);
                if (anmSize != 0) {
                    u8 *anmData = new u8[anmSize];
                    mTxa->Read(base + offsets[k], anmSize, anmData);
                    func_02030374(model, NNS_G3dGetAnmByIdx(anmData, 0), 1, 1);
                    model->anims[1]->unk0 = (u32)anmData;
                    model->anims[1]->loop = 1;
                }
            }
        }
        if (!skip) {
            mObjects[n] = model;
            if (mObjTex[n] != NULL) {
                delete[] mObjTex[n];
                mObjTex[n] = NULL;
            }
            n++;
        }
        base += sizes[k];
    }
    return TRUE;
}

BOOL TitleStage::CopyPrefix(char *dst, const char *src)
{
    int i;

    for (i = 0; i < 16; i++) {
        if (src[i] == '_') {
            return TRUE;
        }
        dst[i] = src[i];
    }
    return FALSE;
}

int TitleStage::CountLot(const char *prefix)
{
    int count = 0;
    u32 i;

    for (i = 0; i < mLot.numEntries; i++) {
        if (strstr(mEntries[i].name, prefix) != NULL) {
            char name[16];
            const char *src;
            char *dst;
            int j;

            MI_CpuFill8(name, 0, sizeof(name));
            src = mEntries[i].name;
            for (j = 0, dst = name; j < 16; j++) {
                char c = src[j];
                if (c == '_') {
                    break;
                }
                *dst++ = c;
            }
            if (strcmp(name, prefix) == 0) {
                count++;
            }
        }
    }
    return count;
}

#ifdef NONMATCHING
BOOL TitleStage::PlaceModel(Model3D *model, const char *prefix, BOOL first)
{
    LotEntry *entries;
    u32 n = mLot.numEntries;
    u32 i;

    for (i = 0; i < n; i++) {
        if (!first) {
            entries = mEntries;
            if (strstr(entries[i].name, prefix) == NULL || strcmp(entries[i].name, prefix) != 0) {
                continue;
            }
        }
        model->trans = mEntries[i].pos;
        return TRUE;
    }
    return FALSE;
}
#else
// NONMATCHING: kept as assembly until the C version matches.
asm BOOL TitleStage::PlaceModel(Model3D *model, const char *prefix, BOOL first) {
    stmdb sp!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, lr}
    mov r10, r0
    ldr r7, [r10, #0x408]
    mov r11, r1
    mov r9, r2
    mov r8, r3
    cmp r7, #0x0
    mov r4, #0x0
    bls _0215a9b8
    mov r5, r4
_0215a948:
    cmp r8, #0x0
    bne _0215a97c
    ldr r6, [r10, #0x41c]
    mov r1, r9
    add r0, r6, r5
    bl strstr
    cmp r0, #0x0
    beq _0215a9a8
    mov r1, r9
    add r0, r6, r5
    bl strcmp
    cmp r0, #0x0
    bne _0215a9a8
_0215a97c:
    ldr r1, [r10, #0x41c]
    mov r0, #0x28
    mla r2, r4, r0, r1
    ldr r1, [r2, #0x10]
    mov r0, #0x1
    str r1, [r11, #0x74]
    ldr r1, [r2, #0x14]
    str r1, [r11, #0x78]
    ldr r1, [r2, #0x18]
    str r1, [r11, #0x7c]
    ldmia sp!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, pc}
_0215a9a8:
    add r4, r4, #0x1
    cmp r4, r7
    add r5, r5, #0x28
    blo _0215a948
_0215a9b8:
    mov r0, #0x0
    ldmia sp!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, pc}
}
#endif

void TitleStage::MoveShip(const VecFx32 *delta)
{
    if (mShip == -1) {
        return;
    }
    mObjects[mShip]->trans.x += delta->x;
    mObjects[mShip]->trans.y += delta->y;
    mObjects[mShip]->trans.z += delta->z;
}

void TitleStage::SetShipPos(const VecFx32 *pos)
{
    if (mShip == -1) {
        return;
    }
    mObjects[mShip]->trans.x = pos->x;
    mObjects[mShip]->trans.y = pos->y;
    mObjects[mShip]->trans.z = pos->z;
}

void TitleStage::SetShipPos(fx32 x, fx32 y, fx32 z)
{
    if (mShip == -1) {
        return;
    }
    mObjects[mShip]->trans.x = x;
    mObjects[mShip]->trans.y = y;
    mObjects[mShip]->trans.z = z;
}

// The original emits its own (unused) copy of GFile's vtable here; its references all go to ARM9's.
extern "C" void func_0202aec0(void);   // GFile's destructors
extern "C" void func_0202afe0(void);
void *gStageFileVtable[4] = {NULL, NULL, (void *)func_0202aec0, (void *)func_0202afe0};
