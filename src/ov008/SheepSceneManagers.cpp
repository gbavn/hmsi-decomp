// The scene's two task managers. In the original this was a `static` array in SheepScene.cpp, placed
// first in that file's .bss and followed by vblank_count (the static local of GX_SetVCount in
// <nitro/gx/gx.h>, present in every game file). Our mwcc always puts arrays after that variable, and
// an unreferenced .bss section would be dropped by the linker, so the array lives in its own unit and
// a third slot stands in for SheepScene.cpp's vblank_count. Layout workaround; it also forces
// SheepScene's constructor to stay in asm (the original used two literal-pool entries for the array).
class TaskManager;

TaskManager *sTaskManagers[3];
