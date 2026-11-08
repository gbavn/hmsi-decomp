// The scene manager (ARM9 0x020308d8..0x02030cfc). Names are ours.
//
// A Task that runs the current scene: each scene has its overlay (plus overlay 8, always loaded with it)
// and its own TaskManager, created when the scene starts with the scene's root task. Other code asks for
// the next scene through gSceneControl->Request(scene); scene 9 means "none".
#include <nitro.h>
#include "Task.h"

typedef struct SceneManager {
    void **vt;                  // Task
    void *manager;
    void *prev;
    void *next;
    u8 dead;
    void **controlVt;           // SceneControl (second base)
    int current;
    int overlay2;
    int requested;
    int loadedOverlay;
    int state;
} SceneManager;

extern "C" {
// .data of the file: the overlay of each scene, then the vtables of SceneManager, of its SceneControl part
// (thunks) and of SceneControl.
extern void *gSceneData[24];

void *_ZN4TaskC2Ev(void *self);
void *_ZN4TaskD2Ev(void *self);
void *_ZN4TasknwEmP11TaskManager(size_t size, TaskManager *manager);
void *_ZN11TaskManagerC1Emi(void *self, u32 size, int count);
void _ZN11TaskManager6UpdateEv(TaskManager *self);
void _ZN11TaskManager4DrawEv(TaskManager *self);
void _ZN11TaskManager5Func4Ev(TaskManager *self);
void _ZN11TaskManager7KillAllEv(TaskManager *self);
void func_02028288(void *self);
void func_02071704(void);

// Root tasks of the scenes (in their overlays).
void *_ZN9Ov001TaskC1Ev(void *self);
void *func_ov002_02192474(void *self);
void *func_02067054(size_t size);
void *func_02066d3c(void *self);
void *func_ov004_02158d20(void *self);
void *_ZN11IslandScenenwEm(size_t size);

void *func_02030c60(void ***self);
}

extern SceneControl *gSceneControl;

// TaskManager's virtual destructors (a view class: see PackFile.cpp).
class TaskManagerView {
public:
    virtual void *Dtor();
    virtual void *DtorDelete();
};

#define SCENE_OVERLAYS ((const FSOverlayID *)gSceneData)

extern "C" {

SceneManager *func_020308d8(SceneManager *self)
{
    _ZN4TaskC2Ev(self);
    func_02030c60(&self->controlVt);
    self->vt = &gSceneData[11];
    self->controlVt = &gSceneData[18];
    self->current = 9;
    self->overlay2 = 9;
    self->requested = 9;
    return self;
}

static inline TaskManager *NewTaskManager(u32 size)
{
    void *manager = operator new(0x1c);

    if (manager != NULL) {
        manager = _ZN11TaskManagerC1Emi(manager, size, 1);
    }
    return (TaskManager *)manager;
}

#ifdef NONMATCHING
BOOL func_0203091c(SceneManager *self)
{
    if (self->state == 0) {
        _ZN11TaskManager7KillAllEv(gSceneTaskManagers[self->current]);
        self->state = 1;
    } else if (self->state == 1) {
        if (gSceneTaskManagers[self->current] != NULL) {
            ((TaskManagerView *)gSceneTaskManagers[self->current])->DtorDelete();
        }
        if (self->loadedOverlay == SCENE_OVERLAYS[self->requested]) {
            self->state = 2;
        } else {
            FS_UnloadOverlay(MI_PROCESSOR_ARM9, SCENE_OVERLAYS[self->current]);
            FS_UnloadOverlay(MI_PROCESSOR_ARM9, SCENE_OVERLAYS[self->overlay2]);
            self->state = 2;
        }
    }
    if (self->state == 2) {
        void *task;

        self->loadedOverlay = SCENE_OVERLAYS[self->requested];
        FS_LoadOverlay(MI_PROCESSOR_ARM9, self->loadedOverlay);
        self->overlay2 = 8;
        FS_LoadOverlay(MI_PROCESSOR_ARM9, SCENE_OVERLAYS[self->overlay2]);
        switch (self->requested) {
        case 0:
            gSceneTaskManagers[self->requested] = NewTaskManager(0x14);
            task = _ZN4TasknwEmP11TaskManager(0x14, gSceneTaskManagers[0]);
            if (task != NULL) {
                _ZN9Ov001TaskC1Ev(task);
            }
            break;
        case 1:
            gSceneTaskManagers[self->requested] = NewTaskManager(0x678);
            task = _ZN4TasknwEmP11TaskManager(0x678, gSceneTaskManagers[1]);
            if (task != NULL) {
                func_ov002_02192474(task);
            }
            break;
        case 2:
            gSceneTaskManagers[self->requested] = NewTaskManager(0xac);
            task = func_02067054(0xac);
            if (task != NULL) {
                func_02066d3c(task);
            }
            break;
        case 5:
            gSceneTaskManagers[self->requested] = NewTaskManager(0xa0);
            task = _ZN4TasknwEmP11TaskManager(0xa0, gSceneTaskManagers[5]);
            if (task != NULL) {
                _ZN9Ov001TaskC1Ev(task);
            }
            break;
        case 7:
            gSceneTaskManagers[self->requested] = NewTaskManager(0x20);
            task = _ZN4TasknwEmP11TaskManager(0x20, gSceneTaskManagers[7]);
            if (task != NULL) {
                func_ov004_02158d20(task);
            }
            break;
        case 6:
            gSceneTaskManagers[self->requested] = NewTaskManager(0x118);
            task = _ZN11IslandScenenwEm(0x118);
            if (task != NULL) {
                _ZN9Ov001TaskC1Ev(task);
            }
            break;
        }
        self->state = 3;
        self->current = self->requested;
        self->requested = 9;
    }
    _ZN11TaskManager6UpdateEv(gSceneTaskManagers[self->current]);
    return TRUE;
}
#else
// Linker names used by the asm below.
extern "C" {
void _Znwm(void);
}
// NONMATCHING: kept as assembly until the C version matches.
asm BOOL func_0203091c(SceneManager *self) {
    stmdb sp!, {r4, lr}
    mov r4, r0
    ldr r0, [r4, #0x28]
    cmp r0, #0x0
    bne _0203094c
    ldr r1, [r4, #0x18]
    ldr r0, [pc, #0x2bc]
    ldr r0, [r0, r1, lsl #0x2]
    bl _ZN11TaskManager7KillAllEv
    mov r0, #0x1
    str r0, [r4, #0x28]
    b _020309c0
_0203094c:
    cmp r0, #0x1
    bne _020309c0
    ldr r1, [r4, #0x18]
    ldr r0, [pc, #0x298]
    ldr r0, [r0, r1, lsl #0x2]
    cmp r0, #0x0
    beq _02030974
    ldr r1, [r0, #0x0]
    ldr r1, [r1, #0x4]
    blx r1
_02030974:
    ldr r0, [r4, #0x20]
    ldr r2, [pc, #0x27c]
    ldr r1, [r4, #0x24]
    ldr r0, [r2, r0, lsl #0x2]
    cmp r1, r0
    moveq r0, #0x2
    streq r0, [r4, #0x28]
    beq _020309c0
    ldr r1, [r4, #0x18]
    mov r0, #0x0
    ldr r1, [r2, r1, lsl #0x2]
    bl FS_UnloadOverlay
    ldr r2, [r4, #0x1c]
    ldr r1, [pc, #0x24c]
    mov r0, #0x0
    ldr r1, [r1, r2, lsl #0x2]
    bl FS_UnloadOverlay
    mov r0, #0x2
    str r0, [r4, #0x28]
_020309c0:
    ldr r0, [r4, #0x28]
    cmp r0, #0x2
    bne _02030be0
    ldr r2, [r4, #0x20]
    ldr r1, [pc, #0x224]
    mov r0, #0x0
    ldr r1, [r1, r2, lsl #0x2]
    str r1, [r4, #0x24]
    bl FS_LoadOverlay
    mov r1, #0x8
    ldr r0, [pc, #0x20c]
    str r1, [r4, #0x1c]
    ldr r1, [r0, r1, lsl #0x2]
    mov r0, #0x0
    bl FS_LoadOverlay
    ldr r0, [r4, #0x20]
    cmp r0, #0x7
    addls pc, pc, r0, lsl #0x2
    b _02030bc8
_02030a0c:
    b _02030a2c
    b _02030a74
    b _02030abc
    b _02030bc8
    b _02030bc8
    b _02030afc
    b _02030b8c
    b _02030b44
_02030a2c:
    mov r0, #0x1c
    bl _Znwm
    cmp r0, #0x0
    beq _02030a48
    mov r1, #0x14
    mov r2, #0x1
    bl _ZN11TaskManagerC1Emi
_02030a48:
    ldr r3, [r4, #0x20]
    ldr r2, [pc, #0x1a4]
    ldr r1, [pc, #0x1a8]
    str r0, [r2, r3, lsl #0x2]
    ldr r1, [r1, #0x0]
    mov r0, #0x14
    bl _ZN4TasknwEmP11TaskManager
    cmp r0, #0x0
    beq _02030bc8
    bl _ZN9Ov001TaskC1Ev
    b _02030bc8
_02030a74:
    mov r0, #0x1c
    bl _Znwm
    cmp r0, #0x0
    beq _02030a90
    ldr r1, [pc, #0x178]
    mov r2, #0x1
    bl _ZN11TaskManagerC1Emi
_02030a90:
    ldr r3, [r4, #0x20]
    ldr r2, [pc, #0x15c]
    ldr r1, [pc, #0x160]
    str r0, [r2, r3, lsl #0x2]
    ldr r1, [r1, #0x4]
    ldr r0, [pc, #0x158]
    bl _ZN4TasknwEmP11TaskManager
    cmp r0, #0x0
    beq _02030bc8
    bl func_ov002_02192474
    b _02030bc8
_02030abc:
    mov r0, #0x1c
    bl _Znwm
    cmp r0, #0x0
    beq _02030ad8
    mov r1, #0xac
    mov r2, #0x1
    bl _ZN11TaskManagerC1Emi
_02030ad8:
    ldr r2, [r4, #0x20]
    ldr r1, [pc, #0x114]
    str r0, [r1, r2, lsl #0x2]
    mov r0, #0xac
    bl func_02067054
    cmp r0, #0x0
    beq _02030bc8
    bl func_02066d3c
    b _02030bc8
_02030afc:
    mov r0, #0x1c
    bl _Znwm
    cmp r0, #0x0
    beq _02030b18
    mov r1, #0xa0
    mov r2, #0x1
    bl _ZN11TaskManagerC1Emi
_02030b18:
    ldr r3, [r4, #0x20]
    ldr r2, [pc, #0xd4]
    ldr r1, [pc, #0xd8]
    str r0, [r2, r3, lsl #0x2]
    ldr r1, [r1, #0x14]
    mov r0, #0xa0
    bl _ZN4TasknwEmP11TaskManager
    cmp r0, #0x0
    beq _02030bc8
    bl _ZN9Ov001TaskC1Ev
    b _02030bc8
_02030b44:
    mov r0, #0x1c
    bl _Znwm
    cmp r0, #0x0
    beq _02030b60
    mov r1, #0x20
    mov r2, #0x1
    bl _ZN11TaskManagerC1Emi
_02030b60:
    ldr r3, [r4, #0x20]
    ldr r2, [pc, #0x8c]
    ldr r1, [pc, #0x90]
    str r0, [r2, r3, lsl #0x2]
    ldr r1, [r1, #0x1c]
    mov r0, #0x20
    bl _ZN4TasknwEmP11TaskManager
    cmp r0, #0x0
    beq _02030bc8
    bl func_ov004_02158d20
    b _02030bc8
_02030b8c:
    mov r0, #0x1c
    bl _Znwm
    cmp r0, #0x0
    beq _02030ba8
    mov r1, #0x118
    mov r2, #0x1
    bl _ZN11TaskManagerC1Emi
_02030ba8:
    ldr r2, [r4, #0x20]
    ldr r1, [pc, #0x44]
    str r0, [r1, r2, lsl #0x2]
    mov r0, #0x118
    bl _ZN11IslandScenenwEm
    cmp r0, #0x0
    beq _02030bc8
    bl _ZN9Ov001TaskC1Ev
_02030bc8:
    mov r0, #0x3
    str r0, [r4, #0x28]
    ldr r1, [r4, #0x20]
    mov r0, #0x9
    str r1, [r4, #0x18]
    str r0, [r4, #0x20]
_02030be0:
    ldr r1, [r4, #0x18]
    ldr r0, [pc, #0xc]
    ldr r0, [r0, r1, lsl #0x2]
    bl _ZN11TaskManager6UpdateEv
    mov r0, #0x1
    ldmia sp!, {r4, pc}
    dcd gSceneTaskManagers
    dcd gSceneData
    dcd gSceneTaskManagers
    dcd 0x678
}
#endif

void func_02030c08(SceneManager *self)
{
    _ZN11TaskManager4DrawEv(gSceneTaskManagers[self->current]);
}

void func_02030c24(SceneManager *self)
{
    _ZN11TaskManager5Func4Ev(gSceneTaskManagers[self->current]);
    func_02071704();
}

void _ZN12SceneControl7RequestEi(SceneManager *self, int scene)
{
    self->requested = scene;
    self->state = self->current == 9 ? 2 : 0;
}

void *func_02030c60(void ***self)
{
    *self = &gSceneData[22];
    gSceneControl = (SceneControl *)((u8 *)self - 0x14);
    return self;
}

SceneManager *func_02030c80(SceneManager *self)
{
    gSceneControl = NULL;
    _ZN4TaskD2Ev(self);
    return self;
}

void *func_02030ca4(void *self)
{
    gSceneControl = NULL;
    return self;
}

void *func_02030cb8(void *self)
{
    gSceneControl = NULL;
    operator delete(self);
    return self;
}

// Destructor thunks of the SceneControl part.
asm void func_02030cdc(void)
{
    ldr ip, [pc, #0x4]
    add r0, r0, ip
    b func_02030c80
    dcd -0x14
}

asm void func_02030cec(void)
{
    ldr ip, [pc, #0x4]
    add r0, r0, ip
    b func_02028288
    dcd -0x14
}

void *gSceneData[24] = {
    (void *)1, (void *)2, (void *)4, (void *)7, (void *)6, (void *)8, (void *)3, (void *)5, (void *)9,
    NULL, NULL, (void *)func_02030c80, (void *)func_02028288, (void *)func_0203091c, (void *)func_02030c08,
    (void *)func_02030c24,
    NULL, NULL, (void *)func_02030cdc, (void *)func_02030cec,
    NULL, NULL, (void *)func_02030ca4, (void *)func_02030cb8,
};
}
