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

# Game engine limits - must match ninjaturdle.h
MAX_COINS = 16
MAX_ENEMY = 16

# Sprite tileset GIDs (firstgid=129)
SPRITE_GID_HP_UP = 129      # 129 + 0 (first tile at 0,0)
SPRITE_GID_CORN_UP = 130    # 129 + 1 (second tile at 1,0)
SPRITE_GID_EXIT = 136       # 129 + 7 (last tile in top row = level exit)
SPRITE_GID_WASP = 137       # 129 + 8
SPRITE_GID_BOUNCE = 138     # 129 + 9
SPRITE_GID_WORM = 139       # 129 + 10
SPRITE_GID_THORNS = 140     # 129 + 11 (tile after worm)
SPRITE_GID_BOSS = 144       # 129 + 15
SPRITE_GID_BOSS2 = 152      # 129 + 8 + 15


# Tiled flip flags (upper 3 bits of 32-bit GID)
TILED_FLIP_H    = 0x80000000
TILED_FLIP_V    = 0x40000000
TILED_FLIP_D    = 0x20000000
TILED_FLIP_MASK = 0xE0000000

def strip_flip_flags(gid):
    """Strip Tiled horizontal/vertical/diagonal flip flags from a GID."""
    return gid & ~TILED_FLIP_MASK

def extract_level_num(filename):
    """Extract level number from filename (levelN.tmx)."""
    match = re.match(r'level(\d+)', os.path.basename(filename))
    if match:
        return match.group(1)
    return None

def parse_csv_data(csv_text):
    """Parse CSV data from TMX file into a 2D list of integers."""
    rows = [row.strip() for row in csv_text.strip().split('\n') if row.strip()]
    return [[int(cell) for cell in row.split(',') if cell.strip()] for row in rows if row]

def process_layer(layer, room_num):
    """Process a single layer for a specific horizontal room (16-tile wide columns)"""
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
            # Strip flip flags, subtract 1 from tile IDs (unless 0)
            room_data.extend([(strip_flip_flags(x) - 1) if strip_flip_flags(x) > 0 else 0 for x in row])

    return room_data


def process_layer_vert(layer, room_num, pad_rows=0):
    """Process a single layer for a specific vertical room (15-tile tall rows).
    pad_rows: number of empty rows prepended to align total height to a multiple of 15."""
    data = layer.find('data')
    if data is None or data.get('encoding') != 'csv':
        print("Error: Only CSV encoding is supported for tile layers")
        return None

    tile_data = parse_csv_data(data.text)
    height = len(tile_data)
    width = len(tile_data[0]) if height > 0 else 0

    # Prepend empty rows for top-padding
    if pad_rows > 0:
        empty_row = [0] * width
        tile_data = [empty_row[:] for _ in range(pad_rows)] + tile_data
        height += pad_rows

    room_data = []
    start_y = room_num * 15
    end_y = start_y + 15

    if start_y >= height:
        return None

    # Extract the 16x15 room section (rows start_y to end_y-1, columns 0-15)
    for y in range(start_y, min(end_y, height)):
        row = tile_data[y][0:16]
        if len(row) == 16:
            room_data.extend([(strip_flip_flags(x) - 1) if strip_flip_flags(x) > 0 else 0 for x in row])

    # Pad to 240 bytes if fewer than 15 rows
    while len(room_data) < 240:
        room_data.append(0)

    return room_data

def process_object_layer(root, is_vertical=False):
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
                coin_data.append((x, y, 0x02))  # COIN_REG
            elif gid == SPRITE_GID_EXIT:
                print(f"Adding exit at ({x}, {y})")
                coin_data.append((x, y, 0x08))  # COIN_END
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
                coin_data.append((x, y, 0x02))  # COIN_REG
            elif gid == SPRITE_GID_EXIT:
                print(f"Adding exit at ({x}, {y})")
                coin_data.append((x, y, 0x08))  # COIN_END
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

        # Detect vertical levels: width == 16 (one screen wide) and taller than wide
        is_vertical = (tmx_data.width == 16 and tmx_data.height > 15)
        if is_vertical:
            print(f"Detected VERTICAL level ({tmx_data.width}x{tmx_data.height})")
        else:
            print(f"Detected HORIZONTAL level ({tmx_data.width}x{tmx_data.height})")

        # Process background layer
        rooms = []
        main_layer = tmx_data.layers[0]  # First layer should be the main/background layer

        if is_vertical:
            # Vertical: pad height to a multiple of 15 rows by adding empty rows at the TOP.
            # This ensures the last room is fully filled, so the level bottom aligns
            # with the bottom of the screen when starting at the last room.
            pad_rows = (15 - (tmx_data.height % 15)) % 15
            padded_height = tmx_data.height + pad_rows
            if pad_rows > 0:
                print(f"  Padding {pad_rows} empty rows at top ({tmx_data.height} -> {padded_height} rows)")
            num_rooms = padded_height // 15
            for i in range(num_rooms):
                room = process_layer_vert(main_layer, i, pad_rows)
                if room:
                    rooms.append(room)
        else:
            # Horizontal: split into rooms by columns (every 16 columns = 1 room)
            for i in range(tmx_data.width // 16):
                room = process_layer(main_layer, i)
                if room:
                    rooms.append(room)

        # Process object layer for coins and enemies
        coin_data, enemy_data = process_object_layer(tmx_data.root, is_vertical)

        # For vertical levels with top-padding, shift object Y coordinates down
        if is_vertical and pad_rows > 0:
            coin_data = [(x, y + pad_rows, ct) for x, y, ct in coin_data]
            enemy_data = [(x, y + pad_rows, et, p) for x, y, et, p in enemy_data]
        
        # Validate object counts against game engine limits
        has_exit = any(ct == 0x08 for _, _, ct in coin_data)
        has_boss = any(et in (TILE_ENEMY_BOSS1, TILE_ENEMY_BOSS2) for _, _, et, _ in enemy_data)
        if not has_exit and not has_boss:
            print(f"ERROR: No exit or boss found! Level must have an exit object (GID {SPRITE_GID_EXIT}) or a boss enemy.")
            return False

        if len(coin_data) > MAX_COINS:
            print(f"ERROR: Too many coins/objects ({len(coin_data)}) - max is {MAX_COINS}.")
            print(f"  The game will only load the first {MAX_COINS} entries.")
            # Show which items would be dropped
            for i, (x, y, ct) in enumerate(coin_data):
                marker = " <-- DROPPED!" if i >= MAX_COINS else ""
                type_name = "EXIT" if ct == 0x08 else "COIN" if ct == 0x02 else f"0x{ct:02x}"
                print(f"  [{i:2d}] ({x:3d}, {y:2d}) {type_name}{marker}")
            return False

        if len(enemy_data) > MAX_ENEMY:
            print(f"ERROR: Too many enemies ({len(enemy_data)}) - max is {MAX_ENEMY}.")
            print(f"  The game will only load the first {MAX_ENEMY} entries.")
            for i, (x, y, et, _) in enumerate(enemy_data):
                marker = " <-- DROPPED!" if i >= MAX_ENEMY else ""
                print(f"  [{i:2d}] ({x:3d}, {y:2d}) type=0x{et:02x}{marker}")
            return False

        # Get level number from filename
        level = extract_level_num(tmx_file)
        if not level:
            print("Error: Could not extract level number from filename (expected levelN.tmx)")
            return False
            
        # Generate output file
        with open(output_file, 'w') as f:
            # Write headers
            f.write('#include <stdint.h>\n\n')

            # Write room data with RLE compression
            for i, room in enumerate(rooms):
                # RLE encode: (count, value) pairs
                rle = []
                j = 0
                while j < len(room):
                    val = room[j]
                    count = 1
                    while j + count < len(room) and room[j + count] == val and count < 255:
                        count += 1
                    rle.append(count)
                    rle.append(val)
                    j += count

                f.write(f'const uint8_t level{level}_main_{i}[] = {{\n')
                # Write RLE data in groups of 16 bytes per line
                for k in range(0, len(rle), 16):
                    chunk = rle[k:k + 16]
                    f.write('    ' + ', '.join(f'0x{x:02x}' for x in chunk) + ',\n')
                f.write('};\n\n')

            # Write coin data
            # Format: (y_val, room, x_val, type)
            # Horizontal: y_val=screen_y, room=x//16, x_val=(x%16)*16
            # Vertical:   y_val=(y%15)*16, room=y//15, x_val=x*16
            f.write(f'const uint8_t level{level}_coins[] = {{\n')
            for x, y, coin_type in coin_data:
                if is_vertical:
                    room_num = y // 15
                    y_in_room = (y % 15) * 16
                    x_pixel = x * 16
                    f.write(f'    0x{y_in_room:02x}, {room_num}, 0x{x_pixel:02x}, 0x{coin_type:02x},\n')
                else:
                    f.write(f'    0x{(y*16):02x}, {x//16}, 0x{(x%16)*16:02x}, 0x{coin_type:02x},\n')
            f.write('    0xff  // End marker\n};\n\n')

            # Write enemy data (y, room, x, type, param)
            f.write(f'const uint8_t level{level}_enemies[] = {{\n')
            for x, y, enemy_type, param in enemy_data:
                param_byte = max(0, min(255, int(param)))
                if is_vertical:
                    room_num = y // 15
                    y_in_room = (y % 15) * 16
                    x_pixel = x * 16
                    f.write(f'    0x{y_in_room:02x}, {room_num}, 0x{x_pixel:02x}, 0x{enemy_type:02x}, 0x{param_byte:02x},\n')
                else:
                    f.write(f'    0x{(y*16):02x}, {x//16}, 0x{(x%16)*16:02x}, 0x{enemy_type:02x}, 0x{param_byte:02x},\n')
            f.write('    0xff  // End marker\n};\n\n')

            # Write room pointers array
            f.write(f'const uint8_t* const level{level}_main[] = {{\n')
            for i in range(len(rooms)):
                f.write(f'    level{level}_main_{i},\n')
            f.write('};\n')
            
        print(f"Successfully converted {tmx_file} to {output_file}")
        return True
        
    except Exception as e:
        print(f"Error converting TMX file: {e}")
        return False

def main():
    if len(sys.argv) != 2:
        print("Usage: python3 convert_tmx.py <level_number>")
        sys.exit(1)

    level = int(sys.argv[1])
    convert_tmx(f"level{level}.tmx", f"level{level}.c")

if __name__ == "__main__":
    main() 