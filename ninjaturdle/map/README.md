# Background and Level Data

This directory contains the background and level data for Ninja Turdle, along with the scripts used to process this data.

## Overview

The game uses a tile-based approach for level design. Levels are created in the Tiled map editor (`.tmx` files) and then directly converted to C code (`.c` files) for inclusion in the game.

Each level consists of two layers:
1. Main layer - Background and collision tiles
2. Object layer - Special objects and sprites

## Level Design Structure

- Main layer tiles are organized by columns:
  - Columns 0-3: Background tiles (no collision)
  - Columns 4-5: Platform tiles (can pass through from below)
  - Columns 6-7: Solid tiles (collision from all sides)

## Workflow

The workflow for creating and processing level data is simple:

1. Design levels in Tiled map editor:
   - Save as `.tmx` files with naming format `w{world}l{level}.tmx` (e.g., `w1l1.tmx`)
   - Use the correct firstgid values in tilesets
   - Ensure the level width is a multiple of 16 (for room divisions)

2. Convert TMX directly to C code using the Python script:
   ```bash
   python3 convert_tmx.py <world> <level>
   ```
   Example:
   ```bash
   python3 convert_tmx.py 1 1  # Converts w1l1.tmx
   ```

## File Types

### TMX Files
- `w{world}l{level}.tmx` (e.g., `w1l1.tmx`)
- These are Tiled Map Editor files containing the complete level designs
- They define both the main layer (tiles) and object layer (sprites)

### C Files (Generated)
- `w{world}l{level}_main.c` - Compiled background tile data
- `w{world}l{level}_object.c` - Compiled object/sprite data

### Image Assets
- `ninjaturdle2.png` - Tileset image containing the background tiles
- `Sprites.png` - Image containing the game sprites

## Generated Code Format

### Main Layer Format
The main layer C files contain arrays of tile indices divided into 16-tile wide rooms:
```c
const unsigned char w1l1_main_0[] = { /* First room data */ };
const unsigned char w1l1_main_1[] = { /* Second room data */ };
// ... more rooms ...
const unsigned char * const w1l1_main_list[] = { /* Room pointer list */ };
```

### Object Layer Format
The object layer C files use the following format:
```c
// Y, room, X, object #
0x40, 2, 0x30, obj1,  // Example object placement
TURN_OFF
```
Where:
- Y: Vertical position (in hex)
- room: Room/screen number
- X: Horizontal position in room (in hex)
- object #: ID of the object to place

## Dependencies

To use the conversion script, you need:
```bash
pip install pytmx
```

## Notes

- The script automatically handles the conversion from Tiled's 1-based indices to 0-based indices required by the game
- Make sure your TMX files have the correct layer names ("main" and "object")
- The level width must be a multiple of 16 tiles for proper room division