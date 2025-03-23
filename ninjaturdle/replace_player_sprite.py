#!/usr/bin/env python3
"""
This script replaces the player character tiles in an NES CHR file with
tiles from a ninja sprite CHR file. It preserves all other game tiles.

Usage:
    python replace_player_sprite.py [source_chr] [ninja_chr] [output_chr]

Arguments:
    source_chr - Original CHR file with all game tiles (default: ninjaturdle.chr)
    ninja_chr - CHR file containing ninja sprite tiles (default: sprites/ninja.chr)
    output_chr - Output CHR file (default: ninjaturdle_new.chr)
"""

import os
import sys
import shutil
from pathlib import Path

def replace_player_tiles(source_chr, ninja_chr, output_chr):
    """
    Replace player character tiles in source_chr with tiles from ninja_chr
    and save to output_chr.
    """
    # Check if files exist
    if not os.path.exists(source_chr):
        print(f"ERROR: Source CHR file not found: {source_chr}")
        return False
    
    if not os.path.exists(ninja_chr):
        print(f"ERROR: Ninja CHR file not found: {ninja_chr}")
        return False
    
    # Read source CHR data
    with open(source_chr, 'rb') as f:
        source_data = f.read()
    
    # Read ninja CHR data
    with open(ninja_chr, 'rb') as f:
        ninja_data = f.read()
    
    # Check sizes
    source_size = len(source_data)
    ninja_size = len(ninja_data)
    
    print(f"Source CHR size: {source_size} bytes")
    print(f"Ninja CHR size: {ninja_size} bytes")
    
    # Create output CHR file (copy of source)
    shutil.copy2(source_chr, output_chr)
    
    # Calculate how many sprite tiles we'll replace
    # The player character typically uses tiles 0x00-0x03 (top) and 0x10-0x13 (bottom)
    # Each tile is 16 bytes (8 bytes for each bit plane)
    
    # For a 24x32 character, we need:
    # - 3 tiles horizontally (24 pixels / 8 pixels per tile)
    # - 4 tiles vertically (32 pixels / 8 pixels per tile)
    # That's a total of 12 tiles

    # Determine how many tiles to replace based on ninja CHR size
    # (limited by the smaller of ninja tiles or standard player tiles)
    max_tiles = min(ninja_size // 16, 12)  # 16 bytes per tile
    
    print(f"Replacing up to {max_tiles} tiles with ninja sprite tiles")
    
    with open(output_chr, 'r+b') as f:
        # Replace tiles for player character
        # First row: tiles 0x00-0x02 (3 tiles)
        f.seek(0)
        f.write(ninja_data[0:48])  # 3 tiles * 16 bytes
        
        # Second row: tiles 0x10-0x12 (3 tiles)
        f.seek(16 * 16)  # Starting at tile 0x10 (16)
        f.write(ninja_data[48:96])  # Next 3 tiles
        
        # Third row: tiles 0x20-0x22 (3 tiles)
        f.seek(32 * 16)  # Starting at tile 0x20 (32)
        f.write(ninja_data[96:144])  # Next 3 tiles
        
        # Fourth row: tiles 0x30-0x32 (3 tiles)
        f.seek(48 * 16)  # Starting at tile 0x30 (48)
        f.write(ninja_data[144:192])  # Next 3 tiles
    
    print(f"Player character tiles successfully replaced in {output_chr}")
    return True

def main():
    # Get arguments or use defaults
    source_chr = "ninjaturdle.chr"
    ninja_chr = "sprites/ninja.chr"
    output_chr = "ninjaturdle_new.chr"
    
    if len(sys.argv) > 1:
        source_chr = sys.argv[1]
    if len(sys.argv) > 2:
        ninja_chr = sys.argv[2]
    if len(sys.argv) > 3:
        output_chr = sys.argv[3]
    
    print(f"Source CHR: {source_chr}")
    print(f"Ninja CHR: {ninja_chr}")
    print(f"Output CHR: {output_chr}")
    
    success = replace_player_tiles(source_chr, ninja_chr, output_chr)
    
    if success:
        print("\nTo use this new CHR file:")
        print("1. Rename it to ninjaturdle.chr (if needed)")
        print("2. Make sure crt0.s is set to include 'ninjaturdle.chr'")
        print("3. Run 'make' to build the game")
        return 0
    else:
        return 1

if __name__ == "__main__":
    sys.exit(main())