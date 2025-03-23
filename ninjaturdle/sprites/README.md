# Sprite Implementation Instructions

## What's Currently Working

Right now, we've implemented a creative workaround to visually simulate a ninja character:

1. The base character is still the original round box character
2. A yellow coin is positioned on top to simulate a mask/headband
3. Parts of enemy sprites are positioned to simulate eyes
4. Animation is created by adjusting the positions of these elements

This is why you see the coin on top of the character's head - it's a visual hack using existing sprite tiles.

## How to Implement Your Custom Ninja Sprite

To properly use your ninja.png as the player character, follow these steps:

### Step 1: Prepare Your Sprite Image

1. Make sure your sprite fits within a 16x16 or 16x32 pixel grid
2. Each 8x8 section will become one NES tile
3. Use only 4 colors including transparent (NES limitation)

### Step 2: Convert to CHR Format

```bash
cd /Users/kennycason/code/nes/
tools/png2chr ninjaturdle/sprites/ninja.png ninja_sprite.chr
```

### Step 3: Create a Combined CHR File

This requires some binary file manipulation:

1. Copy the original CHR file
2. Replace specific tile sections with your ninja sprite tiles
3. Update the game to use this new CHR file

### Step 4: Alternative: Direct Tile Replacement

If binary file manipulation is difficult, try:

1. Edit your ninja.png to be exactly 16x16 pixels
2. Make 2 versions - one for left-facing, one for right-facing
3. Convert both to CHR format
4. Replace just the specific player sprite tiles in the CHR file

## Technical Details

The original player sprite uses these tile IDs:
- Right-facing: 0x00, 0x01, 0x10, 0x11 (top-left, top-right, bottom-left, bottom-right)
- Left-facing: 0x02, 0x03, 0x12, 0x13 (top-left, top-right, bottom-left, bottom-right)

When converting your ninja.png, make sure the CHR data replaces these specific tiles.