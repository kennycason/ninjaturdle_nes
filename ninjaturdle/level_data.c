#define COIN_REG 0
#define COIN_END 1

#define ENEMY_WASP 0
#define ENEMY_BOUNCE 1
#define ENEMY_BOSS1 2

//NOTE MAX_COINS = 12

//y, room, x, type
//y = TURN_OFF end of list
const unsigned char level_1_coins[]={
    0x90, 0, 0x70, COIN_REG, 
    0x90, 0, 0x90, COIN_REG, 
    0x50, 1, 0x40, COIN_REG,  
    0x70, 2, 0x00, COIN_REG, 
    0x50, 2, 0x70, COIN_REG, 
    0x50, 3, 0xa0, COIN_REG, 
    0x50, 3, 0xd0, COIN_REG, 
    0x60, 4, 0xf0, COIN_REG, 
    0x60, 5, 0x20, COIN_REG,  
    0x30, 6, 0xc0, COIN_REG, 
    0x30, 6, 0xe0, COIN_REG, 
    0x30, 7, 0x80, COIN_REG, 
    // 0xb0, 7, 0xc0, COIN_END,     // End of level coin - commented out
    TURN_OFF
};

const unsigned char level_2_coins[]={
    0xa0, 1, 0x20, COIN_REG, 
    0xa0, 1, 0x40, COIN_REG,
    0x60, 2, 0x70, COIN_REG, 
    0x30, 3, 0x20, COIN_REG, 
    0x30, 3, 0x70, COIN_REG, 
    0xc0, 5, 0x00, COIN_REG, 
    0xc0, 5, 0x50, COIN_REG,
    0x40, 6, 0x90, COIN_REG, 
    0x40, 6, 0xd0, COIN_REG, 
    0x40, 7, 0x40, COIN_REG, 
    0xa0, 7, 0xc0, COIN_END, 
    TURN_OFF
};

const unsigned char level_3_coins[]={
    0x80, 0, 0x80, COIN_REG, 
    0x70, 1, 0x50, COIN_REG,
    0x80, 1, 0xd0, COIN_REG,
    0x80, 2, 0x40, COIN_REG, 
    0x80, 2, 0x80, COIN_REG,
    0x80, 2, 0xc0, COIN_REG, 
    0x80, 3, 0x30, COIN_REG, 
    0x50, 7, 0x25, COIN_REG, 
    0x50, 7, 0x80, COIN_END, 
    0xb0, 7, 0xd0, COIN_REG, 
    TURN_OFF
};

const unsigned char * const Coins_list[]={
    level_1_coins, level_2_coins, level_3_coins
};


//NOTE MAX_ENEMY = 10

//NOTE, after testing, we can only handle 4 enemies on the same screen

//y, room, x
//y = TURN_OFF end of list
const unsigned char level_1_enemies[]={

/* stress test    
    0x11,0,0xe2,ENEMY_WASP,
    0x21,0,0xe4,ENEMY_WASP,
    0x31,0,0xe6,ENEMY_WASP,
    0x41,0,0xe8,ENEMY_WASP,
    0x51,0,0xe8,ENEMY_WASP,
    0x61,0,0xe8,ENEMY_WASP,
    0x71,0,0xe8,ENEMY_WASP,
*/
    
    0x90, 0, 0xc0, ENEMY_WASP, 
    0xc0, 1, 0xe0, ENEMY_BOUNCE, 
    0xc0, 2, 0x30, ENEMY_BOUNCE,  
    0x80, 2, 0x90, ENEMY_WASP, 
    0xb0, 3, 0x20, ENEMY_BOUNCE,  
    0xc0, 3, 0xb0, ENEMY_WASP, 
    0x80, 5, 0x00, ENEMY_BOUNCE, 
    0xc0, 5, 0x90, ENEMY_WASP, 
    0xb0, 7, 0xc0, ENEMY_BOSS1, // Moved boss to where the big coin was
    TURN_OFF
};


const unsigned char level_2_enemies[]={
    0xc0, 0, 0x90, ENEMY_WASP, 
    0xc0, 1, 0xd0, ENEMY_WASP, 
    0x40, 3, 0x40, ENEMY_BOUNCE, 
    0xc0, 4, 0x30, ENEMY_BOUNCE, 
    0xc0, 4, 0x80, ENEMY_BOUNCE, 
    0xc0, 6, 0x20, ENEMY_WASP, 
    0xc0, 7, 0x20, ENEMY_BOUNCE, 
    0xc0, 7, 0x60, ENEMY_BOUNCE, 
    TURN_OFF
};

    
const unsigned char level_3_enemies[]={
    0xc0, 0, 0xc0, ENEMY_BOUNCE, 
    0xc0, 0, 0xf0, ENEMY_BOUNCE, 
    0xc0, 1, 0x80, ENEMY_WASP, 
    0xc0, 1, 0xd0, ENEMY_WASP, 
    0xc0, 2, 0x40, ENEMY_BOUNCE, 
    0xc0, 2, 0x80, ENEMY_BOUNCE, 
    0xc0, 2, 0xc0, ENEMY_BOUNCE, 
    0xb0, 3, 0x10, ENEMY_BOUNCE, 
    0xb0, 4, 0x60, ENEMY_WASP, 
    0x90, 5, 0x40, ENEMY_BOUNCE, 
    0x90, 6, 0x50, ENEMY_BOUNCE, 
    0xa0, 6, 0xc0, ENEMY_BOUNCE,
    0xc0, 7, 0xe0, ENEMY_WASP, 
    TURN_OFF
};

const unsigned char * const Enemy_list[]={
    level_1_enemies, level_2_enemies, level_3_enemies
};

// tile tids 0x00-0x3F (0-63) (64 16x16 tiles, 256 8x8 tiles)
const unsigned char metatiles1[] = {
    // row 0

    // Background tiles (columns 0-3)
    0x00, 0x01, 
    0x10, 0x11, COLLISION_NONE | 3,     // Empty background (ID 0)

    0x02, 0x03, 
    0x12, 0x13, COLLISION_NONE | 2,    // Basic background (ID 1)

    0x04, 0x05, 
    0x14, 0x15, COLLISION_NONE | 1,    // Alt background (ID 2)

    0x06, 0x07, 
    0x16, 0x17, COLLISION_NONE | 1,    // Decorative 1 (ID 3)
    
    // Platform tiles (columns 4-5)
    0x08, 0x09, 
    0x18, 0x19, COLLISION_PLATFORM | 1,  // Platform style 1 (ID 4)

    0x0A, 0x0B, 
    0x1A, 0x1B, COLLISION_PLATFORM | 1,   // Platform style 2 (ID 5)
    
    // Solid tiles (columns 6-7)
    0x0C, 0x0D, 
    0x1C, 0x1D, COLLISION_SOLID | 2,      // Solid block 1 (ID 6)

    0x0E, 0x0F, 
    0x1E, 0x1F, COLLISION_SOLID | 2,      // Solid block 2 (ID 7)

    // row 1

    // Background tiles (columns 0-3)
    0x20, 0x21, 
    0x30, 0x31, COLLISION_NONE | 0,    // Empty background (ID 8)

    0x22, 0x23, 
    0x32, 0x33, COLLISION_NONE | 1,    // Basic background (ID 9)

    0x24, 0x25, 
    0x34, 0x35, COLLISION_NONE | 1,    // Alt background (ID A)

    0x26, 0x27, 
    0x36, 0x37, COLLISION_NONE | 1,    // Decorative 1 (ID B)
    
    // Platform tiles (columns 4-5)
    0x28, 0x29, 
    0x38, 0x39, COLLISION_PLATFORM | 1,    // Platform style 1 (ID C)

    0x2A, 0x2B, 
    0x3A, 0x3B, COLLISION_PLATFORM | 1,    // Platform style 2 (ID D)
    
    // Solid tiles (columns 6-7)
    0x2C, 0x2D, 
    0x3C, 0x3D, COLLISION_SOLID | 2,       // Solid block 1 (ID E)

    0x2E, 0x2F, 
    0x3E, 0x3F, COLLISION_SOLID | 2,     // Solid block 2 (ID F)


    // row 3

    // Background tiles (columns 0-3)
    0x40, 0x41, 
    0x50, 0x51, COLLISION_NONE | 0,    // Empty background (ID 10)

    0x42, 0x43, 
    0x52, 0x53, COLLISION_NONE | 1,    // Basic background (ID 11)

    0x44, 0x45, 
    0x54, 0x55, COLLISION_NONE | 1,    // Alt background (ID 12)

    0x46, 0x47, 
    0x56, 0x57, COLLISION_NONE | 1,    // Decorative 1 (ID 13)
    
    // Platform tiles (columns 4-5)
    0x48, 0x49, 
    0x58, 0x59, COLLISION_PLATFORM | 1,    // Platform style 1 (ID 14)

    0x4A, 0x4B, 
    0x5A, 0x5B, COLLISION_PLATFORM | 1,    // Platform style 2 (ID 15)
    
    // Solid tiles (columns 6-7)
    0x4C, 0x4D, 
    0x5C, 0x5D, COLLISION_SOLID | 2,      // Solid block 1 (ID 16)

    0x4E, 0x4F, 
    0x5E, 0x5F, COLLISION_SOLID | 2,     // Solid block 2 (ID 17)

    // row 4

    // Background tiles (columns 0-3)
    0x60, 0x61, 
    0x70, 0x71, COLLISION_NONE | 0,    // Empty background (ID 18)

    0x62, 0x63, 
    0x72, 0x73, COLLISION_NONE | 1,    // Basic background (ID 19)

    0x64, 0x65, 
    0x74, 0x75, COLLISION_NONE | 1,    // Alt background (ID 1A))

    0x66, 0x67, 
    0x76, 0x77, COLLISION_NONE | 1,    // Decorative 1 (ID 1B)
    
    // Platform tiles (columns 4-5)
    0x68, 0x69, 
    0x78, 0x79, COLLISION_PLATFORM | 1,    // Platform style 1 (ID 1C)

    0x6A, 0x6B, 
    0x7A, 0x7B, COLLISION_PLATFORM | 1,    // Platform style 2 (ID 1D)
    
    // Solid tiles (columns 6-7)
    0x6C, 0x6D, 
    0x7C, 0x7D, COLLISION_SOLID | 2,      // Solid block 1 (ID 1E)

    0x6E, 0x6F, 
    0x7E, 0x7F, COLLISION_SOLID | 2,     // Solid block 2 (ID 1F)

    // row 5

    // Background tiles (columns 0-3)
    0x80, 0x81, 
    0x90, 0x91, COLLISION_NONE | 0,    // Empty background (ID 20)

    0x82, 0x83, 
    0x92, 0x93, COLLISION_NONE | 1,    // Basic background (ID 21)

    0x84, 0x85, 
    0x94, 0x95, COLLISION_NONE | 1,    // Alt background (ID 22)

    0x86, 0x87, 
    0x96, 0x97, COLLISION_NONE | 1,    // Decorative 1 (ID 23)
    
    // Platform tiles (columns 4-5)
    0x88, 0x89, 
    0x98, 0x99, COLLISION_PLATFORM | 1,    // Platform style 1 (ID 24)

    0x8A, 0x8B, 
    0x9A, 0x9B, COLLISION_PLATFORM | 1,    // Platform style 2 (ID 25)
    
    // Solid tiles (columns 6-7)
    0x8C, 0x8D, 
    0x9C, 0x9D, COLLISION_SOLID | 2,      // Solid block 1 (ID 26)

    0x8E, 0x8F, 
    0x9E, 0x9F, COLLISION_SOLID | 2,     // Solid block 2 (ID 27)

    // row 6

    // Background tiles (columns 0-3)
    0xA0, 0xA1, 
    0xB0, 0xB1, COLLISION_NONE | 0,    // Empty background (ID 28)

    0xA2, 0xA3, 
    0xB2, 0xB3, COLLISION_NONE | 1,    // Basic background (ID 29)

    0xA4, 0xA5, 
    0xB4, 0xB5, COLLISION_NONE | 1,    // Alt background (ID 2A)

    0xA6, 0xA7, 
    0xB6, 0xB7, COLLISION_NONE | 1,    // Decorative 1 (ID 2B)
    
    // Platform tiles (columns 4-5)
    0xA8, 0xA9, 
    0xB8, 0xB9, COLLISION_PLATFORM | 1,    // Platform style 1 (ID 2C)

    0xAA, 0xAB, 
    0xBA, 0xBB, COLLISION_PLATFORM | 1,    // Platform style 2 (ID 2D)
    
    // Solid tiles (columns 6-7)
    0xAC, 0xAD, 
    0xBC, 0xBD, COLLISION_SOLID | 2,      // Solid block 1 (ID 2E)

    0xAE, 0xAF, 
    0xBE, 0xBF, COLLISION_SOLID | 2,     // Solid block 2 (ID 2F)

    // row 7

    // Background tiles (columns 0-3)
    0xC0, 0xC1, 
    0xD0, 0xD1, COLLISION_NONE | 0,    // Empty background (ID 30)

    0xC2, 0xC3, 
    0xD2, 0xD3, COLLISION_NONE | 1,    // Basic background (ID 31)

    0xC4, 0xC5, 
    0xD4, 0xD5, COLLISION_NONE | 1,    // Alt background (ID 32)

    0xC6, 0xC7, 
    0xD6, 0xD7, COLLISION_NONE | 1,    // Decorative 1 (ID 33)
    
    // Platform tiles (columns 4-5)
    0xC8, 0xC9, 
    0xD8, 0xD9, COLLISION_PLATFORM | 1,    // Platform style 1 (ID 34)

    0xCA, 0xCB, 
    0xDA, 0xDB, COLLISION_PLATFORM | 1,    // Platform style 2 (ID 35)
    
    // Solid tiles (columns 6-7)
    0xCC, 0xCD, 
    0xDC, 0xDD, COLLISION_SOLID | 2,      // Solid block 1 (ID 36)

    0xCE, 0xCF, 
    0xDE, 0xDF, COLLISION_SOLID | 2,     // Solid block 2 (ID 37)


    // row 8

    // Background tiles (columns 0-3)
    0xE0, 0xE1, 
    0xF0, 0xF1, COLLISION_NONE | 0,    // Empty background (ID 38)

    0xE2, 0xE3, 
    0xF2, 0xF3, COLLISION_NONE | 1,    // Basic background (ID 39)

    0xE4, 0xE5, 
    0xF4, 0xF5, COLLISION_NONE | 1,    // Alt background (ID 3A)

    0xE6, 0xE7, 
    0xF6, 0xF7, COLLISION_NONE | 1,    // Decorative 1 (ID 3B)
    
    // Platform tiles (columns 4-5)
    0xE8, 0xE9, 
    0xF8, 0xF9, COLLISION_PLATFORM | 1,    // Platform style 1 (ID 3C)

    0xEA, 0xEB, 
    0xFA, 0xFB, COLLISION_PLATFORM | 1,    // Platform style 2 (ID 3D)
    
    // Solid tiles (columns 6-7)
    0xEC, 0xED, 
    0xFC, 0xFD, COLLISION_SOLID | 2,      // Solid block 1 (ID 3E)

    0xEE, 0xEF, 
    0xFE, 0xFF, COLLISION_SOLID | 2,     // Solid block 2 (ID 3F)
};

// Palette mapping array - maps tile IDs to palette numbers (0-3)
const unsigned char metatiles_pal1[]={
    3,  // ID 0 - Empty background
    2,  // ID 1 - Basic background
    1,  // ID 2 - Alt background
    0,  // ID 3 - Decorative 1
    1,  // ID 4 - Platform style 1
    1,  // ID 5 - Platform style 2
    2,  // ID 6 - Solid block 1
    2,  // ID 7 - Solid block 2
    0,  // ID 8 - Empty background
    1,  // ID 9 - Basic background
    2,  // ID A - Alt background
    3,  // ID B - Decorative 1
    1,  // ID C - Platform style 1
    1,  // ID D - Platform style 2
    2,  // ID E - Solid block 1
    2   // ID F - Solid block 2
};

#define COL_DOWN 0x80
#define COL_ALL 0x40

#include "BG/w1l1_main.c"
#include "BG/w1l2_main.c"
#include "BG/w1l3_main.c"

const unsigned char * const Levels_list[]={
w1l1_main_0,w1l1_main_1,w1l1_main_2,w1l1_main_3,w1l1_main_4,w1l1_main_5,w1l1_main_6,w1l1_main_7,
w1l2_main_0,w1l2_main_1,w1l2_main_2,w1l2_main_3,w1l2_main_4,w1l2_main_5,w1l2_main_6,w1l2_main_7,
w1l3_main_0,w1l3_main_1,w1l3_main_2,w1l3_main_3,w1l3_main_4,w1l3_main_5,w1l3_main_6,w1l3_main_7
};

const unsigned char Level_offsets[]={
    0,8,16
};

#define MAX_ROOMS (8-1)
#define MAX_SCROLL (MAX_ROOMS*0x100)-1
// data is exactly 240 bytes, 16 * 15
// doubles as the collision map data

