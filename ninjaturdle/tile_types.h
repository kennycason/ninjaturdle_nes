#ifndef TILE_TYPES_H
#define TILE_TYPES_H

// Object types - These match the TMX map data after conversion
// Corn/Coin types (0x82-0x83)
#define COIN_REG 0x00  // Regular corn (converted from 0x82)
#define COIN_END 0x01  // End coin (converted from 0x83)

// Enemy types (0x89-0x90)
#define ENEMY_WASP 0x09    // Flying wasp (converted from 0x89)
#define ENEMY_BOUNCE 0x0A  // Bouncing enemy (converted from 0x8A)
#define ENEMY_WORM 0x0B    // Worm enemy (converted from 0x8B)
#define ENEMY_BOSS1 0x10   // Boss enemy (converted from 0x90)

// Remove redundant TILE_* definitions since they're the same as above
// The convert_tmx.py script should handle the conversion from TMX IDs
// to these internal type values

#endif // TILE_TYPES_H 