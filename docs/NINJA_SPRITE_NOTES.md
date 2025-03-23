# Ninja Sprite Implementation Notes

## Current Approach
We're using a simple approach to implement the ninja sprite:

1. Convert ninja.png to CHR format using png2chr
2. Create a combined CHR file:
   - Copy the original ninjaturdle.chr
   - Replace specific tiles (0x00-0x03, 0x10-0x13) with the ninja sprite tiles
3. Update crt0.s to use the combined CHR file
4. Use the original sprite references (RoundSprL/RoundSprR) to display the ninja

## How It Works
- The original player sprite uses tiles 0x00-0x03 for the top half (right-facing)
- And tiles 0x10-0x13 for the bottom half (right-facing)
- For left-facing, it uses tiles 0x02-0x03, 0x12-0x13 but flips them
- Our script replaces just these tiles with the ninja sprite tiles
- Rest of the CHR data (enemies, coins, etc.) remains unchanged

## Next Steps for Sprite Enhancement

1. **Animation Frames**:
   - Add walking animation frames
   - Add jumping/falling frames
   - Add special moves (rolling, throwing)

2. **Sprite Detail**:
   - Add mask options
   - Add expressions (eyes, mouth)
   - Improve ninja detailing within NES constraints

3. **Implementation Improvements**:
   - Create a more complete sprite replacement tool
   - Update metasprite definitions for custom animations
   - Handle flipping/mirroring correctly

## NES Sprite Limitations
- Each sprite can be 8x8 or 8x16 pixels
- 4 colors per sprite (one is transparent)
- 64 sprites maximum per frame
- 8 sprites maximum per scanline
- Each CHR bank typically holds 256 tiles (8x8)

## References
For editing metasprite definitions, see Sprites.h:
```c
const unsigned char RoundSprL[]={
      0xff,  0xfc,0x02,0,
      7,  0xfc,0x03,0,
      0xff,  4,0x12,0,
      7,  4,0x13,0,
    128
};
```
Format is: x_offset, y_offset, tile_id, attribute