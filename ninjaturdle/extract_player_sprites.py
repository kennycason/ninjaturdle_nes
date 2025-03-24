#!/usr/bin/env python3
from PIL import Image
import numpy as np
import os

def extract_player_sprites(input_file, output_file):
    """
    Extract just the player character sprites from the main sprite sheet
    and create a new CHR file with just those sprites.
    
    NES sprites are 8x8 pixels. The player character typically uses 
    the first few tiles of the CHR file.
    """
    print(f"Extracting player sprites from {input_file}")
    
    try:
        # Open the image
        img = Image.open(input_file)
        print(f"Image size: {img.size}")
        
        # For NES, we need to extract just the first few 8x8 tiles
        # Player sprites are typically at the top-left
        # Let's extract a 32x32 area (4x4 tiles) for the player
        player_area = img.crop((0, 0, 32, 32))
        
        # Save to a temporary file
        temp_png = "player_sprites_temp.png"
        player_area.save(temp_png)
        print(f"Saved temporary player sprite sheet to {temp_png}")
        
        # Convert to CHR using the png2chr tool
        os.system(f"../tools/png2chr {temp_png} {output_file}")
        
        # Clean up temporary file
        os.remove(temp_png)
        
        print(f"Successfully created {output_file}")
    except Exception as e:
        print(f"Error processing image: {e}")

if __name__ == "__main__":
    # Extract just the player sprites to a new CHR file
    extract_player_sprites("BG/Sprites.png", "player.chr")