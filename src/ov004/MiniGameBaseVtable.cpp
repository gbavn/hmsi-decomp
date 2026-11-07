// MiniGameBase's vtable (its destructors are asm in MiniGame.cpp, so mwcc emits no vtable). In the
// original the last object of MiniGame.cpp's .data, after MiniGame's vtable.
extern "C" {
void _ZN12MiniGameBaseD1Ev(void);
void _ZN12MiniGameBaseD0Ev(void);

void *_ZTV12MiniGameBase[4] = {0, 0, (void *)_ZN12MiniGameBaseD1Ev, (void *)_ZN12MiniGameBaseD0Ev};
}
