# Ninja Turdle Sprite Debug Guide

## Current Implementation
We're currently trying to use the existing sprite tiles but with modified metasprite definitions in `NinjaTurdleSprites.h`.

## Common Sprite Issues
1. **Incorrect tile IDs**: The tile IDs in your metasprite definitions might not match what's in the CHR file
2. **Bank selection**: Make sure you're using the right bank for sprites (in this code it's bank 1)
3. **Coordinate issues**: The x/y offsets in metasprites might be incorrect
4. **Missing CHR data**: Your custom sprites might not be properly included in the CHR file

## Next Steps

1. **Test with original sprites**: First make sure the original player sprite works correctly
2. **Simplify customization**: Start with small modifications to the existing sprites
3. **CHR integration**: For custom sprites, you'll need to:
   - Create a custom CHR file that includes your turdle sprites
   - Update the crt0.s file to include your CHR data
   - Use the correct tile IDs in your metasprite definitions

## Custom CHR Workflow
```bash
# Convert PNGs to CHR
../tools/png2chr sprites/masks.png masks.chr
../tools/png2chr sprites/eyes.png eyes.chr

# Combine CHR files (will need a custom tool)
# catcart tool_name masks.chr eyes.chr mouth.chr ninjaturdle.chr

# Reference in crt0.s
# .incbin "ninjaturdle.chr"
```

## For Next Debug
- Check if our player sprite renders at all
- See if we can modify just the left/right animation
- Try a simpler approach to introduce custom sprites