#ifndef COLLISION_H
#define COLLISION_H

// Collision types for metatiles
#define COLLISION_NONE 0x00      // No collision (background)
#define COLLISION_PLATFORM 0x80  // Can pass through from below (platforms)
#define COLLISION_SOLID 0x40     // Solid from all directions (walls)
#define COL_DOWN 0x80           // Special flag for downward collision
#define COL_ALL (COLLISION_PLATFORM | COLLISION_SOLID)  // Mask for any collision type

#endif // COLLISION_H 