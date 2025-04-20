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
	  0xff,  7,0x30,2,
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