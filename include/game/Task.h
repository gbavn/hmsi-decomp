#ifndef TASK_H
#define TASK_H

#include <nitro/types.h>

class TaskManager;

// Base class of the game's tasks (ARM9 0x02030550..). Tasks link themselves into the list of the
// current TaskManager (gTaskManager) and are freed through a class-specific operator delete.
// Names are ours; vtable layout taken from the ROM (0x020bc7f0).
class Task {
public:
    Task();
    virtual ~Task();
    virtual BOOL Update();   // base returns TRUE
    virtual void Func3();    // empty in the base
    virtual void Func4();    // empty in the base

    static void operator delete(void *ptr);

    TaskManager *mManager;
    Task *mPrev;
    Task *mNext;
    u8 mFlag;
};

// Object at 0x020f57b0 used by overlay tasks to request the next scene.
class SceneControl {
public:
    void Request(int scene);
};

extern SceneControl *gSceneControl;

#endif
