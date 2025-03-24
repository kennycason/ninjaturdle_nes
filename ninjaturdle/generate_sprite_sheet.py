#!/usr/bin/env python3
import os
import math
import numpy as np
from PIL import Image, ImageDraw, ImageFont
import argparse

def create_sprite_sheet(input_dir, output_file, tiles_per_row=16):
    """
    Create a sprite sheet from individual tile images.
    
    Args:
        input_dir: Directory containing the tile PNG files
        output_file: Path to save the output sprite sheet
        tiles_per_row: Number of tiles to display per row
    """
    # Get all PNG files
    png_files = [f for f in os.listdir(input_dir) if f.endswith('.png')]
    png_files.sort()  # Sort to ensure tile order
    
    if not png_files:
        print(f"No PNG files found in {input_dir}")
        return
    
    # Load the first image to get tile dimensions
    first_tile = Image.open(os.path.join(input_dir, png_files[0]))
    tile_width, tile_height = first_tile.size
    
    num_tiles = len(png_files)
    num_rows = math.ceil(num_tiles / tiles_per_row)
    
    # Create the sprite sheet
    sheet_width = tiles_per_row * tile_width
    sheet_height = num_rows * tile_height
    
    sprite_sheet = Image.new('RGBA', (sheet_width, sheet_height), (0, 0, 0, 0))
    
    # Add tiles to the sprite sheet
    for i, png_file in enumerate(png_files):
        row = i // tiles_per_row
        col = i % tiles_per_row
        
        x = col * tile_width
        y = row * tile_height
        
        tile = Image.open(os.path.join(input_dir, png_file))
        sprite_sheet.paste(tile, (x, y))
    
    # Add tile indices as text
    sheet_with_indices = Image.new('RGBA', (sheet_width, sheet_height + 20), (0, 0, 0, 0))
    sheet_with_indices.paste(sprite_sheet, (0, 0))
    
    draw = ImageDraw.Draw(sheet_with_indices)
    
    # Draw row headers
    for row in range(num_rows):
        y = row * tile_height + tile_height // 2
        draw.text((2, y), f"{row*tiles_per_row:02X}", fill=(255, 255, 255, 255))
    
    # Save the sprite sheet
    sheet_with_indices.save(output_file)
    print(f"Sprite sheet saved to {output_file}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Create a sprite sheet from tile images.')
    parser.add_argument('input_dir', help='Directory containing tile PNG files')
    parser.add_argument('output_file', help='Path to save the sprite sheet')
    parser.add_argument('--tiles-per-row', type=int, default=16, help='Number of tiles per row')
    
    args = parser.parse_args()
    create_sprite_sheet(args.input_dir, args.output_file, args.tiles_per_row)