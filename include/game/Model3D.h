#ifndef MODEL3D_H
#define MODEL3D_H

#include <nnsys/g3d.h>

// An animation attached to a Model3D (only the NNS animation object is known).
struct ModelAnim {
    u32 unk0;
    u32 unk4;
    NNSG3dAnmObj *anmObj;
};

// A loaded 3D model with its render object and placement (0xB4 bytes).
struct Model3D {
    NNSG3dRenderObj renderObj;
    u32 unk54;               // buffer freed with delete[]
    NNSG3dResMdl *resMdl;
    u32 unk5C;
    ModelAnim *anims[5];
    VecFx32 trans;
    VecFx32 scale;
    MtxFx33 rot;
    u8 unkB0;
    u8 unkB1;
    u8 numAnims;
    u8 unkB3;
};

Model3D *Model3D_Load(const char *mdlPath);                       // 0x0202fc10
Model3D *Model3D_LoadAnimated(const char *mdlPath, const char *anmPath); // 0x0202fd40
void Model3D_Release(Model3D *model);                             // 0x02030444

// Renderer object at 0x020f577c.
class Renderer3D {
public:
    void Begin();                  // 0x0202fb18
    void Draw(Model3D *model);     // 0x0202fb44
    BOOL DrawCulled(Model3D *model); // 0x01ff8d8c (ITCM)
    void Flush();                  // 0x01ff8ef8 (ITCM)
};

extern Renderer3D *gRenderer3D;

u32 RandRange(u32 min, u32 max); // 0x020358ec

#endif
