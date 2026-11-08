// The manager of the task being created/destroyed. In the original a global of Task.cpp placed before the
// vblank_count of <nitro/gx/gx.h>; our mwcc puts vblank_count first, so it lives in this unit.
struct TaskPool;

TaskPool *gCurrentTaskManager;
