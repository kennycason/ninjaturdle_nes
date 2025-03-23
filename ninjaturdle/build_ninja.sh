#!/bin/bash
# Build script for Ninja Turdle with custom ninja sprite

# Convert ninja sprite to CHR
echo "Converting ninja.png to CHR format..."
if [ -f "sprites/ninja.chr" ]; then
  echo "Using existing ninja.chr file."
else
  echo "Running sprite conversion..."
  # Uncomment this if you need to generate a CHR file from PNG
  # cd sprites
  # python3 ./convert_ninja.py
  # cd ..
fi

# Replace player sprite in CHR file
echo "Replacing player sprite in CHR file..."
python3 ./replace_player_sprite.py

# Rename the new CHR file to replace the original
echo "Installing new CHR file..."
mv ninjaturdle_new.chr ninjaturdle.chr

# Make sure crt0.s is using ninjaturdle.chr
echo "Updating crt0.s to use ninjaturdle.chr..."
sed -i '' 's/\.incbin "ninja_combined\.chr"/\.incbin "ninjaturdle\.chr"/' crt0.s

# Build the game
echo "Building game..."
make

echo "Build complete!"
echo "Run the game with: fceux ninjaturdle.nes"