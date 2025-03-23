# Ninja Turdle Development Guide

## Build Commands
```
make clean    # Clean build artifacts
make          # Build the game
```

## Code Style
- Variable declarations at top of functions (cc65 requirement)
- Use const for read-only data
- Comment complex logic
- Follow NES memory constraints (2KB RAM)
- Maintain separate concerns (input, physics, rendering)

## Sprite Information

### Custom Sprites
- **masks.png**: 23x72 pixels, 8 sprites in 1 column, 8 rows
- **mouth.png**: 7x12 pixels, 3 sprites in 1 column, 3 rows
- **eyes.png**: 9x12 pixels, 4 sprites in 1 column, 4 rows
- **ninja_walking_sheet.png**: 72x32 pixels, 3 sprites in 3 columns, 1 row
- **rolling.png**: 20x20 pixels, 1 sprite

### Sprite Conversion
Use the png2chr tool to convert PNG sprites to NES CHR format:
```
../tools/png2chr sprites/masks.png > masks.chr
```

Then include the CHR data in your ROM using assembly or C headers.