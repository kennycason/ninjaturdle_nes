#!/usr/bin/env python3
import os
import numpy as np
from PIL import Image
import argparse

def read_chr_file(filename):
    """Read a CHR file and return its contents as bytes."""
    with open(filename, 'rb') as f:
        return f.read()

def extract_tile(chr_data, tile_index):
    """
    Extract an 8x8 tile from CHR data.
    
    NES CHR files store tiles as 16 bytes:
    - First 8 bytes: bitplane 0 (low bit of color)
    - Next 8 bytes: bitplane 1 (high bit of color)
    
    Each row is one byte in each bitplane.
    """
    if tile_index * 16 + 16 > len(chr_data):
        return None
    
    # Get the 16 bytes for this tile
    offset = tile_index * 16
    tile_data = chr_data[offset:offset+16]
    
    # Create an 8x8 array filled with zeros
    tile = np.zeros((8, 8), dtype=np.uint8)
    
    # Process each row
    for y in range(8):
        # Get the two bytes for this row (one from each bitplane)
        bp0 = tile_data[y]
        bp1 = tile_data[y+8]
        
        # Process each pixel in the row
        for x in range(8):
            # Get the bit from each bitplane (most significant bit first)
            bit0 = (bp0 >> (7-x)) & 1
            bit1 = (bp1 >> (7-x)) & 1
            
            # Combine bits to get the color index (0-3)
            color = (bit1 << 1) | bit0
            tile[y, x] = color
    
    return tile

def save_tile_as_png(tile, output_path):
    """
    Save a tile as a PNG image.
    
    Colors:
    0 - Transparent (0, 0, 0, 0)
    1 - Light gray   (192, 192, 192, 255)
    2 - Dark gray    (96, 96, 96, 255)
    3 - Black        (0, 0, 0, 255)
    """
    if tile is None:
        return False
    
    # Define the color palette (RGBA)
    palette = [
        (0, 0, 0, 0),           # 0: Transparent
        (192, 192, 192, 255),    # 1: Light gray
        (96, 96, 96, 255),       # 2: Dark gray
        (0, 0, 0, 255)           # 3: Black
    ]
    
    # Create a new RGB image
    img = Image.new('RGBA', (8, 8))
    pixels = img.load()
    
    # Set the pixels
    for y in range(8):
        for x in range(8):
            pixels[x, y] = palette[tile[y, x]]
    
    # Save the image
    img = img.resize((32, 32), Image.NEAREST)  # Scale up for better visibility
    img.save(output_path)
    return True

def extract_all_tiles(chr_filename, output_dir):
    """Extract all tiles from a CHR file and save them as PNG images."""
    # Create output directory if it doesn't exist
    os.makedirs(output_dir, exist_ok=True)
    
    # Read the CHR file
    try:
        chr_data = read_chr_file(chr_filename)
    except FileNotFoundError:
        print(f"Error: CHR file '{chr_filename}' not found.")
        return
    
    # Calculate number of tiles
    num_tiles = len(chr_data) // 16
    print(f"Found {num_tiles} tiles in {chr_filename}")
    
    # Extract and save each tile
    for i in range(num_tiles):
        tile = extract_tile(chr_data, i)
        output_path = os.path.join(output_dir, f"tile_{i:03d}.png")
        if save_tile_as_png(tile, output_path):
            print(f"Saved tile {i} to {output_path}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Extract tiles from NES CHR file.')
    parser.add_argument('chr_file', help='Path to the CHR file')
    parser.add_argument('output_dir', help='Directory to save the extracted tiles')
    
    args = parser.parse_args()
    extract_all_tiles(args.chr_file, args.output_dir)
    print("Extraction complete!")