// The original TitleScene.cpp ends its .data with its own (unused) copy of GFile's vtable, after
// TitleScene's vtable; mwcc would sort the smaller copy first, so it lives in this unit.
extern "C" void func_0202aec0(void);   // GFile's destructors
extern "C" void func_0202afe0(void);

void *gTitleSceneFileVtable[4] = {0, 0, (void *)func_0202aec0, (void *)func_0202afe0};
