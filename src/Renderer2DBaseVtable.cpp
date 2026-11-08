// Renderer2DBase's vtable (its destructors are asm in Sprite2D.cpp, so mwcc emits no vtable). In the
// original the last object of Sprite2D.cpp's .data, after Renderer2D's vtable.
extern "C" {
void _ZN14Renderer2DBaseD1Ev(void);
void _ZN14Renderer2DBaseD0Ev(void);

void *_ZTV14Renderer2DBase[4] = {0, 0, (void *)_ZN14Renderer2DBaseD1Ev, (void *)_ZN14Renderer2DBaseD0Ev};
}
