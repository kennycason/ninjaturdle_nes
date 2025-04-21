#ifndef NINJATURDLE_H
#define NINJATURDLE_H

#include "tile_types.h"
#include "collision.h"

#define ACCEL 30
#define DECEL 50
#define GRAVITY 0x4c
#define MAX_SPEED 0x240
#define JUMP_VEL -0x600
#define MAX_RIGHT 0x9000


#pragma bss-name(push, "ZEROPAGE")

// CHR bank definitions for MMC1 4KB mode
#define CHR_BANK_FONT 0     // Font tiles and title screen (top half)
#define CHR_BANK_TITLE 1    // Title screen graphics (bottom half)
#define CHR_BANK_MAP 2      // Map tiles (top half)
#define CHR_BANK_SPRITES 3  // Sprite tiles (bottom half)

// MMC1 registers
#define MMC1_CONTROL    0x8000
#define MMC1_CHR0       0xA000
#define MMC1_CHR1       0xC000
#define MMC1_PRG        0xE000

// MMC1 control flags
#define MMC1_MIRROR_ONE_SCREEN_LOW    0x00
#define MMC1_MIRROR_ONE_SCREEN_HIGH   0x01
#define MMC1_MIRROR_VERTICAL          0x02
#define MMC1_MIRROR_HORIZONTAL        0x03

// GLOBAL VARIABLES
unsigned char pad1;
unsigned char pad1_new;
unsigned char collision;
unsigned char collision_L;
unsigned char collision_R;
unsigned char collision_U;
unsigned char collision_D;
unsigned char coordinates;
unsigned char temp1;
unsigned char temp2;
unsigned char temp3;
unsigned char temp4;
unsigned int temp5;
unsigned int temp6;
unsigned char eject_L; // from the left
unsigned char eject_R; // remember these from the collision sub routine
unsigned char eject_D; // from below
unsigned char eject_U; // from up
unsigned char direction; //facing left or right?
#define LEFT 0
#define RIGHT 1

int address;
unsigned char x; // room loader code
unsigned char y;
unsigned char nt;
unsigned char index;
unsigned char index2;
unsigned char room;
unsigned char map;
unsigned int scroll_x;
unsigned int scroll_y;
unsigned char scroll_count;
unsigned int pseudo_scroll_x;
//unsigned int pseudo_scroll_y;
unsigned char L_R_switch;
unsigned int old_x;
//unsigned char old_y;
unsigned char temp_x;
unsigned char temp_y;

// song & sfx variables
#define MAX_SONGS 2

unsigned char song;
enum { 
	SONG_GAME, 
	SONG_PAUSE
};

enum { 
	SFX_JUMP, 
	SFX_DING, 
	SFX_NOISE,
	SFX_HIT
};

unsigned char game_mode;
enum {
	MODE_TITLE, 
	MODE_GAME, 
	MODE_PAUSE, 
	MODE_SWITCH, 
	MODE_END, 
	MODE_GAME_OVER, 
	MODE_RESET
};

unsigned char coins;
unsigned char lives;
const unsigned char * pointer;
unsigned char level;
unsigned char offset;
unsigned char level_up;
unsigned char bright;
unsigned char bright_count;
unsigned char death;
unsigned char timer;
unsigned char enemy_frames; //in case of skipped frames
unsigned char map_loaded; //only load it once
unsigned char short_jump_count;

unsigned char temp_room;


#pragma bss-name(push, "BSS")

unsigned char c_map[240];
unsigned char c_map2[240];

struct Entity {
	unsigned char x;
	unsigned char y;
	unsigned char width;
	unsigned char height;
};

struct Entity ENTITY1; 
struct Entity ENTITY2; 

struct Ninja {
	unsigned int x; // low byte is sub-pixel
	unsigned int y;
	signed int vel_x; // speed, signed, low byte is sub-pixel
	signed int vel_y;
	unsigned char health;    // Player health
	unsigned char invincible; // Invincibility timer
};

struct Ninja NINJA;
// the width and height should be 1 less than the dimensions (14x12)
// note, I'm using the top left as the 0,0 on x,y

#define HERO_WIDTH 13
#define HERO_HEIGHT 11


#define MAX_COINS 16
unsigned char coin_x[MAX_COINS];
unsigned char coin_y[MAX_COINS];
unsigned char coin_active[MAX_COINS];
unsigned char coin_room[MAX_COINS];
unsigned char coin_actual_x[MAX_COINS];
unsigned char coin_type[MAX_COINS];


#define TURN_OFF 0xff
#define COIN_WIDTH 7
#define COIN_HEIGHT 11
#define BIG_COIN 13


#define MAX_ENEMY 16
unsigned char enemy_x[MAX_ENEMY];
unsigned char enemy_y[MAX_ENEMY];
unsigned char enemy_active[MAX_ENEMY];
unsigned char enemy_room[MAX_ENEMY];
unsigned char enemy_actual_x[MAX_ENEMY];
unsigned char enemy_type[MAX_ENEMY];
const unsigned char * enemy_anim[MAX_ENEMY];

#define ENEMY_WIDTH 13
#define ENEMY_HEIGHT 13


//for shuffling 16 enemies
const unsigned char shuffle_array[]={
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
	15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,
	0,2,4,6,8,10,12,14,1,3,5,7,9,11,13,15,
	15,13,11,9,7,5,3,1,14,12,10,8,6,4,2,0	
};


const unsigned char palette_title[]={
	//0x0f, 0x00, 0x10, 0x30,
	0x0f, 0x04, 0x15, 0x32,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0
}; 


const unsigned char title_color_rotate[]={
	0x32, 0x22, 0x30, 0x37
};


const unsigned char palette_bg[]={
	0x0F, 0x16, 0x26, 0x06,  // Light Purple, Dark Brown, Teal, Light Blue
	0x0F, 0x17, 0x19, 0x29,  // Light Purple, Brown, Dark Pink, Navy
	0x0F, 0x17, 0x07, 0x10,  // Light Purple, Brown, Dark Green, Dark Yellow
	0x0F, 0x08, 0x19, 0x29   // Light Purple, Green, Dark Pink, Navy
};


const unsigned char palette_sp[]={
    0x0F, 0x01, 0x11, 0x10,  // Light Purple, White, Yellow, Dark Yellow
    0x0F, 0x17, 0x28, 0x38,  // Light Purple, Brown, Navy, Tan
    0x0F, 0x0c, 0x2c, 0x30,  // Light Purple, Light Red, Maroon, Light Lime
    0x0F, 0x03, 0x13, 0x33,  // Light Purple, Light Gray, Dark Orange, Light Beige
};


const unsigned char END_TEXT[] = {0x43, 0x4F, 0x4E, 0x47, 0x52, 0x41, 0x54, 0x55, 0x4C, 0x41, 0x54, 0x49, 0x4F, 0x4E, 0x53, 0x01};  // "CONGRATULATIONS!"
const unsigned char END_TEXT2[] = {0x59, 0x4F, 0x55, 0x00, 0x53, 0x41, 0x56, 0x45, 0x44, 0x00, 0x53, 0x54, 0x45, 0x56, 0x45, 0x00, 0x41, 0x47, 0x41, 0x49, 0x4E, 0x0E};  // "YOU SAVED STEVE, AGAIN."
const unsigned char END_TEXT3[] = {0x43, 0x4F, 0x52, 0x4E, 0x1A, 0x00};  // "CORN: "

const unsigned char DEAD_TEXT[] = {0x27, 0x41, 0x4D, 0x45, 0x00, 0x2F, 0x56, 0x45, 0x52};  // "Game Over"

const unsigned char KENNY_CASON_TEXT[] = {0x2B, 0x45, 0x4E, 0x4E, 0x59, 0x00, 0x23, 0x41, 0x53, 0x4F, 0x4E};  // "Kenny Cason"

#include "title.h"


// PROTOTYPES
void load_title(void);
void load_room(void);
void draw_sprites(void);
void movement(void);	
void draw_screen_R(void);
void new_cmap(void);
char bg_collision_sub(void);
char get_position(void);
void enemy_moves(void);
void sprite_collisions(void);
void check_spr_objects(void);
void sprite_obj_init(void);
void set_sprite_zero(void);
void update_hud(void);
void bg_collision_fast(void);

char bg_coll_L(void);
char bg_coll_R(void);
char bg_coll_U(void);
char bg_coll_D(void);
char bg_coll_D2(void);

// Add these definitions for the turd projectiles
#define MAX_TURDS 4  // Maximum number of turds on screen at once
#define TURD_WIDTH 7
#define TURD_HEIGHT 7
#define TURD_SPEED 3  // Horizontal speed
#define TURD_GRAVITY 1 // How fast turds fall
#define TURD_COOLDOWN 20 // Frames between shots

// Turd direction constants
#define TURD_RIGHT 0
#define TURD_LEFT 1
#define TURD_UP 2
#define TURD_DOWN 3

// Turd projectile arrays
unsigned char turd_x[MAX_TURDS];
unsigned char turd_y[MAX_TURDS];
unsigned char turd_active[MAX_TURDS];
signed char turd_vel_x[MAX_TURDS];
signed char turd_vel_y[MAX_TURDS];
unsigned char turd_direction[MAX_TURDS];

// Corn mode variables
unsigned char corn_mode; // 0 = turd mode, 1 = corn mode
#define CORN_DAMAGE_MULTIPLIER 3 // Corn chunks do double damage

// Add these definitions for enemy bullets
#define MAX_ENEMY_BULLETS 8  // Maximum number of enemy bullets on screen at once
#define ENEMY_BULLET_WIDTH 13  // Make bullets bigger (same as HERO_WIDTH)
#define ENEMY_BULLET_HEIGHT 11 // Make bullets bigger (same as HERO_HEIGHT)
#define ENEMY_BULLET_DAMAGE 2  // Damage per hit
#define ENEMY_BULLET_COOLDOWN 90  // Frames between shots (slower than player)
#define INVINCIBLE_TIME 60  // Frames of invincibility after being hit

// Enemy bullet types
#define BULLET_LINEAR 0  // Moves in a straight line
#define BULLET_THROW 1   // Thrown like player turds with gravity

// Enemy bullet velocities
#define ENEMY_BULLET_SPEED 4     // Faster than player turds
#define ENEMY_BULLET_JUMP -6     // Higher arc than player turds
#define ENEMY_BULLET_GRAVITY 1   // Same gravity as player turds

// Enemy bullet arrays
unsigned char enemy_bullet_x[MAX_ENEMY_BULLETS];
unsigned char enemy_bullet_y[MAX_ENEMY_BULLETS];
unsigned char enemy_bullet_active[MAX_ENEMY_BULLETS];
signed char enemy_bullet_vel_x[MAX_ENEMY_BULLETS];
signed char enemy_bullet_vel_y[MAX_ENEMY_BULLETS];
unsigned char enemy_bullet_type[MAX_ENEMY_BULLETS];
unsigned char enemy_bullet_cooldown[MAX_ENEMY_BULLETS];  // Cooldown for each enemy
unsigned char enemy_bullet_room[MAX_ENEMY_BULLETS];      // Room for each enemy bullet

// Turd power flag (default to true for now)
unsigned char has_turd_power;
unsigned char turd_cooldown;

// Function prototypes
void fire_turd(void);
void update_turds(void);
void draw_turds(void);

// Enemy bullet function prototypes
void fire_enemy_bullet(unsigned char enemy_index, unsigned char bullet_type);
void update_enemy_bullets(void);
void draw_enemy_bullets(void);

// Add player health variables
#define MAX_HEALTH 10
unsigned char player_health;
unsigned char damage_cooldown; // Invincibility frames after taking damage
#define DAMAGE_COOLDOWN_TIME 60 // 1 second of invincibility

#define BOSS_MAX_HEALTH 20
#define BOSS_DAMAGE_PER_HIT 2
unsigned char boss_health;

// Collision type checks
#define IS_BACKGROUND(tile) ((tile & 0x07) < 0x04)  // Columns 0-3
#define IS_PLATFORM(tile)   ((tile & 0x07) >= 0x04 && (tile & 0x07) <= 0x05)  // Columns 4-5
#define IS_SOLID(tile)      ((tile & 0x07) >= 0x06)  // Columns 6-7

#endif // NINJATURDLE_H