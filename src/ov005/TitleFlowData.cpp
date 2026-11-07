// TitleFlow's state. In the original a static of TitleFlow.cpp, before the vblank_count that
// <nitro/gx/gx.h> defines; our mwcc puts vblank_count first, so it lives in this unit (the struct is
// padded over TitleFlow.cpp's vblank_count). Layout workaround.
#include "ov005_flow.h"

TitleFlowState sTitleFlow;
