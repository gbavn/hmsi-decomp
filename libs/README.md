# Libraries

- `NitroSDK/` and `NitroSystem/`: decompiled Nintendo DS SDK sources from
  https://github.com/ntrtwl/NitroSDK (eaae40f) and https://github.com/ntrtwl/NitroSystem (21c8fd0),
  the same revisions used by pret/pokeplatinum. Library units in `config/usa/arm9/delinks.txt`
  point straight at these files; `-dead` linking strips the functions the game doesn't use.
- `stub/`: minimal C library headers plus the generated `nitro/fx/fx_const.h`
  (from `NitroSDK/gen/nitro/fx/fx_const.csv` via `scripts/gen_fx_const.py`).
