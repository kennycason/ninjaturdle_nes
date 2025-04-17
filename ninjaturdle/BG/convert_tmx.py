#!/usr/bin/env python3

import sys
import os
import re
import xml.etree.ElementTree as ET
import argparse

# Tile type constants - matching sprite sheet layout
TILE_HP_UP = 0x01     # First tile in top row (0,0)
TILE_CORN_UP = 0x02   # Second tile in top row (1,0)
TILE_ENEMY_WASP = 0x10   # Flying wasp (converted from 0x89)
TILE_ENEMY_BOUNCE = 0x11  # Bouncing enemy (converted from 0x8A)
TILE_ENEMY_BOSS1 = 0x1F   # Boss enemy (converted from 0x90)

# Sprite tileset GIDs (firstgid=129)
SPRITE_GID_HP_UP = 129      # 129 + 0 (first tile at 0,0)
SPRITE_GID_CORN_UP = 130    # 129 + 1 (second tile at 1,0)
SPRITE_GID_WASP = 137       # 129 + 8
SPRITE_GID_BOUNCE = 138     # 129 + 9
SPRITE_GID_BOSS = 144       # 129 + 15

def extract_world_level(filename):
    """Extract world and level numbers from filename."""
    match = re.match(r'w(\d+)l(\d+)', os.path.basename(filename))
    if match:
        return match.group(1), match.group(2)
    return None, None

def parse_csv_data(csv_text):
    """Parse CSV data from TMX file into a 2D list of integers."""
    rows = [row.strip() for row in csv_text.strip().split('\n')]
    return [[int(cell) for cell in row.split(',') if cell.strip()] for row in rows]

def process_layer(layer, room_num):
    """Process a single layer for a specific room"""
    # Get layer data
    data = layer.find('data')
    if data is None or data.get('encoding') != 'csv':
        print("Error: Only CSV encoding is supported for tile layers")
        return None
        
    # Parse CSV data into 2D array
    tile_data = parse_csv_data(data.text)
    height = len(tile_data)
    width = len(tile_data[0]) if height > 0 else 0
    
    # Extract room data
    room_data = []
    start_x = room_num * 16
    end_x = start_x + 16
    
    if start_x >= width:
        return None
        
    # Extract the 16x15 room section
    for y in range(min(15, height)):
        row = tile_data[y][start_x:end_x]
        if len(row) == 16:  # Only add complete rows
            # Subtract 1 from tile IDs (unless 0)
            room_data.extend([(x - 1) if x > 0 else 0 for x in row])
            
    return room_data

def process_object_layer(root):
    """Process object layer and extract coin/enemy data"""
    coin_data = []
    enemy_data = []
    
    # Find the object layer
    object_layer = root.find(".//layer[@name='object']")
    if object_layer is None:
        print("No object layer found")
        return coin_data, enemy_data
        
    # Get layer data
    data = object_layer.find('data')
    if data is None or data.get('encoding') != 'csv':
        print("Error: Object layer data not found or not in CSV format")
        return coin_data, enemy_data
        
    # Parse CSV data into 2D array
    tile_data = parse_csv_data(data.text)
    height = len(tile_data)
    width = len(tile_data[0]) if height > 0 else 0
    
    # Process each tile in the object layer
    for y in range(height):
        for x in range(width):
            gid = tile_data[y][x]
            if gid == 0:  # Skip empty tiles
                continue
                
            print(f"Found object with GID {gid} at ({x}, {y})")
            
            # Map GIDs to internal tile types - use exact matches
            if gid == SPRITE_GID_CORN_UP:
                print(f"Adding coin at ({x}, {y})")
                coin_data.append((x, y))
            # Enemy GIDs are completely different values
            elif gid == SPRITE_GID_WASP:
                print(f"Adding wasp at ({x}, {y})")
                enemy_data.append((x, y, TILE_ENEMY_WASP))
            elif gid == SPRITE_GID_BOUNCE:
                print(f"Adding bounce enemy at ({x}, {y})")
                enemy_data.append((x, y, TILE_ENEMY_BOUNCE))
            elif gid == SPRITE_GID_BOSS:
                print(f"Adding boss at ({x}, {y})")
                enemy_data.append((x, y, TILE_ENEMY_BOSS1))
            else:
                print(f"Unknown GID {gid} at ({x}, {y})")
            
    return coin_data, enemy_data

class TMXData:
    """Class to hold TMX file data"""
    def __init__(self, layers, width, height, root):
        self.layers = layers
        self.width = width
        self.height = height
        self.root = root

def load_tmx(tmx_file):
    """Load and parse a TMX file"""
    try:
        tree = ET.parse(tmx_file)
        root = tree.getroot()
        
        # Get map dimensions
        map_width = int(root.get('width'))
        map_height = int(root.get('height'))
        
        # Find layers
        layers = []
        for layer in root.findall('layer'):
            layer_name = layer.get('name', '').lower()
            print(f"Found layer: {layer_name}")
            layers.append(layer)
            
        if not layers:
            print(f"Error: No layers found in {tmx_file}")
            return None
            
        return TMXData(layers, map_width, map_height, root)
        
    except FileNotFoundError:
        print(f"Error: Could not find {tmx_file}")
    except ET.ParseError:
        print(f"Error: Could not parse {tmx_file}")
    except Exception as e:
        print(f"Error: {str(e)}")
    return None

def convert_tmx(tmx_file, output_file):
    """Convert TMX file to game format"""
    try:
        # Load and parse TMX file
        tmx_data = load_tmx(tmx_file)
        if not tmx_data:
            return False
            
        # Process background layer
        rooms = []
        main_layer = tmx_data.layers[0]  # First layer should be the main/background layer
        
        # Process each room (16x15 tile sections)
        for i in range(tmx_data.width // 16):
            room = process_layer(main_layer, i)
            if room:
                rooms.append(room)
                
        # Process object layer for coins and enemies
        coin_data, enemy_data = process_object_layer(tmx_data.root)
        
        # Get world and level from filename
        world, level = extract_world_level(tmx_file)
        if not world or not level:
            print("Error: Could not extract world and level from filename")
            return False
            
        # Generate output file
        with open(output_file, 'w') as f:
            # Write headers
            f.write('#include <stdint.h>\n\n')
            
            # Write room data
            for i, room in enumerate(rooms):
                f.write(f'const uint8_t w{world}l{level}_main_{i}[] = {{\n')
                # Write room data in rows of 16
                for row in range(15):
                    start = row * 16
                    row_data = room[start:start + 16]
                    f.write('    ' + ', '.join(f'0x{x:02x}' for x in row_data) + ',\n')
                f.write('};\n\n')
                
            # Write coin data
            f.write(f'const uint8_t w{world}l{level}_coins[] = {{\n')
            for x, y in coin_data:
                f.write(f'    0x{(y*16):02x}, {x//16}, 0x{(x%16)*16:02x}, 0x02,\n')  # 0x02 = COIN_REG
            f.write('    0xff  // End marker\n};\n\n')
            
            # Write enemy data  
            f.write(f'const uint8_t w{world}l{level}_enemies[] = {{\n')
            for x, y, enemy_type in enemy_data:
                f.write(f'    0x{(y*16):02x}, {x//16}, 0x{(x%16)*16:02x}, 0x{enemy_type:02x},\n')
            f.write('    0xff  // End marker\n};\n\n')
            
            # Write room pointers array
            f.write(f'const uint8_t* const w{world}l{level}_main[] = {{\n')
            for i in range(len(rooms)):
                f.write(f'    w{world}l{level}_main_{i},\n')
            f.write('};\n')
            
        print(f"Successfully converted {tmx_file} to {output_file}")
        return True
        
    except Exception as e:
        print(f"Error converting TMX file: {e}")
        return False

def main():
    if len(sys.argv) != 3:
        print("Usage: python convert_tmx.py <world> <level>")
        sys.exit(1)
    
    world = int(sys.argv[1])
    level = int(sys.argv[2])
    convert_tmx(f"w{world}l{level}.tmx", f"w{world}l{level}.c")

if __name__ == "__main__":
    main() 