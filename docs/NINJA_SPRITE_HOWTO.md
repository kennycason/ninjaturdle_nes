# How to Replace the Player Sprite with Ninja

I've created a simple approach to replace just the player character with your ninja sprite while preserving all other game elements. Here's how it works:

## How the Sprite System Works

In NES games:
- Graphics are stored in CHR (Character) files
- Each 8x8 pixel tile takes up 16 bytes in the CHR file
- The player character typically uses:
  - Tiles 0x00-0x02 for top row (right-facing)
  - Tiles 0x10-0x12 for second row
  - Tiles 0x20-0x22 for third row
  - Tiles 0x30-0x32 for fourth row

## Implementation Steps

1. **Create your ninja sprite PNG**:
   - Make it 24x32 pixels (3 tiles wide, 4 tiles tall)
   - Use indexed color mode with 4 colors max
   - Color 0 should be transparent

2. **Convert PNG to CHR**:
   ```bash
   # Only needed if you don't already have a ninja.chr file
   cd sprites
   ../tools/png2chr ninja.png ninja.chr
   ```

3. **Replace player tiles in the game's CHR file**:
   ```bash
   python3 replace_player_sprite.py
   ```
   This script:
   - Takes the original ninjaturdle.chr
   - Replaces just the player character tiles with your ninja
   - Preserves all other game tiles (enemies, platforms, etc.)
   - Creates ninjaturdle_new.chr

4. **Build the game**:
   ```bash
   # This script automates steps 3-4
   ./build_ninja.sh
   ```

## What Changes Were Made

1. Created `replace_player_sprite.py` - Replaces player tiles in the CHR
2. Created `build_ninja.sh` - Automates the build process

## Understanding the CHR Layout

- Your 24x32 ninja sprite is arranged like this in the CHR:
  ```
  [0x00][0x01][0x02]  <- Top row (first 3 tiles)
  [0x10][0x11][0x12]  <- Second row
  [0x20][0x21][0x22]  <- Third row
  [0x30][0x31][0x32]  <- Bottom row (last 3 tiles)
  ```

- The sprite data in ninjaturdle.c references these tiles
- By replacing just these specific tiles, your ninja appears in place of the player

## Advanced Customizations

Once this basic approach is working, you can:
1. Add more animation frames
2. Customize the sprite for different actions (jumping, etc.)
3. Add special ninja abilities