#!/usr/bin/env python3

import sys
import os
import re
import xml.etree.ElementTree as ET
import argparse

def extract_world_level(filename):
    """Extract world and level numbers from filename."""
    match = re.match(r'w(\d+)l(\d+)', os.path.basename(filename))
    if match:
        return match.group(1), match.group(2)
    return None, None

def parse_csv_data(csv_text):
    """Parse CSV data from TMX file into a 2D list of integers."""
    rows = [row.strip() for row in csv_text.strip().split('\n')]
    return [[int(cell) - 1 for cell in row.split(',') if cell.strip()] for row in rows]

def convert_main_layer(tmx_file, output_file):
    """Convert the main layer to C code."""
    # Parse TMX file
    tree = ET.parse(tmx_file)
    root = tree.getroot()
    
    # Find the main layer's data
    main_layer = root.find(".//layer[@name='main']/data[@encoding='csv']")
    if main_layer is None:
        print("Error: Main layer not found!")
        return
    
    # Parse CSV data
    layer_data = parse_csv_data(main_layer.text)
    height = len(layer_data)
    width = len(layer_data[0])
    rooms = width // 16

    # Debug: Print first few values to verify
    print(f"First row values: {layer_data[0][:16]}")
    
    # Write each room's data
    for room in range(rooms):
        base_name = os.path.splitext(os.path.basename(output_file.name))[0]
        output_file.write(f"const unsigned char {base_name}_{room}[] = {{\n")
        for y in range(height):
            for x in range(16):  # 16 tiles per room
                tile_x = x + (room * 16)
                if y == height-1 and x == 15:
                    output_file.write(f"{layer_data[y][tile_x]}")
                else:
                    output_file.write(f"{layer_data[y][tile_x]},")
            output_file.write("\n")
        output_file.write("};\n\n")

    # Write the room list
    base_name = os.path.splitext(os.path.basename(output_file.name))[0]
    output_file.write(f"\nconst unsigned char * const {base_name}_list[] = {{\n")
    for room in range(rooms):
        if room == rooms - 1:
            output_file.write(f"{base_name}_{room}")
        else:
            output_file.write(f"{base_name}_{room},")
    output_file.write("\n};\n\n")

def convert_object_layer(tmx_file, output_file):
    """Convert the object layer to C code."""
    # Parse TMX file
    tree = ET.parse(tmx_file)
    root = tree.getroot()
    
    # Find the object layer's data
    object_layer = root.find(".//layer[@name='object']/data[@encoding='csv']")
    if object_layer is None:
        print("Error: Object layer not found!")
        return
    else:
        print("Object layer found.")
    
    # Get the firstgid for the sprites tileset - look for tileset with source="sprites.tsx"
    sprites_tileset = root.find(".//tileset[@source='sprites.tsx']")
    if sprites_tileset is None:
        print("Error: Sprites tileset not found!")
        print("Available tilesets:")
        for tileset in root.findall(".//tileset"):
            print(f"  source={tileset.get('source')} firstgid={tileset.get('firstgid')}")
        return
    else:
        print("Sprites tileset found.")
    sprites_firstgid = int(sprites_tileset.get('firstgid'))
    print(f"Sprites firstgid: {sprites_firstgid}")
    
    # Parse CSV data
    layer_data = parse_csv_data(object_layer.text)
    height = len(layer_data)
    width = len(layer_data[0])
    rooms = width // 16

    print(f"Writing header to {output_file.name}")
    output_file.write("// Y, room, X, object #\n\n")
    output_file.flush()  # Force write to disk
    
    # Process objects room by room
    objects_found = 0
    for room in range(rooms):
        room_start = room * 16
        room_end = room_start + 16
        
        # Scan each room column by column
        for x in range(room_start, room_end):
            for y in range(height):
                value = layer_data[y][x] + 1  # Convert back to 1-based for comparison
                if value != 0:  # If there's an object
                    # Get local tile ID by subtracting the sprites firstgid
                    obj_id = value - sprites_firstgid
                    x_in_room = x % 16
                    print(f"Object found at (x={x_in_room}, y={y}) with ID: {obj_id}")
                    output_line = f"0x{y:02x}0, {room}, 0x{x_in_room:02x}0, obj{obj_id}, \n"
                    print(f"Writing: {output_line.strip()}")
                    output_file.write(output_line)
                    output_file.flush()  # Force write to disk
                    objects_found += 1

    print(f"Writing footer to {output_file.name}")
    output_file.write("TURN_OFF\n\n")
    output_file.flush()  # Force write to disk
    print(f"Total objects written: {objects_found}")
    print(f"File size after writing: {os.path.getsize(output_file.name)} bytes")

def main():
    parser = argparse.ArgumentParser(description='Convert Tiled TMX files to C code')
    parser.add_argument('world', type=int, help='World number')
    parser.add_argument('level', type=int, help='Level number')
    args = parser.parse_args()

    tmx_file = f"w{args.world}l{args.level}.tmx"
    if not os.path.exists(tmx_file):
        print(f"Error: {tmx_file} not found!")
        sys.exit(1)

    print(f"Converting map for World {args.world}, Level {args.level}...")

    # Convert main layer
    main_output = f"w{args.world}l{args.level}_main.c"
    with open(main_output, 'w') as f:
        convert_main_layer(tmx_file, f)
    print(f"Generated {main_output}")

    # Convert object layer
    object_output = f"w{args.world}l{args.level}_object.c"
    with open(object_output, 'w') as f:
        convert_object_layer(tmx_file, f)
    print(f"Generated {object_output}")

    print("Conversion complete!")

if __name__ == "__main__":
    main() 