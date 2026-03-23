#ifndef SPRITES_H
#define SPRITES_H

#include <stdint.h>

// Sprite tile structure - matches NES OAM format
typedef struct {
    int8_t y_offset;
    uint8_t tile_index;
    uint8_t attributes;
    int8_t x_offset;
} sprite_tile_t;

// ---- NINJA (tile 0: face right, tile 1: walk right) ----
// Only right-facing stored; use oam_meta_spr_flip_h() for left

const unsigned char NinjaSprR[]={
	  0xff,  0xfc,0x00,1,
	  7,  0xfc,0x01,1,
	  0xff,  4,0x10,1,
	  7,  4,0x11,1,
	128
};

// ---- COINS / ITEMS ----

const unsigned char CoinSpr[]={
	  0,  0,0x20,1,
	128
};

const unsigned char CornSelectedSpr[]={
	  0,  0,0x20,2,
	128
};

const unsigned char BigCoinSpr[]={
	  0,  0,0x21,0,
	  0,  8,0x31,1,
	  8,  0,0x22,2,
	  8,  8,0x32,3,
	128
};

// ---- BOUNCE ENEMY (tile 2,3: face left) ----
// Use H-flip for right-facing

const unsigned char EnemyBounceSpr[]={
	  0xff,	0xff,0x06,3,
	  7,  	0xff,0x07,3,
	  0xff,	7,	0x16, 3,
	  7,	7,	0x17, 3,
	128
};

const unsigned char EnemyBounceSpr2[]={
	  0xff,	0xff,0x04,3,
	  7,  	0xff,0x05,3,
	  0xff,	7,	0x14, 3,
	  7,	7,	0x15, 3,
	128
};

// ---- WORM ENEMY (tile 4,5: face left) ----
// Use H-flip for right-facing

const unsigned char EnemyWormSprL1[]={
    0xff, 0xff, 0x08, 3,
    7,    0xff, 0x09, 3,
    0xff, 7,    0x18, 3,
    7,    7,    0x19, 3,
    128
};

const unsigned char EnemyWormSprL2[]={
    0xff, 0xff, 0x0a, 3,
    7,    0xff, 0x0b, 3,
    0xff, 7,    0x1a, 3,
    7,    7,    0x1b, 3,
    128
};

// ---- THORNS (tile 6,7: no facing) ----

const unsigned char EnemyThornsOutSpr[]={
      0xff, 0xff, 0x0c, 2,
      7,    0xff, 0x0d, 2,
      0xff, 7,    0x1c, 2,
      7,    7,    0x1d, 2,
    128
};

const unsigned char EnemyThornsInSpr[]={
      0xff, 0xff, 0x0e, 2,
      7,    0xff, 0x0f, 2,
      0xff, 7,    0x1e, 2,
      7,    7,    0x1f, 2,
    128
};

// ---- RING WORM (tile 9: symmetric, bounces around) ----

const unsigned char EnemyRingWormSpr[]={
    0xff, 0xff, 0x22, 2,
    7,    0xff, 0x23, 2,
    0xff, 7,    0x32, 2,
    7,    7,    0x33, 2,
    128
};

// ---- HOP WORM (tile A: face left, hops toward player) ----

const unsigned char EnemyHopWormSprL[]={
    0xff, 0xff, 0x24, 3,
    7,    0xff, 0x25, 3,
    0xff, 7,    0x34, 3,
    7,    7,    0x35, 3,
    128
};

// ---- WASP (tile B: face left) ----
// Use H-flip for right-facing

const unsigned char EnemyWaspSprL[]={
	  0xff,	0xff,0x26,2,
	  7,  	0xff,0x27,2,
	  0xff,	7,	0x36, 2,
	  7,	7,	0x37, 2,
	128
};

// ---- POOP MONSTER (tile C,D: face left, 2 frames) ----
// Use H-flip for right-facing

const unsigned char EnemyPoodSprL1[]={
    0xff, 0xff, 0x28, 3,
    7,    0xff, 0x29, 3,
    0xff, 7,    0x38, 3,
    7,    7,    0x39, 3,
    128
};

const unsigned char EnemyPoodSprL2[]={
    0xff, 0xff, 0x2a, 3,
    7,    0xff, 0x2b, 3,
    0xff, 7,    0x3a, 3,
    7,    7,    0x3b, 3,
    128
};

// ---- BOSS 1 (tiles E,F,16,17: face left, 32x32) ----
// Use H-flip for right-facing

const unsigned char Boss1SprL[]={
    // Top-left 16x16 block
    0xff, 0xff, 0x2C, 1,
    7,    0xff, 0x2D, 1,
    0xff, 7,    0x3C, 1,
    7,    7,    0x3D, 1,

    // Top-right 16x16 block
    15,  0xff, 0x2E, 1,
    23,  0xff, 0x2F, 1,
    15,  7,    0x3E, 1,
    23,  7,    0x3F, 1,

    // Bottom-left 16x16 block
    0xff, 15,  0x4C, 1,
    7,    15,  0x4D, 1,
    0xff, 23,  0x5C, 1,
    7,    23,  0x5D, 1,

    // Bottom-right 16x16 block
    15,  15,  0x4E, 1,
    23,  15,  0x4F, 1,
    15,  23,  0x5E, 1,
    23,  23,  0x5F, 1,
    128
};

// BOSS 2 / MOTHER WORM sprite removed to save ROM space - re-add when needed

// ---- PROJECTILES ----

const unsigned char TurdSpr[]={
    0, 0, 0x10, 1,
    128
};

const unsigned char TurdLifeSpr[]={
    0, 0, 0x30, 1,
    128
};

// ---- EXIT MARKER (tile 20: 16x16) ----

const unsigned char ExitSpr[]={
    0xff, 0xff, 0x40, 3,
    7,    0xff, 0x41, 3,
    0xff, 7,    0x50, 3,
    7,    7,    0x51, 3,
    128
};

// ---- SPIDER (3rd row col 1, directly after exit: 16x16) ----

const unsigned char EnemySpiderSpr[]={
    0xff, 0xff, 0x42, 2,
    7,    0xff, 0x43, 2,
    0xff, 7,    0x52, 2,
    7,    7,    0x53, 2,
    128
};

// ---- FLYING BUG (3rd row col 2: 16x16, face left) ----

const unsigned char EnemyFlyBugSprL[]={
    0xff, 0xff, 0x44, 2,
    7,    0xff, 0x45, 2,
    0xff, 7,    0x54, 2,
    7,    7,    0x55, 2,
    128
};

// ---- ANGRY VIRUS (3rd row col 3: 16x16, face left) ----

const unsigned char EnemyVirusSprL[]={
    0xff, 0xff, 0x46, 2,
    7,    0xff, 0x47, 2,
    0xff, 7,    0x56, 2,
    7,    7,    0x57, 2,
    128
};

#endif // SPRITES_H
