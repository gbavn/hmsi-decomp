// The task manager of each scene. In the original a static of Scene.cpp placed before the vblank_count
// of <nitro/gx/gx.h>; our mwcc puts vblank_count first, so it lives in this unit (and gSceneControl in
// SceneData2.cpp, to keep the order). Layout workaround.
class TaskManager;

TaskManager *gSceneTaskManagers[9];
