// State shared by IslandView's joint callbacks. In the ROM it comes first in the overlay's .bss, before
// IslandView.cpp's vblank_count (static local of GX_SetVCount in <nitro/gx/gx.h>), and the follow
// state comes before the joint matrix; our mwcc orders .bss objects differently (by size, with that
// variable first), so both live in one struct in their own unit, which includes no GX header.
// Layout workaround.
#include "ov003_follow.h"

FollowState sFollowState;
