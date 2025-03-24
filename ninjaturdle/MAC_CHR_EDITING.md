# CHR Editing Options for Mac

Since you're on a Mac and need a way to edit/create CHR files, here are your best options:

## 1. Python Image to CHR Conversion

You already have the `png2chr` tool, but we can create a more flexible Python script that:
- Takes any input image
- Handles color indexing and constraints
- Outputs a properly formatted CHR file

### Basic Python CHR Converter

```python
#!/usr/bin/env python3
import sys
from PIL import Image
import numpy as np
import os

def convert_image_to_chr(input_path, output_path):
    """Convert an image to NES CHR format."""
    try:
        # Open the image
        img = Image.open(input_path)
        
        # Convert to indexed mode with max 4 colors if not already
        if img.mode != 'P':
            img = img.convert('P', palette=Image.ADAPTIVE, colors=4)
        
        # Get width and height
        width, height = img.size
        
        # Make sure dimensions are multiples of 8
        if width % 8 != 0 or height % 8 != 0:
            print(f"Warning: Image dimensions ({width}x{height}) not divisible by 8")
            print("Cropping to nearest multiple of 8")
            width = width - (width % 8)
            height = height - (height % 8)
            img = img.crop((0, 0, width, height))
        
        # Convert image data to numpy array
        data = np.array(img)
        
        # Calculate number of tiles
        num_tiles_x = width // 8
        num_tiles_y = height // 8
        
        # Prepare output file
        with open(output_path, 'wb') as f:
            # Process each 8x8 tile
            for ty in range(num_tiles_y):
                for tx in range(num_tiles_x):
                    # Extract the current 8x8 tile
                    tile = data[ty*8:(ty+1)*8, tx*8:(tx+1)*8]
                    
                    # Process each row in the tile
                    bit_plane1 = bytearray(8)
                    bit_plane2 = bytearray(8)
                    
                    for y in range(8):
                        # Process each pixel in the row
                        for x in range(8):
                            pixel_value = tile[y, x] & 3  # Ensure value is 0-3
                            
                            # Set bits in the two bit planes
                            if pixel_value & 1:  # If bit 0 is set
                                bit_plane1[y] |= (1 << (7 - x))
                            if pixel_value & 2:  # If bit 1 is set
                                bit_plane2[y] |= (1 << (7 - x))
                    
                    # Write the two bit planes to the file
                    f.write(bit_plane1)
                    f.write(bit_plane2)
        
        total_tiles = num_tiles_x * num_tiles_y
        print(f"Converted {total_tiles} tiles ({width}x{height} pixels)")
        print(f"Output written to {output_path}")
        return True
        
    except Exception as e:
        print(f"Error: {e}")
        return False

def main():
    if len(sys.argv) < 3:
        print("Usage: python image_to_chr.py input.png output.chr")
        return 1
    
    input_path = sys.argv[1]
    output_path = sys.argv[2]
    
    if not os.path.exists(input_path):
        print(f"Error: Input file {input_path} not found")
        return 1
    
    success = convert_image_to_chr(input_path, output_path)
    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())
```

Save this as `image_to_chr.py` and use:
```bash
python3 image_to_chr.py your_sprites.png output.chr
```

## 2. Wine + NES Screen Tool

NES Screen Tool runs well under Wine on Mac:

1. Install Wine:
   ```bash
   brew install --cask wine-stable
   ```

2. Download NES Screen Tool and run it:
   ```bash
   wine path/to/nesst.exe
   ```

## 3. Browser-Based CHR Editors

Several browser-based NES CHR editors work on any platform:

1. [NES CHR Editor](https://eonarheim.github.io/NES-CHR-Editor/) - Simple but functional
2. [Tiled](https://www.mapeditor.org/) - More full-featured tilemap editor that can handle NES tiles

## 4. Native Mac Alternatives

1. [Aseprite](https://www.aseprite.org/) ($) - Excellent sprite editor with export scripts
2. [Pixen](https://pixenapp.com/) - Mac-native pixel art editor

## Creating a Custom Workflow

For best results without specialized tools:

1. Create sprites in your favorite image editor
2. Save as indexed PNG with 4 colors max
3. Use the Python script to convert to CHR
4. Replace specific tiles in the original CHR file:

```python
def replace_character_in_chr(original_chr, new_chr, output_chr):
    # Copy the original CHR file
    with open(original_chr, 'rb') as f:
        data = bytearray(f.read())
    
    # Read the new character data
    with open(new_chr, 'rb') as f:
        new_data = f.read()
    
    # Replace the player character sprites (first 4 tiles and tiles 16-19)
    # Each tile is 16 bytes (8 bytes per bitplane)
    data[0:64] = new_data[0:64]       # Top half (tiles 0-3)
    data[256:320] = new_data[64:128]  # Bottom half (tiles 16-19)
    
    # Write the modified data
    with open(output_chr, 'wb') as f:
        f.write(data)
```