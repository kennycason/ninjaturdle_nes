# Ninja Sprite Implementation

I've created a simple approach to implement ninja sprites in the game using the existing tile set. The implementation adds:

1. A mask (using a coin sprite)
2. Eyes (using parts of the enemy sprite)
3. Walking animation frames

## Files Changed

1. **NinjaTurdleSprites.h** - Contains new sprite definitions for:
   - NinjaSprR/NinjaSprL - Basic ninja sprites with mask and eyes
   - NinjaWalkR1/R2/L1/L2 - Walking animation frames

2. **ninjaturdle.c** - Updated to:
   - Use the new sprite definitions 
   - Add simple walking animation
   - Switch between standing and walking frames

## How the Sprites Work

The ninja sprites are built by combining:
- The original player character sprite as the base
- A coin sprite (tile 0x20) positioned as a "mask"
- Parts of enemy sprites (tiles 0x04/0x05) used as "eyes"

Walking animation is achieved by slightly changing the positions of the sprite components between frames.

## How to Test/Build

1. Make sure the game builds successfully:
   ```bash
   cd /Users/kennycason/code/nes/ninjaturdle
   make
   ```

2. Run the game:
   ```bash
   fceux ninjaturdle.nes
   ```

## Next Steps

1. **Better Walking Animation**: Create more varied frames for walking
2. **Jumping Animation**: Add custom frames for jumping
3. **CHR Customization**: Create a true custom CHR file with your ninja sprites
4. **Special Abilities**: Add special moves like rolling and throwing stars

## Advanced Implementation (Future)

To implement fully custom sprites, you'll need to:

1. Convert your PNG sprites to CHR format:
   ```bash
   ../tools/png2chr sprites/ninja.png ninja.chr
   ```

2. Create a CHR combination tool to merge your custom sprites with the original game tiles

3. Update crt0.s to use your custom CHR file:
   ```asm
   .segment "CHARS"
      .incbin "ninja_custom.chr"
   ```

4. Create proper metasprite definitions that reference your custom tile IDs