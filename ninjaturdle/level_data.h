#ifndef LEVEL_DATA_H
#define LEVEL_DATA_H

// Level object type constants
#define TILE_HP_UP 0x01
#define TILE_CORN_UP 0x02
#define TILE_ENEMY_WASP 0x10
#define TILE_ENEMY_BOUNCE 0x11
#define TILE_ENEMY_BOSS1 0x1F

// External declarations for level data
extern const unsigned char * const Coins_list[];
extern const unsigned char * const Enemy_list[];
extern const unsigned char * const Levels_list[];
extern const unsigned char Level_offsets[];

#endif 