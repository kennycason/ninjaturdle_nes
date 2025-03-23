// Ninja sprites for the main character
// This file will contain animations and special poses for the ninja character

// For now, we're using the original metasprite definitions from Sprites.h
// but they will point to our custom ninja tile data in the CHR file

// Future version will include:
// - Walking animation 
// - Jumping frames
// - Rolling animation
// - Combat/throwing poses

// Placeholder for ninja-specific tiles
#define NINJA_TILE_START 0x00  // First ninja tile

// We can define animation sequences here:
#define ANIM_IDLE 0
#define ANIM_WALK 1
#define ANIM_JUMP 2
#define ANIM_ROLL 3

// Animation frame counts
#define WALK_FRAME_COUNT 3