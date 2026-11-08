// Tasks and task managers (ARM9 0x02030514..0x020308d8), see Task.h.
//
// A TaskManager owns a buffer with two sentinel nodes (the list of running tasks and the free list, 0x14
// bytes each) followed by `count` slots of `size` bytes; tasks are created into its free slots with
// `new (manager) SomeTask(...)` and run in list order every frame.
//
// The functions are written as extern "C" functions under their linker names, with explicit vtables:
// the original emits one constructor per class and the out-of-line copies in an order our mwcc doesn't.
#include <nitro.h>
#include "Task.h"

// The data layout of a Task.
typedef struct TaskNode {
    void **vt;
    struct TaskPool *manager;
    struct TaskNode *prev;
    struct TaskNode *next;
    u8 dead;
} TaskNode;

// The data layout of a TaskManager.
typedef struct TaskPool {
    void **vt;
    TaskNode *running;    // sentinel of the running tasks
    TaskNode *free;       // sentinel of the free slots
    u32 size;
    int count;
    int freeCount;
    u8 *buffer;
} TaskPool;

extern "C" {
// .data of the file: TaskManager's and Task's vtables.
extern void *gTaskVtables[11];

TaskNode *func_020308a8(TaskPool *self, int i);
}

// The manager of the task being created/destroyed.
extern TaskPool *gCurrentTaskManager;   // defined in TaskData.cpp

extern "C" {

void *_ZN4TasknwEmP11TaskManager(size_t size, TaskPool *manager)
{
    TaskNode *node;

    if (manager->freeCount <= 0) {
        return NULL;
    }
    node = manager->free->next;
    manager->free->next = node->next;
    manager->freeCount--;
    gCurrentTaskManager = manager;
    return node;
}

TaskNode *_ZN4TaskC2Ev(TaskNode *self)
{
    self->vt = &gTaskVtables[6];
    self->dead = FALSE;
    self->manager = gCurrentTaskManager;
    self->prev = self->manager->running->prev;
    self->next = self->manager->running;
    self->next->prev = self;
    self->prev->next = self->next->prev;
    return self;
}

TaskNode *_ZN4TaskD1Ev(TaskNode *self)
{
    self->prev->next = self->next;
    self->next->prev = self->prev;
    gCurrentTaskManager = self->manager;
    return self;
}

void _ZN4TaskdlEPv(void *ptr);

TaskNode *_ZN4TaskD0Ev(TaskNode *self)
{
    self->prev->next = self->next;
    self->next->prev = self->prev;
    gCurrentTaskManager = self->manager;
    _ZN4TaskdlEPv(self);
    return self;
}

TaskNode *_ZN4TaskD2Ev(TaskNode *self)
{
    self->prev->next = self->next;
    self->next->prev = self->prev;
    gCurrentTaskManager = self->manager;
    return self;
}

void _ZN4TaskdlEPv(void *ptr)
{
    TaskPool *manager = gCurrentTaskManager;

    ((TaskNode *)ptr)->next = manager->free->next;
    manager->free->next = (TaskNode *)ptr;
    manager->freeCount++;
}

void _ZN11TaskManager6UpdateEv(TaskPool *self)
{
    Task *task;

    for (task = (Task *)self->running->next; task != (Task *)self->running; task = task->mNext) {
        if (task->mFlag) {
            task = task->mPrev;
            delete task->mNext;
        } else if (!task->Update()) {
            task->mFlag = TRUE;
        }
    }
}

BOOL _ZN4Task6UpdateEv(Task *self)
{
    return TRUE;
}

void _ZN11TaskManager4DrawEv(TaskPool *self)
{
    Task *task;

    for (task = (Task *)self->running->next; task != (Task *)self->running; task = task->mNext) {
        task->Draw();
    }
}

void _ZN4Task4DrawEv(Task *self)
{
}

void _ZN11TaskManager5Func4Ev(TaskPool *self)
{
    Task *task;

    for (task = (Task *)self->running->next; task != (Task *)self->running; task = task->mNext) {
        task->Func4();
    }
}

void _ZN4Task5Func4Ev(Task *self)
{
}

TaskPool *_ZN11TaskManagerC1Emi(TaskPool *self, u32 size, int count)
{
    int i;

    self->vt = &gTaskVtables[2];
    self->size = size;
    self->count = count;
    self->freeCount = count;
    self->buffer = new u8[self->size * self->count + 0x28];
    self->running = func_020308a8(self, 0);
    self->running->next = self->running;
    self->running->prev = self->running->next;
    self->free = func_020308a8(self, 1);
    for (i = 1; i < self->count + 1; i++) {
        func_020308a8(self, i)->next = func_020308a8(self, i + 1);
    }
    func_020308a8(self, self->count + 1)->next = self->free;
    return self;
}

TaskPool *func_0203082c(TaskPool *self)
{
    self->vt = &gTaskVtables[2];
    delete[] self->buffer;
    return self;
}

TaskPool *func_02030850(TaskPool *self)
{
    self->vt = &gTaskVtables[2];
    delete[] self->buffer;
    operator delete(self);
    return self;
}

void _ZN11TaskManager7KillAllEv(TaskPool *self)
{
    TaskNode *task;

    for (task = self->running->next; task != self->running; task = task->next) {
        task->dead = TRUE;
    }
}

// Node `i`: 0 and 1 are the two sentinels, then the slots.
TaskNode *func_020308a8(TaskPool *self, int i)
{
    if (i <= 2) {
        return (TaskNode *)(self->buffer + i * 0x14);
    }
    return (TaskNode *)(self->buffer + 0x28 + self->size * (i - 2));
}

void *gTaskVtables[11] = {
    NULL, NULL, (void *)func_0203082c, (void *)func_02030850,
    NULL, NULL, (void *)_ZN4TaskD1Ev, (void *)_ZN4TaskD0Ev,
    (void *)_ZN4Task6UpdateEv, (void *)_ZN4Task4DrawEv, (void *)_ZN4Task5Func4Ev,
};
}
