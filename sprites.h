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

// a 16x16 pixel metasprite

const unsigned char NinjaSprL[]={
	  0xff,  0xfc,0x02,1,
	  7,  0xfc,0x03,1,
	  0xff,  4,0x12,1,
	  7,  4,0x13,1,
	128
};


const unsigned char NinjaSprR[]={
	  0xff,  0xfc,0x00,1,
	  7,  0xfc,0x01,1,
	  0xff,  4,0x10,1,
	  7,  4,0x11,1,
	128
};


const unsigned char CoinSpr[]={
	  0xff,  0xff,0x20,1,
	//   0xff,  7,0x30,1,
	128
};

const unsigned char CornSelectedSpr[]={
	  0xff,  0xff,0x20,2,
	128
};

const unsigned char BigCoinSpr[]={
	  0xff,  0xff,0x21,0,
	  0xff,  7,0x31,1,
	  7,  0xff,0x22,2,
	  7,  7,0x32,3,	  
	128
};

const unsigned char CoinHud[]={
	  0,  0,0x20,1,
	//   8,  0,0x24,1,
	//   0,  8,0x33,1,
	//   8,  8,0x34,1,
	128
};


const unsigned char EnemyWaspSprL[]={
	  0xff,	0xff,0x08,2,
	  7,  	0xff,0x09,2,
	  0xff,	7,	0x18, 2,
	  7,	7,	0x19, 2,
	128
};

const unsigned char EnemyWaspSprR[]={
	  0xff,	0xff,0x0A,2,
	  7,  	0xff,0x0B,2,
	  0xff,	7,	0x1A, 2,
	  7,	7,	0x1B, 2,
	128
};


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

// Thorns enemy (two states). Tiles are directly below wasp art.
const unsigned char EnemyThornsOutSpr[]={
      0xff, 0xff, 0x0c, 2,  // Top-left tile (thorns OUT, 2nd-to-last 16x16 block in row 1)
      7,    0xff, 0x0d, 2,  // Top-right tile
      0xff, 7,    0x1c, 2,  // Bottom-left tile
      7,    7,    0x1d, 2,  // Bottom-right tile
    128
};

const unsigned char EnemyThornsInSpr[]={
      0xff, 0xff, 0x0e, 2,  // Top-left tile (thorns IN, last 16x16 block in row 1)
      7,    0xff, 0x0f, 2,  // Top-right tile
      0xff, 7,    0x1e, 2,  // Bottom-left tile
      7,    7,    0x1f, 2,  // Bottom-right tile
    128
};

const unsigned char Boss1SprL[]={
    // Top-left 16x16 block
    0xff, 0xff, 0x2C, 1,  // Top-left tile
    7,    0xff, 0x2D, 1,   // Top-right tile
    0xff, 7,    0x3C, 1,   // Bottom-left tile
    7,    7,    0x3D, 1,    // Bottom-right tile

    // Top-right 16x16 block
    15,  0xff, 0x2E, 1,   // Top-left tile
    23,  0xff, 0x2F, 1,   // Top-right tile
    15,  7,    0x3E, 1,    // Bottom-left tile
    23,  7,    0x3F, 1,    // Bottom-right tile

    // Bottom-left 16x16 block
    0xff, 15,  0x4C, 1,   // Top-left tile
    7,    15,  0x4D, 1,    // Top-right tile
    0xff, 23,  0x5C, 1,   // Bottom-left tile
    7,    23,  0x5D, 1,    // Bottom-right tile

    // Bottom-right 16x16 block
    15,  15,  0x4E, 1,    // Top-left tile
    23,  15,  0x4F, 1,    // Top-right tile
    15,  23,  0x5E, 1,    // Bottom-left tile
    23,  23,  0x5F, 1,    // Bottom-right tile
    128
};

const unsigned char Boss1SprR[]={
	// Top-left 16x16 block
    0xff, 0xff, 0x6C, 1,  // Top-left tile
    7,    0xff, 0x6D, 1,   // Top-right tile
    0xff, 7,    0x7C, 1,   // Bottom-left tile
    7,    7,    0x7D, 1,    // Bottom-right tile

    // Top-right 16x16 block
    15,  0xff, 0x6E, 1,   // Top-left tile
    23,  0xff, 0x6F, 1,   // Top-right tile
    15,  7,    0x7E, 1,    // Bottom-left tile
    23,  7,    0x7F, 1,    // Bottom-right tile

    // Bottom-left 16x16 block
    0xff, 15,  0x8C, 1,   // Top-left tile
    7,    15,  0x8D, 1,    // Top-right tile
    0xff, 23,  0x9C, 1,   // Bottom-left tile
    7,    23,  0x9D, 1,    // Bottom-right tile

    // Bottom-right 16x16 block
    15,  15,  0x8E, 1,    // Top-left tile
    23,  15,  0x8F, 1,    // Top-right tile
    15,  23,  0x9E, 1,    // Bottom-left tile
    23,  23,  0x9F, 1,    // Bottom-right tile
    128
};

const unsigned char BossMotherWormSprL[]={
    // Top-left 16x16 block (0,0), (8,0), (0,8), (8,8)
    0x00, 0x00, 0xAA, 1,  // Top-left tile
    0x08, 0x00, 0xAB, 1,  // Top-right tile
    0x00, 0x08, 0xBA, 1,  // Bottom-left tile
    0x08, 0x08, 0xBB, 1,  // Bottom-right tile

    // Top-middle 16x16 block (16,0), (24,0), (16,8), (24,8)
    0x10, 0x00, 0xAC, 1,  // Top-left tile
    0x18, 0x00, 0xAD, 1,  // Top-right tile
    0x10, 0x08, 0xBC, 1,  // Bottom-left tile
    0x18, 0x08, 0xBD, 1,  // Bottom-right tile

    // Top-right 16x16 block (32,0), (40,0), (32,8), (40,8)
    0x20, 0x00, 0xAE, 1,  // Top-left tile
    0x28, 0x00, 0xAF, 1,  // Top-right tile
    0x20, 0x08, 0xBE, 1,  // Bottom-left tile
    0x28, 0x08, 0xBF, 1,  // Bottom-right tile

    // Middle-left 16x16 block
    0x00, 0x10, 0xCA, 1,   // Top-left tile
    0x08, 0x10, 0xCB, 1,   // Top-right tile
    0x00, 0x18, 0xDA, 1,   // Bottom-left tile
    0x08, 0x18, 0xDB, 1,   // Bottom-right tile

    // Middle-middle 16x16 block
    0x10, 0x10, 0xCC, 1,   // Top-left tile
    0x18, 0x10, 0xCD, 1,   // Top-right tile
    0x10, 0x18, 0xDC, 1,   // Bottom-left tile
    0x18, 0x18, 0xDD, 1,   // Bottom-right tile

    // Middle-right 16x16 block
    0x20, 0x10, 0xCE, 1,   // Top-left tile
    0x28, 0x10, 0xCF, 1,   // Top-right tile
    0x20, 0x18, 0xDE, 1,   // Bottom-left tile
    0x28, 0x18, 0xDF, 1,   // Bottom-right tile

    // Bottom-left 16x16 block
    0x00, 0x20, 0xEA, 1,   // Top-left tile
    0x08, 0x20, 0xEB, 1,   // Top-right tile
    0x00, 0x28, 0xFA, 1,   // Bottom-left tile
    0x08, 0x28, 0xFB, 1,   // Bottom-right tile

    // Bottom-middle 16x16 block
    0x10, 0x20, 0xEC, 1,   // Top-left tile
    0x18, 0x20, 0xED, 1,   // Top-right tile
    0x10, 0x28, 0xFC, 1,   // Bottom-left tile
    0x18, 0x28, 0xFD, 1,   // Bottom-right tile

    // Bottom-right 16x16 block
    0x20, 0x20, 0xEE, 1,   // Top-left tile
    0x28, 0x20, 0xEF, 1,   // Top-right tile
    0x20, 0x28, 0xFE, 1,   // Bottom-left tile
    0x28, 0x28, 0xFF, 1,   // Bottom-right tile

    128
};

//enum{ANIM_CHASE1, ANIM_BOUNCE1, ANIM_BOUNCE2};
//const unsigned char * const enemy_anim_list[]={
//	EnemyChaseSpr, EnemyBounceSpr, EnemyBounceSpr2
//}

// Using a simple 8x8 sprite for the turd projectile
const unsigned char TurdSpr[]={
    0, 0, 0x10, 1,
    128
};

const unsigned char TurdLifeSpr[]={
    0, 0, 0x30, 1, 
    128
};

// Level exit marker (16x16). Art is placed 16px
const unsigned char ExitSpr[]={
    0xff, 0xff, 0x40, 3,
    7,    0xff, 0x41, 3,
    0xff, 7,    0x50, 3,
    7,    7,    0x51, 3,
    128
};

const unsigned char EnemyWormSprL1[]={
    // Worm facing LEFT, row directly below wasp = 16x16 row 2 (tiles 0x20/0x30)
    0xff, 0xff, 0x28, 3,  // Top-left tile (frame 1)
    7,    0xff, 0x29, 3,  // Top-right tile
    0xff, 7,    0x38, 3,  // Bottom-left tile
    7,    7,    0x39, 3,  // Bottom-right tile
    128
};

const unsigned char EnemyWormSprL2[]={
    0xff, 0xff, 0x2a, 3,  // Top-left tile (frame 2)
    7,    0xff, 0x2b, 3,  // Top-right tile
    0xff, 7,    0x3a, 3,  // Bottom-left tile
    7,    7,    0x3b, 3,  // Bottom-right tile
    128
};

const unsigned char EnemyWormSprR1[]={
    // Worm facing RIGHT is its own row below that = 16x16 row 3 (tiles 0x40/0x50)
    0xff, 0xff, 0x48, 3,  // Top-left tile (frame 1)
    7,    0xff, 0x49, 3,  // Top-right tile
    0xff, 7,    0x58, 3,  // Bottom-left tile
    7,    7,    0x59, 3,  // Bottom-right tile
    128
};

const unsigned char EnemyWormSprR2[]={
    0xff, 0xff, 0x4a, 3,  // Top-left tile (frame 2)
    7,    0xff, 0x4b, 3,  // Top-right tile
    0xff, 7,    0x5a, 3,  // Bottom-left tile
    7,    7,    0x5b, 3,  // Bottom-right tile
    128
};

#endif // SPRITES_H