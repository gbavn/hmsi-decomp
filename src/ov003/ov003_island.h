#ifndef OV003_ISLAND_H
#define OV003_ISLAND_H

// Overlay 3: island fly-over. Loads the model set of the current island (rom/island/*), plays its
// animations and drives the camera along the model's "camera" joint, with camera shakes and sound
// cues at fixed frames. Class and function names are ours; unknown ARM9 functions keep dsd names.
#include <nnsys/g3d.h>
#include "Task.h"
#include "Model3D.h"

extern "C" {
void func_02035ed4(int a, int b);
void func_02035f30(void);
void func_02035e44(int a);
void func_02035e8c(int a, int b);
void func_02037238(void);
void func_02036048(int a);
void func_0202bf9c(void);
void func_02029fe8(void *obj);
void func_0202bbf4(void *obj);
void func_02037284(int a, int b, int c);
BOOL func_020374f8(int a);
void func_0203357c(void *obj, int id, int n);
void func_02037410(void);
void func_02035d88(void);
int func_0203366c(void *obj, int id);
void func_02035fc0(int a, int b);
int func_020358d0(int n);
void *func_0202feb0(const char *path);
Model3D *func_02030020(void *res, u32 idx);
void func_02030298(Model3D *model, void *anm, int type);
extern void *data_020ec870;
extern void *data_020ec87c;
extern void *data_020f57c0;
}

void Gfx_Dummy0(void);
void Gfx_InitMain3D(void);

struct TaskSystem {
    u8 unk0[0x18];
    TaskManager *mSceneManager;
};
extern TaskSystem data_020f578c;

class IslandCamera {
public:
    IslandCamera();
    ~IslandCamera();
    void Setup();
    void Apply();
    void SetPos(const VecFx32 *pos);
    void SetTarget(const VecFx32 *target);
    void StartShake(fx32 amp, int period);
    void StopShake();

    VecFx32 mPos;
    VecFx32 mTarget;
    VecFx32 mUp;
    u8 unk24[0x30];
    fx32 mFovySin;
    fx32 mFovyCos;
    fx32 mAspect;
    fx32 mNear;
    fx32 mFar;
    u8 unk68[0x18];
    fx32 mDist;
    u16 mAngle;
    u16 unk86;
    u16 unk88;
    u8 unk8A[0x1A];
    u8 unkA4;
    u8 unkA5[0x2B];
    s8 mShaking;
    fx32 mShakeAmp;
    int mShakePeriod;
    int mShakeCounter;
    u8 unkE0[0xC];
    s8 mShakeDir;
    int mShakeRand;
    f32 mShakeRandF;
};

class IslandView {
public:
    IslandView(IslandCamera *camera, IslandCamera *followCamera);
    virtual ~IslandView();
    virtual BOOL Update();
    virtual void Draw();

    BOOL UpdateAnimsIsland8();
    BOOL UpdateAnims();
    BOOL LoadResources(const char *mdl, const char *tp, const char *ta, const char *ca, const char *ma,
                       const char *va);

    static void FollowCallbackIsland8(NNSG3dRS *rs);
    static void FollowCallback(NNSG3dRS *rs);

    int mIsland;
    void *mRes[6];
    Model3D *mModels[64];
    int mNumModels;
    int mFocusIdx;
    u8 unk128;
    IslandCamera *mCamera;
    int mFrame;
};

class IslandScene : public Task {
public:
    IslandScene();
    virtual ~IslandScene();
    virtual BOOL Update();
    virtual void Draw();
    virtual void Func4();

    static void *operator new(size_t size);

    void StartFadeOut();
    void InitGraphics();
    void BeginFrame();
    void SetupLight();
    BOOL IsDone();
    void Func0215612c();

    IslandView *mView;
    IslandCamera mCamera;
    int mNextScene;
    u8 mFading;
};

#include "ov003_follow.h"


#endif
