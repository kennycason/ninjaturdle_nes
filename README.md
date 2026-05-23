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

Levels live in `map/level1.tmx`, `map/level2.tmx`, etc. The converter produces C data used by the game.

- **Convert a TMX**:

```bash
cd map
source venv/bin/activate
python3 map/convert_tmx.py 1
```

This overwrites `map/level1.c` (and similarly for other levels).

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


## Tile Ids for Object Layer

```
| sprites.png tile | GID | Object Type | CHR 8x8 tiles | Status |
|---|---|---|---|---|
| 0 | 129 | turd+2life | 0x30 | new, not wired |
| 1 | 130 | corn | 0x20 (8x8) | ✓ |
| 7 | 136 | exit marker | 0x40,41,50,51 | ✓ |
| 8 | 137 | wasp | 0x26,27,36,37 | ✓ |
| 9 | 138 | jumping monster | 0x04-07,14-17 | ✓ |
| 10 | 139 | worm | 0x08-0B,18-1B | ✓ |
| 11 | 140 | thorns | 0x0C-0F,1C-1F | ✓ |
| 13 | 142 | ring worm | 0x22,23,32,33 | ✓ |
| 14 | 143 | hop worm | 0x24,25,34,35 | ✓ |
| 15 | 144 | boss1 | 0x2C-2F,3C-3F,4C-4F,5C-5F | ✓ |
| 16 | 145 | spider | 0x82 (8x8) | check CHR |
| 17 | 146 | flying bug | 0x84,85,94,95 | ✓ |
| 18 | 147 | virus | 0x86 (8x8) | ✓ |
| 19 | 148 | scropion | 0x87 (8x8) | ✓ |
| 20 | 149 | poop monster | 0x28-2B,38-3B | ✓ |
| 23 | 152 | mother worm | - | sprite removed |
```


## Acknowledgements & Thanks

- **Shiru**: `lib/neslib.*` (NES library). Thank you for making this foundational work available.
- **Doug Fraker**: `lib/nesdoug.*` (helpers + VRAM buffer/metatile workflow + docs). 


## License

All code is free to public, have fun! We are building on NESLib + NESDoug's awesome work. Map data + graphics are not to be used for commercial use as they are attached to my Ninjaturdle game. Feel free to use as you wish for play/learn!