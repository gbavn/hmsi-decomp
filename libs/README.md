# Libraries

- `NitroSDK/` and `NitroSystem/`: decompiled Nintendo DS SDK sources from
  https://github.com/ntrtwl/NitroSDK (eaae40f) and https://github.com/ntrtwl/NitroSystem (21c8fd0),
  the same revisions used by pret/pokeplatinum. Library units in `config/usa/arm9/delinks.txt`
  point straight at these files; `-dead` linking strips the functions the game doesn't use.
- `stub/`: minimal C library headers plus the generated `nitro/fx/fx_const.h`
  (from `NitroSDK/gen/nitro/fx/fx_const.csv` via `scripts/gen_fx_const.py`).

## Changes to match this game's SDK/NitroSystem version

Every edit to the vendored sources is listed here.

- `NitroSDK/libraries/mi/src/mi_dma.c`: added `MI_StopAllDma` (missing from the reference revision).
- `scripts/flags.py` defines `SP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS` for all vendored sources: this game's SDK
  was built with the branch-around variant of conditional asm instructions (os_exception, os_reset, ...).
- `NitroSDK/libraries/os/src/os_reset.c`: `OS_ResetSystem` masks IRQs (`~OS_IE_FIFO_RECV`) before stopping DMA.
