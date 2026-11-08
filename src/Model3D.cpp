// 3D models and the 3D renderer (ARM9 0x0202fa88..0x02030514). Names are ours.
#include <nitro.h>
#include <nnsys/g3d.h>
#include "game_file.h"
#include "Model3D.h"

extern "C" {
void func_0202ac98(void *vec, u32 n);   // MSL vector: reserve
void func_0202adb0(void *vec);          // MSL vector: constructor
void _ZN9SpriteVecD1Ev(void *vec);       // MSL vector: destructor (named after its first user)

// .data of the file: the vtables of Renderer3D and Renderer3DBase around the (unused) GFile vtable copy.
extern void *gModel3DVtables[12];
}

// The list of models to draw (an MSL std::vector<Model3D *>).
typedef struct ModelVec {
    Model3D **data;
    u32 size;
    u32 capacity;
    u32 unkC;
} ModelVec;

typedef struct Renderer3DObj {
    void **vt;
    ModelVec models;
} Renderer3DObj;

extern "C" {
Renderer3DObj *func_02030498(Renderer3DObj *self);
Renderer3DObj *func_020304b4(Renderer3DObj *self);
ModelVec *func_020304c8(ModelVec *vec);

Renderer3DObj *func_0202fa88(Renderer3DObj *self)
{
    func_02030498(self);
    self->vt = &gModel3DVtables[2];
    func_020304c8(&self->models);
    return self;
}

Renderer3DObj *func_0202fab0(Renderer3DObj *self)
{
    _ZN9SpriteVecD1Ev(&self->models);
    func_020304b4(self);
    return self;
}

Renderer3DObj *func_0202fad0(Renderer3DObj *self)
{
    _ZN9SpriteVecD1Ev(&self->models);
    func_020304b4(self);
    operator delete(self);
    return self;
}

void func_0202faf8(Renderer3DObj *self)
{
    if (self->models.capacity < 0x80) {
        func_0202ac98(&self->models, 0x80);
    }
}

void func_02023818(void);

// Inline MSL vector code (clear / push_back) in the original.
asm void _ZN10Renderer3D5BeginEv(void)
{
    stmdb sp!, {r3}
    sub sp, sp, #0x4
    ldr r2, [r0, #0x8]
    mov r1, #0x0
    strb r1, [sp, #0x0]
    ldr r1, [r0, #0x8]
    sub r1, r1, r2
    str r1, [r0, #0x8]
    add sp, sp, #0x4
    ldmia sp!, {r3}
    bx lr
}

asm void _ZN10Renderer3D4DrawEP7Model3D(void)
{
    stmdb sp!, {r0, r1, r2, r3}
    stmdb sp!, {r3, lr}
    mov r3, #0x0
    strb r3, [sp, #0x1]
    ldr r2, [r0, #0x8]
    ldr r1, [r0, #0xc]
    cmp r2, r1
    bhs _draw_grow
    add r3, r2, #0x1
    str r3, [r0, #0x8]
    ldr r1, [r0, #0x4]
    ldr r2, [sp, #0xc]
    sub r0, r3, #0x1
    str r2, [r1, r0, lsl #0x2]
    ldmia sp!, {r3, lr}
    add sp, sp, #0x10
    bx lr
_draw_grow:
    strb r3, [sp, #0x0]
    sub r2, sp, #0x4
    and r1, r3, #0xff
    strb r1, [r2, #0x0]
    ldr r2, [r2, #0x0]
    add r1, sp, #0xc
    add r0, r0, #0x4
    bl func_02023818
    ldmia sp!, {r3, lr}
    add sp, sp, #0x10
    bx lr
}

// Resets the placement and the animations.
void func_0202fbb4(Model3D *model)
{
    int i;

    model->trans.x = 0;
    model->trans.y = 0;
    model->trans.z = 0;
    model->scale.x = FX32_ONE;
    model->scale.y = FX32_ONE;
    model->scale.z = FX32_ONE;
    MTX_Identity33(&model->rot);
    for (i = 0; i < 5; i++) {
        model->anims[i] = NULL;
    }
    model->unkB0 = 0;
    model->unkB1 = 0;
    model->numAnims = 0;
}
}

static inline NNSG3dResMdl *GetModel(void *res, u32 index)
{
    return NNS_G3dGetMdlByIdx(NNS_G3dGetMdlSet((NNSG3dResFileHeader *)res), index);
}

Model3D *Model3D_Load(const char *mdlPath)
{
    Model3D *model = new Model3D;
    GFileObject file;

    GFile_Open(file.Get(), mdlPath);
    if (GFile_IsClosed(file.Get())) {
        return NULL;
    }
    model->unk54 = (u32) new u8[GFile_GetLength(file.Get())];
    GFile_Read(file.Get(), GFile_GetLength(file.Get()), (void *)model->unk54);
    DC_FlushRange((void *)model->unk54, GFile_GetLength(file.Get()));
    model->unk5C = (u32)NNS_G3dGetTex((NNSG3dResFileHeader *)model->unk54);
    NNS_G3dResDefaultSetup((void *)model->unk54);
    model->resMdl = GetModel((void *)model->unk54, 0);
    NNS_G3dRenderObjInit(&model->renderObj, model->resMdl);
    func_0202fbb4(model);
    return model;
}

Model3D *Model3D_LoadAnimated(const char *mdlPath, const char *anmPath)
{
    Model3D *model = Model3D_Load(mdlPath);

    if (anmPath == NULL) {
        return model;
    }
    model->anims[model->numAnims] = new ModelAnim;
    {
        GFileObject file;

        GFile_Open(file.Get(), anmPath);
        model->anims[model->numAnims]->unk0 = (u32) new u8[GFile_GetLength(file.Get())];
        GFile_Read(file.Get(), GFile_GetLength(file.Get()), (void *)model->anims[model->numAnims]->unk0);
        DC_FlushRange((void *)model->anims[model->numAnims]->unk0, GFile_GetLength(file.Get()));
        model->anims[model->numAnims]->unk4 =
            (u32)NNS_G3dGetAnmByIdx((void *)model->anims[model->numAnims]->unk0, 0);
        model->anims[model->numAnims]->anmObj = (NNSG3dAnmObj *) new u8[NNS_G3dAnmObjCalcSizeRequired(
            (void *)model->anims[model->numAnims]->unk4, model->resMdl)];
        NNS_G3dAnmObjInit(model->anims[model->numAnims]->anmObj, (void *)model->anims[model->numAnims]->unk4,
                          model->resMdl, (NNSG3dResTex *)model->unk5C);
        NNS_G3dRenderObjAddAnmObj(&model->renderObj, model->anims[model->numAnims]->anmObj);
        model->numAnims++;
    }
    return model;
}

extern "C" {

// Loads a model file (NSBMD) and sets it up (textures to VRAM).
void *func_0202feb0(const char *path)
{
    GFileObject file;
    u8 *data;

    GFile_Open(file.Get(), path);
    if (GFile_IsClosed(file.Get())) {
        return NULL;
    }
    data = new u8[GFile_GetLength(file.Get())];
    GFile_Read(file.Get(), GFile_GetLength(file.Get()), data);
    DC_FlushRange(data, GFile_GetLength(file.Get()));
    NNS_G3dResDefaultSetup(data);
    return data;
}

// Loads a texture file (NSBTX) and sets it up.
void *func_0202ff68(const char *path)
{
    GFileObject file;
    u8 *data;

    GFile_Open(file.Get(), path);
    if (GFile_IsClosed(file.Get())) {
        return NULL;
    }
    data = new u8[GFile_GetLength(file.Get())];
    GFile_Read(file.Get(), GFile_GetLength(file.Get()), data);
    DC_FlushRange(data, GFile_GetLength(file.Get()));
    NNS_G3dResDefaultSetup(data);
    return data;
}

// Model `index` of an already loaded and set up file.
Model3D *func_02030020(void *res, u32 index)
{
    Model3D *model = new Model3D;

    model->unk54 = (u32)res;
    model->unk5C = (u32)NNS_G3dGetTex((NNSG3dResFileHeader *)res);
    model->resMdl = GetModel((void *)model->unk54, index);
    NNS_G3dRenderObjInit(&model->renderObj, model->resMdl);
    func_0202fbb4(model);
    return model;
}

// Model `index` of an already loaded model file, with the textures of `tex`.
Model3D *func_020300b8(void *mdl, void *tex, u32 index)
{
    Model3D *model = new Model3D;

    model->unk54 = (u32)mdl;
    model->unk5C = (u32)NNS_G3dGetTex((NNSG3dResFileHeader *)tex);
    model->resMdl = GetModel((void *)model->unk54, index);
    NNS_G3dRenderObjInit(&model->renderObj, model->resMdl);
    func_0202fbb4(model);
    return model;
}

// Same as func_02030020, setting the file up first.
Model3D *func_02030154(void *res, u32 index)
{
    Model3D *model = new Model3D;

    model->unk54 = (u32)res;
    model->unk5C = (u32)NNS_G3dGetTex((NNSG3dResFileHeader *)res);
    NNS_G3dResDefaultSetup((void *)model->unk54);
    model->resMdl = GetModel((void *)model->unk54, index);
    NNS_G3dRenderObjInit(&model->renderObj, model->resMdl);
    func_0202fbb4(model);
    return model;
}

// Same as func_020300b8, setting the model file up first.
Model3D *func_020301f4(void *mdl, void *tex, u32 index)
{
    Model3D *model = new Model3D;

    model->unk54 = (u32)mdl;
    model->unk5C = (u32)NNS_G3dGetTex((NNSG3dResFileHeader *)tex);
    NNS_G3dResDefaultSetup((void *)model->unk54);
    model->resMdl = GetModel((void *)model->unk54, index);
    NNS_G3dRenderObjInit(&model->renderObj, model->resMdl);
    func_0202fbb4(model);
    return model;
}

// Adds an animation (an animation resource already loaded).
BOOL func_02030298(Model3D *model, void *anm, u8 type)
{
    if (anm == NULL) {
        return FALSE;
    }
    model->anims[model->numAnims] = new ModelAnim;
    model->anims[model->numAnims]->unk4 = (u32)anm;
    model->anims[model->numAnims]->anmObj = (NNSG3dAnmObj *) new u8[NNS_G3dAnmObjCalcSizeRequired(
        (void *)model->anims[model->numAnims]->unk4, model->resMdl)];
    NNS_G3dAnmObjInit(model->anims[model->numAnims]->anmObj, (void *)model->anims[model->numAnims]->unk4,
                      model->resMdl, (NNSG3dResTex *)model->unk5C);
    NNS_G3dRenderObjAddAnmObj(&model->renderObj, model->anims[model->numAnims]->anmObj);
    model->anims[model->numAnims]->unkC = type;
    model->anims[model->numAnims]->loop = 0;
    model->numAnims++;
    return TRUE;
}

// Sets animation slot `slot`.
BOOL func_02030374(Model3D *model, void *anm, int slot, u8 type)
{
    if (anm == NULL) {
        return FALSE;
    }
    model->anims[slot] = new ModelAnim;
    model->anims[slot]->unk4 = (u32)anm;
    model->anims[slot]->anmObj =
        (NNSG3dAnmObj *) new u8[NNS_G3dAnmObjCalcSizeRequired((void *)model->anims[slot]->unk4, model->resMdl)];
    NNS_G3dAnmObjInit(model->anims[slot]->anmObj, (void *)model->anims[slot]->unk4, model->resMdl,
                      (NNSG3dResTex *)model->unk5C);
    NNS_G3dRenderObjAddAnmObj(&model->renderObj, model->anims[slot]->anmObj);
    model->anims[slot]->unkC = type;
    model->anims[slot]->loop = 0;
    model->numAnims++;
    return TRUE;
}

// Resets the render object (drops the animations).
void func_02030420(Model3D *model)
{
    NNS_G3dRenderObjInit(&model->renderObj, model->resMdl);
    func_0202fbb4(model);
    model->unkB1 = 1;
}
}

void Model3D_Release(Model3D *model)
{
    int i;

    NNS_G3dResDefaultRelease((void *)model->unk54);
    for (i = 0; i < 5; i++) {
        if (model->anims[i] != NULL) {
            operator delete(model->anims[i]->anmObj);
            operator delete(model->anims[i]);
        }
    }
}

extern "C" {

BOOL func_0203048c(void *res)
{
    return NNS_G3dResDefaultSetup(res);
}

// Renderer3DBase: registers itself as gRenderer3D.
Renderer3DObj *func_02030498(Renderer3DObj *self)
{
    self->vt = &gModel3DVtables[10];
    gRenderer3D = (Renderer3D *)self;
    return self;
}

Renderer3DObj *func_020304b4(Renderer3DObj *self)
{
    gRenderer3D = NULL;
    return self;
}

ModelVec *func_020304c8(ModelVec *vec)
{
    func_0202adb0(vec);
    return vec;
}

Renderer3DObj *func_020304dc(Renderer3DObj *self)
{
    gRenderer3D = NULL;
    return self;
}

Renderer3DObj *func_020304f0(Renderer3DObj *self)
{
    gRenderer3D = NULL;
    operator delete(self);
    return self;
}

void func_0202aec0(void);
void func_0202afe0(void);

void *gModel3DVtables[12] = {
    NULL, NULL, (void *)func_0202fab0, (void *)func_0202fad0,
    NULL, NULL, (void *)func_0202aec0, (void *)func_0202afe0,
    NULL, NULL, (void *)func_020304dc, (void *)func_020304f0,
};
}
