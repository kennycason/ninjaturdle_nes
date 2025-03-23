#!/bin/bash
# Build script for Ninja Turdle

# First convert all sprites to CHR
echo "Converting sprites..."
cd "$(dirname "$0")/../sprites"
./convert_all.py

# Then build the game
echo "Building game..."
cd ..
make clean
make

echo "Build complete!"
echo "Run the game with: fceux ninjaturdle.nes"