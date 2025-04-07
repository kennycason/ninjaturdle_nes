#!/bin/bash

# Script to convert Tiled map CSV files to C code
# Usage: ./convert_map.sh <world_number> <level_number>

if [ $# -ne 2 ]; then
    echo "Usage: $0 <world_number> <level_number>"
    echo "Example: $0 1 1"
    exit 1
fi

WORLD=$1
LEVEL=$2
PREFIX="w${WORLD}l${LEVEL}"

echo "Converting map for World $WORLD, Level $LEVEL..."

# Check if CSV files exist
if [ ! -f "${PREFIX}_main.csv" ]; then
    echo "Error: ${PREFIX}_main.csv not found!"
    echo "Please export the main layer from Tiled as ${PREFIX}_main.csv"
    exit 1
fi

if [ ! -f "${PREFIX}_object.csv" ]; then
    echo "Error: ${PREFIX}_object.csv not found!"
    echo "Please export the objects layer from Tiled as ${PREFIX}_object.csv"
    exit 1
fi

# Convert main layer
echo "Converting main layer..."
python3 csv2c_main.py "${PREFIX}_main.csv"
echo "Generated ${PREFIX}_main.c"

# Convert objects layer
echo "Converting objects layer..."
python3 csv2c_objects.py "${PREFIX}_object.csv"
echo "Generated ${PREFIX}_object.c"

echo "Conversion complete!" 