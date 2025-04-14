#ifndef COLLISION_H
#define COLLISION_H

// Collision types for metatiles
#define COLLISION_NONE 0x00      // No collision (background)
#define COLLISION_PLATFORM 0x20  // Can pass through from below (platforms)
#define COLLISION_SOLID 0x40     // Solid from all directions (walls)
#define COL_DOWN 0x80           // Special flag for downward collision
#define COL_ALL (COLLISION_PLATFORM | COLLISION_SOLID)  // Mask for any collision type

// Collision type checks
#define IS_BACKGROUND(tile) ((tile & 0x07) < 0x04)  // Columns 0-3
#define IS_PLATFORM(tile)   ((tile & 0x07) >= 0x04 && (tile & 0x07) <= 0x05)  // Columns 4-5
#define IS_SOLID(tile)      ((tile & 0x07) >= 0x06)  // Columns 6-7

#endif // COLLISION_H 