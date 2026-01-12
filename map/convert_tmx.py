#!/usr/bin/env python3

import sys
import os
import re
import xml.etree.ElementTree as ET
import argparse

# Tile type constants - matching sprite sheet layout
TILE_HP_UP = 0x01     # First tile in top row (0,0)
TILE_CORN_UP = 0x02   # Second tile in top row (1,0)
TILE_ENEMY_WASP = 0x09    # Flying wasp
TILE_ENEMY_BOUNCE = 0x0A  # Bouncing enemy
TILE_ENEMY_WORM = 0x0B    # Worm enemy
TILE_ENEMY_THORNS = 0x0C  # Stationary thorns (toggle spikes)
TILE_ENEMY_BOSS1 = 0x10   # Boss enemy
TILE_ENEMY_BOSS2 = 0x20   # Boss2 enemy

# Sprite tileset GIDs (firstgid=129)
SPRITE_GID_HP_UP = 129      # 129 + 0 (first tile at 0,0)
SPRITE_GID_CORN_UP = 130    # 129 + 1 (second tile at 1,0)
SPRITE_GID_WASP = 137       # 129 + 8
SPRITE_GID_BOUNCE = 138     # 129 + 9
SPRITE_GID_WORM = 139       # 129 + 10
SPRITE_GID_THORNS = 140     # 129 + 11 (tile after worm)
SPRITE_GID_BOSS = 144       # 129 + 15
SPRITE_GID_BOSS2 = 152      # 129 + 8 + 15

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
    
    # Prefer an objectgroup named "object" with tile objects (supports per-object properties)
    object_group = root.find(".//objectgroup[@name='object']")
    if object_group is not None:
        for obj in object_group.findall('object'):
            gid = int(obj.get('gid', 0))
            if gid == 0:
                continue

            # Tiled stores pixel positions; convert to tile units
            x = int(float(obj.get('x', 0)) // 16)
            y = int(float(obj.get('y', 0)) // 16)

            delay_seconds = 4  # default to 4s when not provided
            props = obj.find('properties')
            if props is not None:
                for prop in props.findall('property'):
                    if prop.get('name') == 'delay':
                        try:
                            delay_seconds = int(float(prop.get('value', '0')))
                        except ValueError:
                            delay_seconds = 0

            if gid == SPRITE_GID_CORN_UP:
                print(f"Adding coin at ({x}, {y})")
                coin_data.append((x, y))
            elif gid == SPRITE_GID_WASP:
                print(f"Adding wasp at ({x}, {y})")
                enemy_data.append((x, y, TILE_ENEMY_WASP, 0))
            elif gid == SPRITE_GID_BOUNCE:
                print(f"Adding bounce enemy at ({x}, {y})")
                enemy_data.append((x, y, TILE_ENEMY_BOUNCE, 0))
            elif gid == SPRITE_GID_WORM:
                print(f"Adding worm enemy at ({x}, {y})")
                enemy_data.append((x, y, TILE_ENEMY_WORM, 0))
            elif gid == SPRITE_GID_THORNS:
                print(f"Adding thorns at ({x}, {y}) with delay {delay_seconds}s")
                enemy_data.append((x, y, TILE_ENEMY_THORNS, delay_seconds))
            elif gid == SPRITE_GID_BOSS:
                print(f"Adding boss at ({x}, {y})")
                enemy_data.append((x, y, TILE_ENEMY_BOSS1, 0))
            elif gid == SPRITE_GID_BOSS2:
                print(f"Adding boss2 at ({x}, {y})")
                enemy_data.append((x, y, TILE_ENEMY_BOSS2, 0))
            else:
                print(f"Unknown GID {gid} at ({x}, {y})")

        return coin_data, enemy_data

    # Fallback: legacy tile layer named "object" with CSV data (no properties)
    object_layer = root.find(".//layer[@name='object']")
    if object_layer is None:
        print("No object layer found")
        return coin_data, enemy_data
        
    data = object_layer.find('data')
    if data is None or data.get('encoding') != 'csv':
        print("Error: Object layer data not found or not in CSV format")
        return coin_data, enemy_data
        
    tile_data = parse_csv_data(data.text)
    height = len(tile_data)
    width = len(tile_data[0]) if height > 0 else 0
    
    for y in range(height):
        for x in range(width):
            gid = tile_data[y][x]
            if gid == 0:
                continue
                
            print(f"Found object with GID {gid} at ({x}, {y})")
            
            if gid == SPRITE_GID_CORN_UP:
                print(f"Adding coin at ({x}, {y})")
                coin_data.append((x, y))
            elif gid == SPRITE_GID_WASP:
                print(f"Adding wasp at ({x}, {y})")
                enemy_data.append((x, y, TILE_ENEMY_WASP, 0))
            elif gid == SPRITE_GID_BOUNCE:
                print(f"Adding bounce enemy at ({x}, {y})")
                enemy_data.append((x, y, TILE_ENEMY_BOUNCE, 0))
            elif gid == SPRITE_GID_WORM:
                print(f"Adding worm enemy at ({x}, {y})")
                enemy_data.append((x, y, TILE_ENEMY_WORM, 0))
            elif gid == SPRITE_GID_THORNS:
                print(f"Adding thorns at ({x}, {y})")
                enemy_data.append((x, y, TILE_ENEMY_THORNS, 4))  # default 4s delay in legacy mode
            elif gid == SPRITE_GID_BOSS:
                print(f"Adding boss at ({x}, {y})")
                enemy_data.append((x, y, TILE_ENEMY_BOSS1, 0))
            elif gid == SPRITE_GID_BOSS2:
                print(f"Adding boss2 at ({x}, {y})")
                enemy_data.append((x, y, TILE_ENEMY_BOSS2, 0))
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
            
            # Write enemy data (y, room, x, type, param)
            f.write(f'const uint8_t w{world}l{level}_enemies[] = {{\n')
            for x, y, enemy_type, param in enemy_data:
                param_byte = max(0, min(255, int(param)))
                f.write(f'    0x{(y*16):02x}, {x//16}, 0x{(x%16)*16:02x}, 0x{enemy_type:02x}, 0x{param_byte:02x},\n')
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