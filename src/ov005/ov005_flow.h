#ifndef OV005_FLOW_H
#define OV005_FLOW_H

// Kept apart from ov005_title.h so that TitleFlowData.cpp does not include <nitro/gx/gx.h>
// (and its vblank_count).
#include <nitro/types.h>

class Task;

// State of TitleFlow (0x0215bb88).
struct TitleFlowState {
    u8 fade;
    u8 unk1;
    u8 state;
    u8 next;
    u8 current;
    Task *task;
    u8 unkC[0xc];   // stands in for TitleFlow.cpp's vblank_count and padding (see TitleFlowData.cpp)
};
extern TitleFlowState sTitleFlow;

#endif
