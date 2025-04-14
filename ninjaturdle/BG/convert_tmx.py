#!/usr/bin/env python3

import sys
import os
import re
import xml.etree.ElementTree as ET
import argparse

# Tile type constants - matching sprite sheet layout
TILE_HP_UP = 0x01     # First tile in top row (0,0)
TILE_CORN_UP = 0x02   # Second tile in top row (1,0)
TILE_ENEMY_WASP = 0x10   # First tile in second row
TILE_ENEMY_BOUNCE = 0x11  # Second tile in second row
TILE_ENEMY_BOSS1 = 0x1F   # Boss enemy

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

def process_tile_layer(layer, map_width, map_height):
    """Process tile layer data and convert to game format"""
    data = layer.find('data')
    if data is None or data.get('encoding') != 'csv':
        print("Error: Only CSV encoding is supported for tile layers")
        return []
        
    # Parse CSV data into 2D array
    tile_data = parse_csv_data(data.text)
    
    # Organize into 8 rooms of 16x15 tiles
    rooms = []
    for room in range(8):
        room_data = []
        start_x = room * 16
        for y in range(15):  # 15 rows per room
            row = tile_data[y][start_x:start_x + 16]
            room_data.extend([(x - 1) for x in row])  # Subtract 1 from tile IDs
        rooms.append(room_data)
    
    return rooms

def process_object_layer(layer, map_width, map_height):
    """Process object layer data and convert to game format"""
    object_data = []
    coin_data = []
    enemy_data = []
    enemies_per_room = [0] * 8  # Track number of enemies in each room
    total_enemies = 0
    
    # Get the layer data
    data = layer.find('data')
    if data is None or data.get('encoding') != 'csv':
        print("Error: Only CSV encoding is supported for object layers")
        return [], [], []
        
    # Parse CSV data into 2D array
    rows = [row.strip() for row in data.text.strip().split('\n')]
    tile_data = [[int(cell) for cell in row.split(',') if cell.strip()] for row in rows]
    
    # Process each tile in the layer
    for y in range(map_height):
        for x in range(map_width):
            tile_id = tile_data[y][x]
            if tile_id == 0:  # Skip empty tiles
                continue
                
            # Calculate room and position
            room = x // 16  # Each room is 16 tiles wide
            room_x = (x % 16) * 16  # X position within room in pixels
            y_pos = y * 16  # Y position in pixels
            
            # Map GIDs to game object types
            if tile_id == SPRITE_GID_HP_UP:  # HP Up
                object_data.extend([y_pos, room, room_x, TILE_HP_UP])
                coin_data.extend([y_pos, room, room_x, TILE_HP_UP])
                print(f"Added HP Up: Y=0x{y_pos:02x}, room={room}, X=0x{room_x:02x}, type=0x{TILE_HP_UP:02x}")
                
            elif tile_id == SPRITE_GID_CORN_UP:  # Corn Up
                object_data.extend([y_pos, room, room_x, TILE_CORN_UP])
                coin_data.extend([y_pos, room, room_x, TILE_CORN_UP])
                print(f"Added Corn Up: Y=0x{y_pos:02x}, room={room}, X=0x{room_x:02x}, type=0x{TILE_CORN_UP:02x}")
                
            elif tile_id in [SPRITE_GID_WASP, SPRITE_GID_BOUNCE, SPRITE_GID_BOSS] and total_enemies < 10:
                # Check enemy limit per room
                if enemies_per_room[room] >= 4:
                    print(f"Warning: Skipping enemy in room {room} - maximum 4 enemies per room")
                    continue
                    
                if tile_id == SPRITE_GID_WASP:  # Wasp enemy
                    obj_type = TILE_ENEMY_WASP
                elif tile_id == SPRITE_GID_BOUNCE:  # Bounce enemy
                    obj_type = TILE_ENEMY_BOUNCE
                elif tile_id == SPRITE_GID_BOSS:  # Boss enemy
                    obj_type = TILE_ENEMY_BOSS1
                else:
                    continue
                
                object_data.extend([y_pos, room, room_x, obj_type])
                enemy_data.extend([y_pos, room, room_x, obj_type])
                enemies_per_room[room] += 1
                total_enemies += 1
                print(f"Added enemy: Y=0x{y_pos:02x}, room={room}, X=0x{room_x:02x}, type=0x{obj_type:02x}")
    
    # Sort objects by room number for each array
    def sort_and_terminate(data):
        sorted_objects = []
        for i in range(0, len(data), 4):
            if i + 4 <= len(data):
                sorted_objects.append(data[i:i+4])
        sorted_objects.sort(key=lambda x: x[1])  # Sort by room number
        data = [x for obj in sorted_objects for x in obj]
        if not data:
            data = [0xff]  # Just add TURN_OFF marker if no objects
        elif data[-1] != 0xff:
            data.append(0xff)  # Add TURN_OFF marker
        return data
    
    object_data = sort_and_terminate(object_data)
    coin_data = sort_and_terminate(coin_data)
    enemy_data = sort_and_terminate(enemy_data)
    
    return object_data, coin_data, enemy_data

def convert_tmx(world, level):
    """Convert TMX file to C source code"""
    tmx_file = f'w{world}l{level}.tmx'
    output_file = f'w{world}l{level}.c'
    
    try:
        tree = ET.parse(tmx_file)
        root = tree.getroot()
        
        # Get map dimensions
        map_width = int(root.get('width'))
        map_height = int(root.get('height'))
        
        # Find the main tile layer and object layer
        main_layer = None
        object_layer = None
        
        for layer in root.findall('layer'):
            layer_name = layer.get('name', '').lower()
            print(f"Found layer: {layer_name}")
            if layer_name == 'main':
                main_layer = layer
            elif layer_name == 'object':
                object_layer = layer
                
        if main_layer is None:
            print(f"Error: Could not find 'main' layer in {tmx_file}")
            return
            
        # Process main layer data
        rooms = process_tile_layer(main_layer, map_width, map_height)
        
        # Process object layer data
        object_data = []
        coin_data = []
        enemy_data = []
        if object_layer is not None:
            print("Processing object layer...")
            object_data, coin_data, enemy_data = process_object_layer(object_layer, map_width, map_height)
        else:
            print(f"Warning: No object layer found in {tmx_file}")
            object_data = [0xff]  # Just add TURN_OFF marker if no objects
            coin_data = [0xff]
            enemy_data = [0xff]
            
        # Write output file
        with open(output_file, 'w') as f:
            f.write('// Generated by convert_tmx.py\n')
            f.write('#include "../ninjaturdle.h"\n\n')
            
            # Write main tile data
            for i in range(8):  # 8 rooms
                f.write(f'// Room {i} data\n')
                f.write(f'const unsigned char w{world}l{level}_main_{i}[] = {{\n')
                room_data = rooms[i]
                for j in range(0, len(room_data), 16):
                    f.write('    ' + ', '.join(f'0x{x:02x}' for x in room_data[j:j+16]) + ',\n')
                f.write('};\n\n')
            
            # Write main data list
            f.write(f'const unsigned char * const w{world}l{level}_main_list[] = {{\n')
            for i in range(8):
                f.write(f'    w{world}l{level}_main_{i},\n')
            f.write('};\n\n')
            
            # Write object data (Y, room, X, type)
            f.write('// Object data (Y, room, X, type)\n')
            f.write(f'const unsigned char w{world}l{level}_object[] = {{\n')
            for i in range(0, len(object_data), 4):
                if i + 4 <= len(object_data):
                    f.write(f'    0x{object_data[i]:02x}, 0x{object_data[i+1]:02x}, 0x{object_data[i+2]:02x}, 0x{object_data[i+3]:02x},\n')
            f.write('};\n\n')

            # Write coin/item data
            f.write('// Coin/Item data (Y, room, X, type)\n')
            f.write(f'const unsigned char w{world}l{level}_coins[] = {{\n')
            for i in range(0, len(coin_data), 4):
                if i + 4 <= len(coin_data):
                    f.write(f'    0x{coin_data[i]:02x}, 0x{coin_data[i+1]:02x}, 0x{coin_data[i+2]:02x}, 0x{coin_data[i+3]:02x},\n')
            f.write('};\n\n')

            # Write enemy data
            f.write('// Enemy data (Y, room, X, type)\n')
            f.write(f'const unsigned char w{world}l{level}_enemies[] = {{\n')
            for i in range(0, len(enemy_data), 4):
                if i + 4 <= len(enemy_data):
                    f.write(f'    0x{enemy_data[i]:02x}, 0x{enemy_data[i+1]:02x}, 0x{enemy_data[i+2]:02x}, 0x{enemy_data[i+3]:02x},\n')
            f.write('};\n')
            
        print(f"Successfully converted {tmx_file} to {output_file}")
        
    except FileNotFoundError:
        print(f"Error: Could not find {tmx_file}")
    except ET.ParseError:
        print(f"Error: Could not parse {tmx_file}")
    except Exception as e:
        print(f"Error: {str(e)}")

def main():
    if len(sys.argv) != 3:
        print("Usage: python convert_tmx.py <world> <level>")
        sys.exit(1)
    
    world = int(sys.argv[1])
    level = int(sys.argv[2])
    convert_tmx(world, level)

if __name__ == "__main__":
    main() 