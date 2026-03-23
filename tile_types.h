#ifndef TILE_TYPES_H
#define TILE_TYPES_H

// Object types - These match the TMX map data after conversion
#define HP_UP 0x01  // HP UP TBD
#define COIN_REG 0x02  // Regular corn
#define COIN_END 0x08  // End coin

#define ENEMY_WASP 0x09    // Flying wasp
#define ENEMY_BOUNCE 0x0A  // Bouncing enemy
#define ENEMY_WORM 0x0B    // Worm enemy
#define ENEMY_THORNS 0x0C  // Stationary thorns
#define ENEMY_RINGWORM 0x0D // Ring worm - bounces around room
#define ENEMY_HOPWORM 0x0E  // Hop worm - hops toward player
#define ENEMY_POOD 0x0F     // Poop monster - chases when close
#define ENEMY_BOSS1 0x10   // Boss enemy
#define ENEMY_SPIDER 0x11  // Spider - drops when player near
#define ENEMY_FLYBUG 0x12  // Flying bug - ping pongs left/right
#define ENEMY_VIRUS 0x13   // Angry virus - hovers, chases when hit
#define ENEMY_BOSS2 0x20   // Boss2 enemy

#endif // TILE_TYPES_H
