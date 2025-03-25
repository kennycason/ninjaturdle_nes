# NES Sprite Creation Guide

## Image Format Requirements

PNG files for NES sprites must meet these requirements:

1. **Dimensions**: Must be multiples of 8 pixels (e.g., 8x8, 16x16, 24x32)
2. **Color Mode**: Must be in indexed color mode (paletted PNG)
3. **Color Count**: Must have exactly 4 colors including transparent
4. **Layout**: Should be arranged in 8x8 pixel tiles

## Converting Images for NES

### Using Image Editors:

1. **GIMP**:
   - Go to Image → Mode → Indexed
   - Select "Use custom palette" with maximum 4 colors
   - Make sure one color is used for transparency
   - Export as PNG

2. **Photoshop**:
   - Go to Image → Mode → Indexed Color
   - Set Colors to 4
   - Set Transparency to "Preserve"
   - Save as PNG-8

3. **Aseprite**:
   - Go to Sprite → Color Mode → Indexed
   - Limit to 4 colors
   - Export as PNG

### Using NES-Specific Tools:

1. **NES Screen Tool** (recommended):
   - Can directly create and edit CHR files
   - Available at: https://shiru.untergrund.net/software.shtml

2. **YY-CHR**:
   - CHR editor with support for many systems
   - Available at: https://www.romhacking.net/utilities/119/

## NES Sprite Technical Details

### Tile Structure:
- Each tile is 8x8 pixels
- Each pixel can be one of 4 colors (2 bits per pixel)
- Stored as two bit planes (total 16 bytes per tile)

### Metasprite Structure:
In metasprite definitions (like in sprites.h):
```c
const unsigned char RoundSprR[]={
    0xff, 0xfc, 0x00, 0,  // x_offset, y_offset, tile_id, attribute
    0x07, 0xfc, 0x01, 0,
    0xff, 0x04, 0x10, 0,
    0x07, 0x04, 0x11, 0,
    128  // Terminator
};
```

- **x_offset**: Horizontal offset from sprite position (-128 to 127)
- **y_offset**: Vertical offset from sprite position (-128 to 127)
- **tile_id**: Which tile to use (0x00 to 0xFF)
- **attribute**: Palette and flip options

### Metasprite Usage:
```c
oam_meta_spr(x, y, metasprite_definition);
```

## Recommended Workflow

1. Create sprite art in 8x8 tile grid
2. Limit to 4 colors with one being transparent
3. Export as indexed color PNG
4. Convert to CHR using png2chr
5. Create combined CHR file
6. Update metasprite definitions if needed
7. Test in emulator

## Helpful Resources

- [NES Graphics Guide](https://nerdy-nights.nes.science/about/)
- [NES Dev Wiki](https://wiki.nesdev.org/w/index.php/PPU_sprites)
- [Shiru's NES Graphics Tutorial](http://shiru.untergrund.net/articles/programming_nes_games_in_c.htm)