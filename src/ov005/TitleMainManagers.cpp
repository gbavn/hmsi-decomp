// The title overlay's task managers. In the original this array is a static of TitleMain.cpp, before
// the vblank_count that <nitro/gx/gx.h> defines; our mwcc puts vblank_count first, so the array lives
// in this unit, with a 10th slot standing in for TitleMain.cpp's vblank_count (an unreferenced .bss
// section would be dropped by the linker). Layout workaround; it also keeps TitleMain's constructor
// and Update in asm (the original addressed the array through two literal-pool entries).
class TaskManager;

TaskManager *sTitleManagers[10];
