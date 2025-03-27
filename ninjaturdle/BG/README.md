# Background and Level Data

This directory contains the background and level data for Ninja Turdle, along with the scripts used to process this data.

## Overview

The game uses a tile-based approach for level design. Levels are first created in the Tiled map editor (`.tmx` files), then exported to CSV format (`.csv` files), and finally converted to C code (`.c` files) for inclusion in the game.

There are two types of level data:
1. Background tiles (regular `.c` and `.csv` files)
2. Special objects/sprites (files with `_SP` suffix)

## Workflow

The workflow for creating and processing level data is as follows:

1. Design levels in Tiled map editor, saving as `.tmx` files
2. Export level data from Tiled to `.csv` format
3. Convert `.csv` files to C code using the Python conversion scripts:
   - `CSV2C_BIG.py` for background tiles
   - `CSV2C_SP.py` for special objects/sprites

## File Types

### TMX Files
- `Level1.tmx`, `Level2.tmx`, `Level3.tmx`
- These are Tiled Map Editor files containing the complete level designs
- They define the layout of tiles and objects in each level

### CSV Files
- `Level1.csv`, `Level2.csv`, `Level3.csv` - Background tile data
- `Level1_SP.csv`, `Level2_SP.csv`, `Level3_SP.csv` - Special object/sprite data
- These are exported from Tiled and contain numerical representations of tiles/objects

### C Files
- `Level1.c`, `Level2.c`, `Level3.c` - Compiled background tile data
- `Level1_SP.c`, `Level2_SP.c`, `Level3_SP.c` - Compiled special object/sprite data
- These contain the level data in a format that can be included in the game code

### Python Scripts
- `CSV2C_BIG.py` - Converts background tile CSV data to C code
- `CSV2C_SP.py` - Converts special object/sprite CSV data to C code

### Image Assets
- `Metatiles.png` - Tileset image containing the background tiles
- `Sprites.png` - Image containing the game sprites

## Special Object Format

The special object files (`_SP` suffix) use the following format:

```
// Y, room, X, object #
```

Where:
- Y: Vertical position
- room: Room/screen number
- X: Horizontal position
- object #: ID of the object to place

## Background Tile Format

The background tile files contain arrays of tile indices that define the level layout. Each level is divided into rooms/screens, and each room consists of a grid of tiles.

## Usage

To update level data after making changes in Tiled:

1. Export the level from Tiled to CSV format
2. Run the appropriate conversion script:
   ```
   python CSV2C_BIG.py
   python CSV2C_SP.py
   ```
3. The C files will be updated with the new level data