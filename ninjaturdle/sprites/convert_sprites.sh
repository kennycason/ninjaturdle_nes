#!/bin/bash
# Convert all PNG sprites to CHR format and generate C headers

# Check if png2chr exists
if [ ! -f ../../tools/png2chr ]; then
  echo "png2chr tool not found! Please compile it first."
  exit 1
fi

# Create a temporary directory for CHR files
mkdir -p ../temp_chr

# Convert sprite PNGs to CHR files
../../tools/png2chr masks.png ../temp_chr/masks.chr
../../tools/png2chr mouth.png ../temp_chr/mouth.chr
../../tools/png2chr ninja_walking_sheet.png ../temp_chr/ninja_walking.chr
../../tools/png2chr rolling.png ../temp_chr/rolling.chr

echo "Sprite conversion complete! CHR files are in ../temp_chr/"
echo "Now you can include these in your game or create a header file."