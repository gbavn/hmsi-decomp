#ifndef OV008_SHEEP_H
#define OV008_SHEEP_H

#include <nnsys/g3d.h>
#include "Task.h"
#include "Model3D.h"

#define WOOL_COUNT 32

class SheepScene : public Task {
public:
    SheepScene();
    virtual ~SheepScene();
    virtual BOOL Update();
    virtual void Draw();
    virtual void Func4();

    void InitGraphics();
    static void BeginFrame();
    void SetupLight();

    u8 mLightR;
    u8 mLightG;
    u8 mLightB;
    VecFx32 mCamPos;
    VecFx32 mCamTarget;
    VecFx32 mCamUp;
    u8 unk38[0x30];
    fx32 mFovySin;
    fx32 mFovyCos;
    fx32 mAspect;
    fx32 mNear;
    fx32 mFar;
    u8 unk7C[0x18];
    fx32 mCamDist;
    u16 mCamAngle;
    u16 unk9A;
    u16 unk9C;
};

class SheepModel : public Task {
public:
    SheepModel();
    virtual ~SheepModel();
    virtual BOOL Update();
    virtual void Draw();

    static void NodeCallback(NNSG3dRS *rs);

    Model3D *mModel;
};

class SheepWool : public Task {
public:
    SheepWool();
    virtual ~SheepWool();
    virtual BOOL Update();
    virtual void Draw();

    Model3D *mBase;
    Model3D *mParts[WOOL_COUNT];
    VecFx32 mVel[WOOL_COUNT];
    VecFx32 mDir[WOOL_COUNT];
    VecFx32 mGravity;
    fx32 mAlpha[WOOL_COUNT];
    u8 mVisible[WOOL_COUNT];
};

#endif
