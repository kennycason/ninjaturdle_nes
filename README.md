# Ninja Turdle NES DEV


## Build cc65

```bash
cd tools/cc65/
make
```

## Build & Run Ninja Turdle

```bash
make
fceux ninjaturdle.nes
```

## Reginerating NES .CHR files

```bash
./tools/png2chr ninjaturdle.png
./tools/png2chr ninjaturdle2.png
```

This generates `ninjaturdle.chr` and `ninjaturdle2.chr`.

- **Tip**: If you regenerate CHR and `make` doesn’t seem to pick it up, run:

```bash
make clean && make
```

## Maps workflow (TMX -> C)

Levels live in `map/w1l1.tmx`, `map/w1l2.tmx`, etc. The converter produces C data used by the game.

- **Convert a TMX**:

```bash
cd map
source venv/bin/activate
python convert_tmx.py 1 1
```

This overwrites `map/w1l1.c` (and similarly for other levels).

- **What gets generated**:
- `wXlY_main_*[]`: background metatile indices (16x15 per room)
- `wXlY_coins[]`: coin spawns
- `wXlY_enemies[]`: enemy spawns (supports an optional 5th “param” byte for some enemies)

## Sprites workflow (tile indices)

Enemy/player sprites are metasprites (2x2 of 8x8 tiles = 16x16) defined in `sprites.h`.
If you move art around in the tilesheet, update the tile indices in `sprites.h` to match.


## `kenes.h`

### Mapper / banking (MMC1)

This lib extends **Shiru's `neslib`** and **Doug Fraker's `nesdoug`**. Those libraries commonly target the simplest cartridge layout (**NROM / mapper 0**, i.e. no bank switching). **Ninja Turdle extends that baseline to support MMC1 (mapper 1)** for CHR banking and a larger ROM layout.

- **Mapper / memory config**: `mmc1_32k.cfg`
  - `NES_MAPPER = 1` (MMC1)
  - PRG: 32KB at `$8000`
  - CHR: 16KB total (4 × 4KB banks)
- **Startup / iNES header**: `crt0.s` (uses the `NES_*` symbols from `mmc1_32k.cfg`)
- **Project helpers**: `lib/kenes.h`
  - MMC1 helpers: `mmc1_write()`, `mmc1_init()`
  - CHR bank constants used by the game: `CHR_BANK_FONT`, `CHR_BANK_TITLE`, `CHR_BANK_MAP`, `CHR_BANK_SPRITES`
  - Convenience casts to avoid cc65 signedness warnings: `K_PAL`, `K_STR`, etc.


## Acknowledgements & Thanks

- **Shiru**: `lib/neslib.*` (NES library). Thank you for making this foundational work available.
- **Doug Fraker**: `lib/nesdoug.*` (helpers + VRAM buffer/metatile workflow + docs). 


## License

All code is free to public, have fun! We are building on NESLib + NESDoug's awesome work. Map data + graphics are not to be used for commercial use as they are attached to my Ninjaturdle game. Feel free to use as you wish for play/learn!