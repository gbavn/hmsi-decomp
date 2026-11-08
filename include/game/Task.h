#ifndef TASK_H
#define TASK_H

#include <nitro/types.h>
#include <stddef.h>

class TaskManager;

// Base class of the game's tasks (ARM9 0x02030550..). Tasks link themselves into the list of the
// current TaskManager and are allocated from / freed to its pool.
// Names are ours; vtable layout taken from the ROM (0x020bc7f0).
class Task {
public:
    Task();
    virtual ~Task();
    virtual BOOL Update();   // base returns TRUE; FALSE ends the task
    virtual void Draw();     // empty in the base
    virtual void Func4();    // empty in the base

    static void *operator new(size_t size, TaskManager *manager);
    static void operator delete(void *ptr);

    TaskManager *mManager;
    Task *mPrev;
    Task *mNext;
    u8 mFlag;
};

// Owns a pool of `count` tasks of at most `size` bytes each (0x02030764).
class TaskManager {
public:
    TaskManager(u32 size, int count);
    void Update();
    void Draw();
    void Func4();     // calls Func4() of every task
    void KillAll();   // flags every task to be deleted

    u8 unk[0x1c];
};

// Object at 0x020f57b0 used by overlay tasks to request the next scene.
class SceneControl {
public:
    void Request(int scene);
};

extern SceneControl *gSceneControl;

// The task manager of each scene (0x020f578c), created by the scene manager when the scene starts.
extern TaskManager *gSceneTaskManagers[9];

#endif
