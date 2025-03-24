# Updating Sprites in Ninja Turdle

This guide explains how to update the sprites in your game.

## Updating Sprites.png

1. **Edit BG/Sprites.png**:
   - This file contains all sprite graphics for the game
   - Make your changes directly to this file
   - Keep the format: indexed color with 4 colors maximum
   - Maintain the 8x8 pixel grid alignment

2. **Convert to CHR Format**:
   There are two main ways to convert your sprites to CHR format:

   ### Option A: Use NES Screen Tool (Recommended)
   ```
   1. Open NES Screen Tool
   2. Load the NES_ST/sprites.nss project
   3. Import your updated Sprites.png (File > Import Bitmap)
   4. Export the CHR file (File > Export CHR)
   5. Save as ninjaturdle.chr in the main directory
   ```

   ### Option B: Use png2chr Tool
   ```bash
   ../tools/png2chr BG/Sprites.png ninjaturdle.chr
   ```

3. **Build the Game**:
   ```bash
   make
   ```

4. **Run the Game**:
   ```bash
   fceux ninjaturdle.nes
   ```

## Understanding Sprite Positions

The sprite sheet is organized as follows:
- Player character: Top left sprites (first 2x2 grid)
- Enemies: Adjacent to player sprites
- Coins and collectibles: Lower section

### Player Sprite Tiles
- Right-facing: Tiles 0x00-0x03 (top) and 0x10-0x13 (bottom)
- Left-facing: Tiles 0x02-0x03, 0x12-0x13 (flipped versions)

### Important Notes
1. Keep the same dimensions for each sprite
2. Maintain the color palette limitations
3. Make sure your changes align with the collision boxes in the code
4. The BG/Sprites.png file must be indexed color (not RGB)

## Advanced: Metasprite Structure

If you change sprite designs significantly, you may need to update the metasprite definitions in Sprites.h:

```c
const unsigned char RoundSprL[]={
    0xff, 0xfc, 0x02, 0,  // x_offset, y_offset, tile_id, attribute
    0x07, 0xfc, 0x03, 0,
    0xff, 0x04, 0x12, 0,
    0x07, 0x04, 0x13, 0,
    128
};
```

This defines how individual 8x8 tiles are combined into a full sprite.