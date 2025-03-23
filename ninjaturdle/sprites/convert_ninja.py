#!/usr/bin/env python3
# A script to convert the ninja_nes.png to CHR format and create a combined CHR file

import os
import sys
import subprocess
import shutil
from pathlib import Path
from PIL import Image

# Directory paths
CURRENT_DIR = Path(os.path.dirname(os.path.realpath(__file__)))
PROJECT_DIR = CURRENT_DIR.parent
TOOLS_DIR = PROJECT_DIR.parent / "tools"
OUTPUT_DIR = PROJECT_DIR / "chr_output"

# Ensure output directory exists
os.makedirs(OUTPUT_DIR, exist_ok=True)

def check_image_requirements(image_path):
    """Check if the image meets NES requirements"""
    try:
        img = Image.open(image_path)
        
        # Check if image is in indexed mode (paletted)
        if img.mode != 'P':
            return False, "Image is not in indexed color mode"
        
        # Check number of colors
        palette = img.getpalette()
        if palette:
            # Count unique colors actually used in the image
            colors_used = len(set(img.getdata()))
            if colors_used > 4:
                return False, f"Image uses {colors_used} colors, but NES sprites can only use 4 colors"
        
        # Check dimensions
        width, height = img.size
        if width % 8 != 0 or height % 8 != 0:
            return False, f"Image dimensions ({width}x{height}) are not multiples of 8"
            
        return True, "Image meets requirements"
    except Exception as e:
        return False, f"Error checking image: {str(e)}"

def main():
    print(f"Converting ninja sprite...")
    
    # Check if png2chr exists
    png2chr_path = TOOLS_DIR / "png2chr"
    if not png2chr_path.exists():
        print(f"ERROR: png2chr tool not found at {png2chr_path}")
        print("Please compile it first")
        return 1
    
    # Find the ninja sprite image
    ninja_png = CURRENT_DIR / "ninja_nes.png"
    if not ninja_png.exists():
        print(f"ERROR: Ninja sprite not found at {ninja_png}")
        return 1
    
    # Check if the image meets requirements
    valid, message = check_image_requirements(ninja_png)
    if not valid:
        print(f"ERROR: {message}")
        print("""
Your ninja_nes.png image needs to be in indexed color mode (paletted PNG)
with exactly 4 colors (including transparent).

To fix this:
1. Open the image in an image editor (like GIMP, Photoshop, or Aseprite)
2. Make sure the image dimensions are multiples of 8 (e.g., 16x16, 24x24)
3. Convert to indexed color mode with exactly 4 colors
4. Save as PNG

For NES, your palette should typically be:
- Color 0: Transparent (usually black with alpha=0)
- Color 1, 2, 3: Your sprite colors

Alternative: Use an NES-specific sprite editor like NES Screen Tool
or YY-CHR that can directly output CHR files.
        """)
        return 1
    
    # Convert the image to CHR format
    output_chr = OUTPUT_DIR / "ninja.chr"
    try:
        print(f"Converting {ninja_png} to CHR format...")
        result = subprocess.run(
            [str(png2chr_path), str(ninja_png), str(output_chr)],
            check=True,
            capture_output=True,
            text=True
        )
        print(f"Conversion successful! Output saved to {output_chr}")
        print(result.stdout)
        return 0
    except subprocess.CalledProcessError as e:
        print(f"ERROR: Conversion failed: {e}")
        print(e.stdout)
        print(e.stderr)
        return 1

if __name__ == "__main__":
    sys.exit(main())