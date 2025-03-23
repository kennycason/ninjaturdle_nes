// Ninja Turdle Sprites
// Contains sprite definitions using existing tile set

// Ninja Turdle sprites for the main character
// This file contains metasprite definitions for the ninja character

// Define where in the CHR data our ninja sprites start
// We're using the same tile IDs as the original sprites
// since we're replacing those tiles in the CHR file
#define NINJA_TILE_OFFSET 0x00  // Starting at the beginning of the tile set

// Ninja standing sprites (left and right facing)
const unsigned char NinjaSprL[]={
    0xff, 0xfc, 0x02, 0,  // top-left
    7,    0xfc, 0x03, 0,  // top-right
    0xff, 4,    0x12, 0,  // bottom-left
    7,    4,    0x13, 0,  // bottom-right
    128
};

const unsigned char NinjaSprR[]={
    0xff, 0xfc, 0x00, 0,  // top-left
    7,    0xfc, 0x01, 0,  // top-right
    0xff, 4,    0x10, 0,  // bottom-left
    7,    4,    0x11, 0,  // bottom-right
    128
};

// Walking animation frames (right facing)
const unsigned char NinjaWalkR1[]={
    0xff, 0xfc, 0x04, 0,  // top-left
    7,    0xfc, 0x05, 0,  // top-right
    0xff, 4,    0x14, 0,  // bottom-left
    7,    4,    0x15, 0,  // bottom-right
    128
};

// Walking animation frames (left facing)
const unsigned char NinjaWalkL1[]={
    0xff, 0xfc, 0x06, 0,  // top-left
    7,    0xfc, 0x07, 0,  // top-right
    0xff, 4,    0x16, 0,  // bottom-left
    7,    4,    0x17, 0,  // bottom-right
    128
};

const unsigned char NinjaWalkR2[]={
    0xff, 0xfc, 0x08, 0,  // top-left
    7,    0xfc, 0x09, 0,  // top-right
    0xff, 4,    0x18, 0,  // bottom-left
    7,    4,    0x19, 0,  // bottom-right
    128
};

const unsigned char NinjaWalkL2[]={
    0xff, 0xfc, 0x0A, 0,  // top-left
    7,    0xfc, 0x0B, 0,  // top-right
    0xff, 4,    0x1A, 0,  // bottom-left
    7,    4,    0x1B, 0,  // bottom-right
    128
};

// Animation sequences
enum {
    ANIM_NINJA_R, ANIM_NINJA_L,
    ANIM_NINJA_WALK_R1, ANIM_NINJA_WALK_R2,
    ANIM_NINJA_WALK_L1, ANIM_NINJA_WALK_L2
};