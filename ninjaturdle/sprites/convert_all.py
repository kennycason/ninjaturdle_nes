#!/usr/bin/env python3
# A script to convert PNG sprites to NES CHR format and optionally combine them

import os
import sys
import subprocess
from pathlib import Path

# Directory paths
CURRENT_DIR = Path(os.path.dirname(os.path.realpath(__file__)))
PROJECT_DIR = CURRENT_DIR.parent
TOOLS_DIR = PROJECT_DIR.parent / "tools"
OUTPUT_DIR = PROJECT_DIR / "chr_output"

# Ensure output directory exists
os.makedirs(OUTPUT_DIR, exist_ok=True)

# Sprite definitions
SPRITES = [
    {"file": "masks.png", "width": 23, "height": 72, "cols": 1, "rows": 8},
    {"file": "mouth.png", "width": 7, "height": 12, "cols": 1, "rows": 3},
    {"file": "eyes.png", "width": 9, "height": 12, "cols": 1, "rows": 4},
    {"file": "ninja_walking_sheet.png", "width": 72, "height": 32, "cols": 3, "rows": 1},
    {"file": "rolling.png", "width": 20, "height": 20, "cols": 1, "rows": 1},
]

def main():
    print(f"Converting sprites...")
    
    # Check if png2chr exists
    png2chr_path = TOOLS_DIR / "png2chr"
    if not png2chr_path.exists():
        print(f"ERROR: png2chr tool not found at {png2chr_path}")
        print("Please compile it first")
        return 1
    
    # Process each sprite
    for sprite in SPRITES:
        input_file = CURRENT_DIR / sprite["file"]
        output_file = OUTPUT_DIR / f"{Path(sprite['file']).stem}.chr"
        
        print(f"Converting {input_file.name} to CHR format...")
        
        # Run png2chr
        cmd = [str(png2chr_path), str(input_file), str(output_file)]
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        if result.returncode != 0:
            print(f"ERROR: Failed to convert {input_file}")
            print(result.stderr)
            return 1
        
        print(f"✅ Created {output_file.name}")
    
    print("\nSprite conversion complete!")
    print(f"CHR files are in {OUTPUT_DIR}")
    return 0

if __name__ == "__main__":
    sys.exit(main())