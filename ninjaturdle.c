/*	
 * Ninja Turdle II - NES
 */	
 
#include "lib/neslib.h"
#include "lib/nesdoug.h"
#include "lib/kenes.h"
#include "sprites.h"
#include "ninjaturdle.h"
#include "level_data.c"

#define LEFT 0
#define RIGHT 1

unsigned char bounce[] = {
    0, 0, 0, 1, 2, 2, 2, 1
};
	
unsigned char boss_health;
unsigned char coyote_time;  // Frames of coyote time remaining
unsigned char was_jumping;  // Whether we were jumping last frame
unsigned char drop_through; // Timer: when > 0, ninja falls through platforms
	
unsigned char enemy_dir[MAX_ENEMY]; // 0 = left, 1 = right

// Default anim sprites indexed by (enemy_type - ENEMY_WASP)
const unsigned char * const enemy_init_anim[] = {
	EnemyWaspSprL,      // 0x09 WASP
	EnemyBounceSpr,     // 0x0A BOUNCE
	EnemyWormSprL1,     // 0x0B WORM
	EnemyThornsInSpr,   // 0x0C THORNS
	EnemyRingWormSpr,   // 0x0D RINGWORM
	EnemyHopWormSprL,   // 0x0E HOPWORM
	EnemyPoodSprL1,     // 0x0F POOD
	Boss1SprL,          // 0x10 BOSS1
	EnemySpiderSpr,     // 0x11 SPIDER
	EnemyFlyBugSprL,    // 0x12 FLYBUG
	EnemyVirusSprL      // 0x13 VIRUS
};

// Tracks which room index is in c_map and c_map2
unsigned char cmap_room_id;   // room index stored in c_map (even rooms)
unsigned char cmap2_room_id;  // room index stored in c_map2 (odd rooms)
unsigned char above_screen;   // nonzero when player jumped above top of screen

// Decompress RLE room data: (count, value) pairs into 240-byte dest
void decompress_room(unsigned char *dest, const unsigned char *src) {
	unsigned char i = 0;
	unsigned char count, val;
	while (i < 240) {
		count = *src++;
		val = *src++;
		while (count--) {
			dest[i++] = val;
		}
	}
}

// Forward declarations (cc65 defaults to C89 rules; functions must be declared before use).
static char bg_collision_sub_any_ground(void);
static char has_ground_ahead(unsigned char dir);
static void vert_set_room_y(unsigned char screen_y);

void main(void) {
	ppu_off(); // screen off
	
	// Initialize MMC1
	mmc1_init();
	
	set_vram_buffer(); // do at least once
	
	load_title();
	
	// Set sprite pattern table to 1
	bank_spr(1);
	
	ppu_on_all(); // turn on screen
	
	scroll_x = 0;
	set_scroll_x(scroll_x);
	level = 0;
	
	has_turd_power = 1; // Default to having turd power
	turd_cooldown = 0;
	player_health = MAX_HEALTH; // Initialize player health
	restart_level = 0;
	damage_cooldown = 0;
	boss_health = BOSS_MAX_HEALTH; // Initialize boss health
	corn_mode = 0; // Initialize to turd mode
	
	direction = RIGHT; // Initialize direction to face right
	
	// Initialize turd projectiles
	for (index = 0; index < MAX_TURDS; ++index) {
		turd_active[index] = 0;
	}
	
	// Initialize player health and invincibility
	NINJA.health = MAX_HEALTH;
	NINJA.invincible = 0;
	
	while (1) {
		while (game_mode == MODE_TITLE) {
			ppu_wait_nmi();
			
			temp1 = get_frame_count();
			temp1 = (temp1 >> 3) & 3;
			pal_col(3,title_color_rotate[temp1]);
			
			
			pad1 = pad_poll(0); // read the first controller
			pad1_new = get_pad_new(0);
			if (pad1_new & PAD_START) {
				pal_fade_to(4,0); // fade to black
				ppu_off();
				
				// Clear all sprite and VRAM data
				oam_clear();
				clear_vram_buffer();
				
				// Set up MMC1 banks first
				mmc1_write(MMC1_CONTROL, 0x12);  // 4KB CHR mode
				mmc1_write(MMC1_CHR0, CHR_BANK_MAP);     // Pattern Table 0 (background)
				mmc1_write(MMC1_CHR1, CHR_BANK_SPRITES); // Pattern Table 1 (sprites)
				
				// Ensure sprite pattern table is set
				bank_spr(1);
				
				// level = 2;  // TESTING: Skip to end screen
				restart_level = 0;
				load_room();
				game_mode = MODE_GAME;
				pal_bg(palette_bg);
				song = SONG_GAME;
				music_play(song);
				scroll_x = 0;
				set_scroll_x(scroll_x);
				
				// Do one full update cycle while PPU is off
				check_spr_objects();
				draw_sprites();
				
				// Wait even longer before enabling PPU
				// for (temp1 = 0; temp1 < 8; ++temp1) {
				// 	ppu_wait_nmi();
				// }
				
				// Start with black screen
				pal_bright(0);
				ppu_on_all();
				
				// Only for initial level 1 load
				if (level == 0) {
					// Longer wait after PPU on
					for (temp1 = 0; temp1 < 8; ++temp1) {
						ppu_wait_nmi();
					}
					
					// Slower fade in
					for (bright = 0; bright < 5; ++bright) {
						// for (temp1 = 0; temp1 < 12; ++temp1) {
						// 	ppu_wait_nmi();
						// }
						pal_bright(bright);
					}
				} else {
					pal_bright(4);
				}
			}
		}
		
		

		while (game_mode == MODE_GAME) {
			ppu_wait_nmi();
			
			set_music_speed(8);
		
			pad1 = pad_poll(0); // read the first controller
			pad1_new = get_pad_new(0);
			
			movement();

			// Track when player jumps above screen vs falls below
			if (high_byte(NINJA.y) < 0xE0) above_screen = 0;
			else if (NINJA.vel_y < 0) above_screen = 1;

			// Fell off the bottom of the screen -> lose 2 HP and restart the level (or game over).
			if (high_byte(NINJA.y) >= 0xF0 && !above_screen) {
				if (player_health <= 2) player_health = 0;
				else player_health -= 2;

				if (player_health == 0) {
					death = 1;
				} else {
					game_mode = MODE_RESTART;
					break;
				}
			}
			
			check_spr_objects(); // see which objects are on screen
			
			sprite_collisions();
			
			// Update enemy bullets
			update_enemy_bullets();
			
			// set scroll
			if (is_vertical) {
				set_scroll_x(0);
				// For vertical scrolling: convert room-based scroll_y to PPU scroll
				// Low byte = Y offset in room (0-239), high byte & 1 = nametable select
				temp5 = scroll_y & 0xff;
				temp1 = (scroll_y >> 8) & 1;
				set_scroll_y(temp5 | ((unsigned int)temp1 << 8));

				if (U_D_switch) {
					draw_screen_D();
				} else {
					draw_screen_U();
				}
			} else {
				set_scroll_x(scroll_x);
				set_scroll_y(scroll_y);

				if (L_R_switch) {
					draw_screen_R();
				} else {
					draw_screen_L();
				}
			}

			draw_sprites();
			
			if (pad1_new & PAD_START) {
				game_mode = MODE_PAUSE;
				song = SONG_PAUSE;
				music_play(song);
				color_emphasis(COL_EMP_DARK);
				break; // out of the game loop
			}
			
			if (level_up) {
				game_mode = MODE_SWITCH;
				level_up = 0;
				bright = 4;
				bright_count = 0;
				++level;
			}
			else if (death) {
				death = 0;
				bright = 0;
				scroll_x = 0;
				set_scroll_x(scroll_x);
				ppu_off();
				clear_vram_buffer();
				
				// Set up CHR banks and palette BEFORE turning PPU back on
				mmc1_write(MMC1_CONTROL, 0x12);  // 4KB CHR mode
				mmc1_write(MMC1_CHR0, CHR_BANK_FONT);   // Font tiles
				mmc1_write(MMC1_CHR1, CHR_BANK_TITLE);  // Title graphics
				
				// Clear nametable and set palette
				vram_adr(NAMETABLE_A);
				vram_fill(0, 1024); // blank the screen
				pal_bg(palette_title);
				
				// Draw the game over text
				vram_adr(NTADR_A(12,14));
				multi_vram_buffer_horz(DEAD_TEXT, sizeof(DEAD_TEXT), NTADR_A(12,14));
				
				ppu_on_all();
				game_mode = MODE_GAME_OVER;
			}
		}
		
	
		// switch rooms, due to level++
		// also, death, restart level (removed feature)
		while (game_mode == MODE_SWITCH) { 
			ppu_wait_nmi();
			++bright_count;
			if (bright_count >= 0x10) { // fade out
				bright_count = 0;
				--bright;
				if (bright != 0xff) pal_bright(bright); // fade out
			}
			set_scroll_x(scroll_x);
			
			if (bright == 0xff) { // now switch rooms
				ppu_off();
				oam_clear();
				scroll_x = 0;
				set_scroll_x(scroll_x);
				if (level < 10) {
					load_room();
					game_mode = MODE_GAME;
					ppu_on_all();
					pal_bright(4); // back to normal brighness
				}
				else { // set end of game. Did we win?
					game_mode = MODE_END;
					vram_adr(NAMETABLE_A);
					vram_fill(0,1024);
					ppu_on_all();
					pal_bright(4); // back to normal brighness
				}
			}
		}

		// Restart current room without refilling HP (used when falling offscreen).
		while (game_mode == MODE_RESTART) {
			ppu_wait_nmi();
			ppu_off();
			oam_clear();

			scroll_x = 0;
			scroll_y = 0;
			set_scroll_x(scroll_x);
			set_scroll_y(scroll_y);

			// Clear any pending transitions so we truly restart the current level.
			level_up = 0;
			death = 0;

			restart_level = 1;
			load_room();
			restart_level = 0;

			pal_bg(palette_bg);
			ppu_on_all();
			pal_bright(4);
			game_mode = MODE_GAME;
		}
		
		
		
		while (game_mode == MODE_PAUSE) {
			ppu_wait_nmi();

			pad1 = pad_poll(0); // read the first controller
			pad1_new = get_pad_new(0);
			
			draw_sprites();
			
			if (pad1_new & PAD_START) {
				game_mode = MODE_GAME;
				song = SONG_GAME;
				music_play(song);
				color_emphasis(COL_EMP_NORMAL);
			}
		}
		
		while (game_mode == MODE_END) {
			ppu_wait_nmi();
			oam_clear();
			
			// Switch to CHR bank 0 for fonts/text and set up proper palette
			mmc1_write(MMC1_CONTROL, 0x12);  // 4KB CHR mode
			mmc1_write(MMC1_CHR0, CHR_BANK_FONT);   // Font tiles
			mmc1_write(MMC1_CHR1, CHR_BANK_TITLE);  // Title graphics
			
			// Set up palette for text
			pal_bg(palette_title);
			
			multi_vram_buffer_horz(END_TEXT, sizeof(END_TEXT), NTADR_A(6,13));
			multi_vram_buffer_horz(END_TEXT2, sizeof(END_TEXT2), NTADR_A(8,15));
			multi_vram_buffer_horz(END_TEXT3, sizeof(END_TEXT3), NTADR_A(11,17));
			temp1 = (coins / 10) + 0x10;  // Map 0-9 to 0x10-0x19 in current font bank
			temp2 = (coins % 10) + 0x10;
			one_vram_buffer(temp1, NTADR_A(18,17));
			one_vram_buffer(temp2, NTADR_A(19,17));
			
			set_scroll_x(0);
			
			music_stop();
            game_mode = MODE_RESET;
		}
		
		while (game_mode == MODE_GAME_OVER) {
			ppu_wait_nmi();
			oam_clear();
			set_scroll_x(0);
			
			// Wait a bit before stopping music and transitioning
			delay(60); // Wait 1 second
			music_stop();
			game_mode = MODE_RESET;
		}
        
        while (game_mode == MODE_RESET) {
            // just sit and wait for a while
            // then reset if start pressed
            delay(240); // 4 seconds
            delay(60); // 1 second
            
            while (1) {
                ppu_wait_nmi();
                pad1 = pad_poll(0); // read the first controller
                pad1_new = get_pad_new(0);
                if (pad1_new & PAD_START) {
                    ppu_off();
                    load_title();
                    ppu_on_all();
                    game_mode = MODE_TITLE;
                    coins = 0;
                    level = 0;
                    death = 0;
                    break;
                }
            }
        }
	}
}

// Helper function to convert digit to font tile
unsigned char digit_to_font_tile(unsigned char digit) {
    return digit + 0x10;  // 0x10-0x19 are the number tiles in the font bank
}

void load_title(void) {
	// Make sure MMC1 is in 4KB mode and set banks for title screen
	mmc1_write(MMC1_CONTROL, 0x12);  // 4KB CHR mode
	mmc1_write(MMC1_CHR0, CHR_BANK_FONT);   // Font tiles
	mmc1_write(MMC1_CHR1, CHR_BANK_TITLE);  // Title graphics
	
	pal_bg(palette_title);
	pal_spr(palette_sp);
	vram_adr(NAMETABLE_A);
	vram_unrle(title);
	game_mode = MODE_TITLE;
}

void load_room(void) {
	clear_vram_buffer();

	is_vertical = scroll_mode[level];

	if (is_vertical) {
		// Vertical level: start at the bottom room, scroll up
		// horizontal mirroring = nametables stacked top/bottom for vertical scrolling
		mmc1_write(MMC1_CONTROL, 0x13);  // 4KB CHR mode + horizontal mirroring
		mmc1_write(MMC1_CHR0, CHR_BANK_MAP);
		mmc1_write(MMC1_CHR1, CHR_BANK_SPRITES);
		bank_spr(1);

		// Start at the bottom room (last room)
		temp1 = scroll_limits[level]; // e.g. 8 for 9-room level

		// Load last two rooms: even room -> c_map, odd room -> c_map2
		if (temp1 & 1) {
			cmap2_room_id = temp1;
			cmap_room_id = temp1 - 1;
		} else {
			cmap_room_id = temp1;
			cmap2_room_id = temp1 > 0 ? temp1 - 1 : 0;
		}
		decompress_room(c_map, level_main_data[level][cmap_room_id]);
		decompress_room(c_map2, level_main_data[level][cmap2_room_id]);

		// Draw bottom room (temp1) to its nametable
		// For vertical scrolling with horizontal mirroring: nt0=$2000, nt2=$2800
		nt = temp1 & 1;
		if (nt) set_data_pointer(c_map2);
		else set_data_pointer(c_map);
		set_mt_pointer(metatiles1);
		for(y=0; ;y+=0x20) {
			for(x=0; ;x+=0x20) {
				address = get_ppu_addr(nt << 1, x, y);
				index = (y & 0xf0) + (x >> 4);
				buffer_4_mt(address, index);
				flush_vram_update2();
				if (x == 0xe0) break;
			}
			if (y == 0xe0) break;
		}

		// Draw room above (temp1-1) fully to the other nametable
		temp2 = temp1 - 1;
		nt = temp2 & 1;
		if (nt) set_data_pointer(c_map2);
		else set_data_pointer(c_map);
		for(y=0; ;y+=0x20) {
			for(x=0; ;x+=0x20) {
				address = get_ppu_addr(nt << 1, x, y);
				index = (y & 0xf0) + (x >> 4);
				buffer_4_mt(address, index);
				flush_vram_update2();
				if (x == 0xe0) break;
			}
			if (y == 0xe0) break;
		}

		// Set scroll to bottom room
		scroll_y = (unsigned int)temp1 << 8;
		scroll_x = 0;

		sprite_obj_init();
		NINJA.x = 0x7800; // center horizontally
		NINJA.y = 0xc400; // near bottom of screen
		NINJA.vel_x = 0;
		NINJA.vel_y = 0;
		above_screen = 0;

		map_loaded = 0;
		U_D_switch = 0; // default to scrolling up
		L_R_switch = 0;

	} else {
		// Horizontal level (original code)
		cmap_room_id = 0;
		cmap2_room_id = 1;

		mmc1_write(MMC1_CONTROL, 0x12);  // 4KB CHR mode + vertical mirroring
		mmc1_write(MMC1_CHR0, CHR_BANK_MAP);
		mmc1_write(MMC1_CHR1, CHR_BANK_SPRITES);
		bank_spr(1);

		decompress_room(c_map, level_main_data[level][0]);
		set_data_pointer(c_map);
		set_mt_pointer(metatiles1);
		for(y=0; ;y+=0x20) {
			for(x=0; ;x+=0x20) {
				address = get_ppu_addr(0, x, y);
				index = (y & 0xf0) + (x >> 4);
				buffer_4_mt(address, index);
				flush_vram_update2();
				if (x == 0xe0) break;
			}
			if (y == 0xe0) break;
		}

		decompress_room(c_map2, level_main_data[level][1]);
		set_data_pointer(c_map2);
		for(y=0; ;y+=0x20) {
			x = 0;
			address = get_ppu_addr(1, x, y);
			index = (y & 0xf0) + (x >> 4);
			buffer_4_mt(address, index);
			flush_vram_update2();
			if (y == 0xe0) break;
		}

		sprite_obj_init();
		NINJA.x = 0x4000;
		NINJA.y = 0xc400;
		NINJA.vel_x = 0;
		NINJA.vel_y = 0;
		above_screen = 0;

		map_loaded = 0;
		L_R_switch = 1;
	}

	if (!restart_level) player_health = MAX_HEALTH;
	damage_cooldown = 0;
	boss_health = BOSS_MAX_HEALTH;
	coyote_time = 0;
	was_jumping = 0;
}


void draw_sprites(void) {
	// clear all sprites from sprite buffer
	oam_clear();
	
	temp_x = high_byte(NINJA.x);
	if (temp_x > 0xfc) temp_x = 1;
	if (temp_x == 0) temp_x = 1;
	// draw 1 hero
	if (direction == LEFT) {
		oam_meta_spr_flip_h(temp_x, high_byte(NINJA.y), NinjaSprR);
	}
	else {
		oam_meta_spr(temp_x, high_byte(NINJA.y), NinjaSprR);
	}

	// Draw turds early so they render on top of level and enemy tiles
	// (lower OAM index = higher sprite priority on NES)
	draw_turds();

	// draw coin sprites
	for (index = 0; index < MAX_COINS; ++index) {
		temp_y = coin_y[index];
		if (temp_y == TURN_OFF) continue;
		if (!coin_active[index]) continue;
		temp_x = coin_x[index];
		if (temp_x > 0xf0) continue;
		
		if (temp_y < 0xf0) {
			if (coin_type[index] == COIN_REG) {
				// bounce the corn (center 8x8 sprite in 16x16 metatile)
				temp1 = get_frame_count();
				temp1 = (temp1 >> 2) & 7;
				temp1 = bounce[temp1];
				temp_y += temp1;
				oam_meta_spr(temp_x + 4, temp_y + 4, CoinSpr);
			}
			else if (coin_type[index] == COIN_END) {
				// Level exit marker (no bounce)
				oam_meta_spr(temp_x, temp_y, ExitSpr);
			}
			else {
				// bounce other coin-like objects
				temp1 = get_frame_count();
				temp1 = (temp1 >> 2) & 7;
				temp1 = bounce[temp1];
				temp_y += temp1;
				oam_meta_spr(temp_x, temp_y, BigCoinSpr);
			}
		}
	}

	// draw enemy sprites
	offset = get_frame_count() & 3;
	offset = offset << 4; // * 16, the size of the shuffle array
	for (index = 0; index < MAX_ENEMY; ++index) {
		index2 = shuffle_array[offset];
		++offset;
		temp_y = enemy_y[index2];
		if (temp_y == TURN_OFF) continue;
		if (!enemy_active[index2]) continue;
		temp_x = enemy_x[index2];
		if (temp_x == 0) temp_x = 1; // problems with x = 0
		if (temp_x > 0xf0) continue;
		if (temp_y < 0xf0) {
			if (enemy_dir[index2] == 1 &&
			           enemy_type[index2] != ENEMY_THORNS &&
			           enemy_type[index2] != ENEMY_RINGWORM) {
				if (enemy_type[index2] == ENEMY_BOSS1)
					oam_meta_spr_flip_h_boss(temp_x, temp_y, enemy_anim[index2]);
				else
					oam_meta_spr_flip_h(temp_x, temp_y, enemy_anim[index2]);
			} else {
				oam_meta_spr(temp_x, temp_y, enemy_anim[index2]);
			}
		}
	}
	
	// Draw enemy bullets
	draw_enemy_bullets();
	
	// Draw HUD elements as sprites (fixed position)

	// Draw health icon in upper left
	// oam_spr(0x10, 0x10, 0x20, 1); // Tile $20 for health icon
	oam_meta_spr(0x08, 0x10, TurdLifeSpr);
	
	// Draw health count
	temp1 = (player_health / 10) + 0xF0;
	temp2 = (player_health % 10) + 0xF0;
	oam_spr(0x16, 0x10, temp1, 1); // Tens digit
	oam_spr(0x1F, 0x10, temp2, 1); // Ones digit
	
	
	// Draw coin icon in upper right
	if (corn_mode) {
		oam_meta_spr(0xDD, 0x10, CornSelectedSpr);
	}
	else {
		oam_meta_spr(0xDD, 0x10, CoinSpr);
	}
	
	// Draw coin count
	temp1 = (coins / 10) + 0xF0; // Convert to tile number
	temp2 = (coins % 10) + 0xF0;
	oam_spr(0xE8, 0x10, temp1, 1); // Tens digit
	oam_spr(0xF0, 0x10, temp2, 1); // Ones digit
	
}

	
void movement(void) {
    // Declare all variables at the top
    char can_jump;
    unsigned int old_x;
    unsigned int temp5;
    unsigned char temp1;
    
    // handle x
    old_x = NINJA.x;
    
    if (pad1 & PAD_LEFT) {
		// Only change direction if not shooting (B button not held)
		if (!(pad1 & PAD_B)) {
			direction = LEFT;
		}
		
        if (NINJA.vel_x >= DECEL) {
            NINJA.vel_x -= DECEL;
        }
        else if (NINJA.vel_x > 0) {
            NINJA.vel_x = 0;
        }
		else {
			NINJA.vel_x -= ACCEL;
			if (NINJA.vel_x < -MAX_SPEED) NINJA.vel_x = -MAX_SPEED;
		}
	}
	else if (pad1 & PAD_RIGHT) {
		// Only change direction if not shooting (B button not held)
		if (!(pad1 & PAD_B)) {
			direction = RIGHT;
		}

		if (NINJA.vel_x <= DECEL) {
            NINJA.vel_x += DECEL;
        }
        else if (NINJA.vel_x < 0) {
            NINJA.vel_x = 0;
        }
		else {
			NINJA.vel_x += ACCEL;
			if (NINJA.vel_x >= MAX_SPEED) NINJA.vel_x = MAX_SPEED;
		}
	}
	else { // nothing pressed
		if (NINJA.vel_x >= ACCEL) NINJA.vel_x -= ACCEL;
		else if (NINJA.vel_x < -ACCEL) NINJA.vel_x += ACCEL;
		else NINJA.vel_x = 0;
	}
	
	NINJA.x += NINJA.vel_x;
	
	if (NINJA.x > 0xf000) { // too far, don't wrap around
        
        if (old_x >= 0x8000) {
            NINJA.x = 0xf000; // max right
        }
        else {
            NINJA.x = 0x0000; // max left
        }
        
		NINJA.vel_x = 0;
	} 
	
	ENTITY1.x = high_byte(NINJA.x); // this is much faster than passing a pointer to NINJA
	ENTITY1.y = high_byte(NINJA.y);
	ENTITY1.width = HERO_WIDTH;
	ENTITY1.height = HERO_HEIGHT;
	
    if (NINJA.vel_x < 0) {
        if (bg_coll_L() ) { // check collision left
            high_byte(NINJA.x) = high_byte(NINJA.x) - eject_L;
            NINJA.vel_x = 0;
            if (NINJA.x > 0xf000) {
                // no wrap around
                NINJA.x = 0xf000;
            }
        }
    }
    else if (NINJA.vel_x > 0) {
        if (bg_coll_R() ) { // check collision right
            high_byte(NINJA.x) = high_byte(NINJA.x) - eject_R;
            NINJA.vel_x = 0;
            if (NINJA.x > 0xf000) {
                // no wrap around
                NINJA.x = 0x0000;
            }
        }
    }
    // skip collision if vel = 0

	
	// handle y (gravity and jumping)
    if (NINJA.vel_y < 0x300) {
        NINJA.vel_y += GRAVITY;
    }
    else {
        NINJA.vel_y = 0x300; // terminal velocity
    }
    
    // Update y position
    NINJA.y += NINJA.vel_y;
    
    // Update collision box position
    ENTITY1.x = high_byte(NINJA.x);
    ENTITY1.y = high_byte(NINJA.y);
    
    // Check for collisions
    if (NINJA.vel_y > 0) {
        if (bg_coll_D()) { // check collision below
            high_byte(NINJA.y) = high_byte(NINJA.y) - eject_D;
            NINJA.y &= 0xff00;
            NINJA.vel_y = 0;
        }
    }
    else if (NINJA.vel_y < 0) {
        if (bg_coll_U()) { // check collision above
            high_byte(NINJA.y) = high_byte(NINJA.y) - eject_U;
            NINJA.vel_y = 0;
        }
    }
    
    // Check if we can jump (on ground)
    can_jump = bg_coll_D2();
    
    // Add coyote time - allow jumping for a few frames after leaving the ground
    if (can_jump) {
        coyote_time = 5; // Reset coyote time when on ground
    } else if (coyote_time > 0) {
        --coyote_time;
        can_jump = 1; // Allow jumping during coyote time
    }
    
    // Handle dropping through platforms with DOWN press
    if (drop_through > 0) {
        --drop_through;
    }
    if ((pad1_new & PAD_DOWN) && can_jump) {
        // Set drop_through timer so ninja falls through platforms
        drop_through = 12;
    }

    // Handle jumping - variable height: tap for short jump, hold for full jump
    if (pad1 & PAD_A) {
        if (can_jump && !was_jumping) {
            NINJA.vel_y = JUMP_VEL;
            sfx_play(SFX_JUMP, 0);
            was_jumping = 1;
        }
    } else {
        // When A is released early, cut upward velocity for a short hop
        if (was_jumping && NINJA.vel_y < -0x200) {
            NINJA.vel_y = -0x200;
        }
        was_jumping = 0;
    }

    if (is_vertical) {
		// VERTICAL SCROLLING
		// Load new collision map when entering a new vertical room
		if (U_D_switch && (scroll_y & 0xff) > 0xe8) {
			if (!map_loaded) {
				new_cmap_D();
				map_loaded = 1;
			}
		}
		else if (!U_D_switch && (scroll_y & 0xff) < 4) {
			if (!map_loaded) {
				new_cmap_U();
				map_loaded = 1;
			}
		}
		else {
			map_loaded = 0;
		}

		// Vertical scroll: camera follows player up/down
		temp6 = NINJA.y;
		if (NINJA.y < MIN_UP && scroll_y > 0) {
			temp1 = (MIN_UP - NINJA.y) >> 8;
			if (temp1 > 3) temp1 = 3;
			if (temp1 > scroll_y) temp1 = scroll_y;
			scroll_y -= temp1;
			// Skip the 240-255 dead zone (NES rooms are 240px, not 256)
			if ((scroll_y & 0xff) >= 240) scroll_y -= 16;
			high_byte(NINJA.y) = high_byte(NINJA.y) + temp1;
			U_D_switch = 0; // scrolling up
		}
		else if (NINJA.y > MAX_DOWN) {
			temp1 = (NINJA.y - MAX_DOWN) >> 8;
			if (temp1 > 3) temp1 = 3;
			scroll_y += temp1;
			// Skip the 240-255 dead zone
			if ((scroll_y & 0xff) >= 240) scroll_y += 16;
			high_byte(NINJA.y) = high_byte(NINJA.y) - temp1;
			U_D_switch = 1; // scrolling down
		}

		if (scroll_y >= MAX_SCROLL_V) {
			scroll_y = MAX_SCROLL_V;
			NINJA.y = temp6;
			if (high_byte(NINJA.y) >= 0xf1) {
				NINJA.y = 0xf100;
			}
		}
		// Don't scroll past top
		if (scroll_y > 0xF000) { // unsigned underflow check
			scroll_y = 0;
			NINJA.y = temp6;
			// Clamp ninja to top of screen
			if (high_byte(NINJA.y) < 8) {
				NINJA.y = 0x0800;
			}
		}

	} else {
		// HORIZONTAL SCROLLING (original code)
		if (L_R_switch && (scroll_x & 0xff) < 4) {
			if (!map_loaded) {
				new_cmap();
				map_loaded = 1;
			}
		}
		else if (!L_R_switch && (scroll_x & 0xff) > 0xfb) {
			if (!map_loaded) {
				new_cmap_L();
				map_loaded = 1;
			}
		}
		else {
			map_loaded = 0;
		}

		temp5 = NINJA.x;
		if (NINJA.x > MAX_RIGHT) {
			temp1 = (NINJA.x - MAX_RIGHT) >> 8;
			if (temp1 > 3) temp1 = 3;
			scroll_x += temp1;
			high_byte(NINJA.x) = high_byte(NINJA.x) - temp1;
			L_R_switch = 1;
		}
		else if (NINJA.x < MIN_LEFT && scroll_x > 0) {
			temp1 = (MIN_LEFT - NINJA.x) >> 8;
			if (temp1 > 3) temp1 = 3;
			if (temp1 > scroll_x) temp1 = scroll_x;
			scroll_x -= temp1;
			high_byte(NINJA.x) = high_byte(NINJA.x) + temp1;
			L_R_switch = 0;
		}

		if (scroll_x >= MAX_SCROLL_H) {
			scroll_x = MAX_SCROLL_H;
			NINJA.x = temp5;
			if (high_byte(NINJA.x) >= 0xf1) {
				NINJA.x = 0xf100;
			}
		}
	}

	// Handle turd firing
	if (turd_cooldown > 0) {
		--turd_cooldown;
	}
	
	// Debug: SELECT+START = skip to next level
	if ((pad1_new & PAD_SELECT) && (pad1 & PAD_START)) {
		++level;
		game_mode = MODE_SWITCH;
		bright = 4;
		bright_count = 0;
	}
	// Toggle corn mode with Select button (only if START not held)
	else if (pad1_new & PAD_SELECT) {
		corn_mode = !corn_mode; // Toggle between 0 and 1
		sfx_play(SFX_DING, 0); // Play a sound to indicate mode change
	}
	
	// Automatically switch back to turd mode if corn count is 0
	if (corn_mode && coins == 0) {
		corn_mode = 0;
		sfx_play(SFX_DING, 0); // Play a sound to indicate mode change
	}
	
	// Continuous turd firing when B is held down
	if (has_turd_power && (pad1 & PAD_B) && turd_cooldown == 0) {
		// Check if we have enough corn in corn mode
		if (corn_mode && coins > 0) {
			fire_turd();
			--coins; // Use one corn
			turd_cooldown = TURD_COOLDOWN;
		} else if (!corn_mode) {
			fire_turd();
			turd_cooldown = TURD_COOLDOWN;
		}
	}
	
	// Update active turds
	update_turds();
}	


void check_spr_objects(void) {
	++enemy_frames;
	ENTITY2.x = high_byte(NINJA.x);

	for (index = 0; index < MAX_COINS; ++index) {
		coin_active[index] = 0;
		if (coin_y[index] != TURN_OFF) {
			if (is_vertical) {
				// Vertical: check Y visibility (room+y_offset vs scroll_y)
				high_byte(temp5) = coin_room[index];
				low_byte(temp5) = coin_actual_y[index];
				coin_active[index] = get_position_vert();
				if (coin_active[index]) {
					coin_y[index] = temp_y; // computed screen Y
					coin_x[index] = coin_actual_x[index]; // X is fixed (no scroll)
				}
			} else {
				// Horizontal: check X visibility
				high_byte(temp5) = coin_room[index];
				low_byte(temp5) = coin_actual_x[index];
				coin_active[index] = get_position();
				coin_x[index] = temp_x;
			}
		}
	}

	for (index = 0; index < MAX_ENEMY; ++index) {
		enemy_active[index] = 0;
		if (enemy_y[index] != TURN_OFF) {
			if (is_vertical) {
				high_byte(temp5) = enemy_room[index];
				low_byte(temp5) = enemy_actual_y[index];
				temp1 = enemy_active[index] = get_position_vert();
				if (temp1 == 0) {
					continue;
				}
				enemy_y[index] = temp_y;
				enemy_x[index] = enemy_actual_x[index];
			} else {
				high_byte(temp5) = enemy_room[index];
				low_byte(temp5) = enemy_actual_x[index];
				temp1 = enemy_active[index] = get_position();
				if (temp1 == 0) {
					continue;
				}
				enemy_x[index] = temp_x;
			}

			// Run enemy AI at 30fps (skip odd frames)
			// Position updates still 60fps above
			if (enemy_frames & 1) continue;
			enemy_moves();
		}
	}
}


char get_position(void) {
	// Horizontal: is the object within one screen of scroll_x?
	temp5 -= scroll_x;
	temp_x = temp5 & 0xff;
	if (high_byte(temp5)) return 0;
	return 1;
}


char get_position_vert(void) {
	// Vertical: convert from 256-per-room encoding to linear pixels (240 per room)
	// linear = room * 240 + offset = (room << 8) - (room << 4) + offset
	// This eliminates the 16-pixel-per-room error in the subtraction.
	temp1 = high_byte(temp5);
	temp5 -= (unsigned int)temp1 << 4; // object linear Y

	temp1 = scroll_y >> 8;
	temp6 = scroll_y - ((unsigned int)temp1 << 4); // scroll linear Y

	temp5 -= temp6; // screen Y = object_linear - scroll_linear
	temp_y = temp5 & 0xff;
	if (high_byte(temp5)) return 0;
	return 1;
}


void enemy_moves(void) {
	if (enemy_type[index] == ENEMY_BOSS1) {
		ENTITY1.x = enemy_x[index];
		ENTITY1.y = enemy_y[index] + 28;
		ENTITY1.width = 28;
		ENTITY1.height = 4;
		if (enemy_frames & 1) return;
		// Face the ninja (once, at end of jump logic)
		enemy_anim[index] = Boss1SprL;
		enemy_dir[index] = (enemy_x[index] > ENTITY2.x) ? 0 : 1;
		temp1 = (enemy_frames + (index << 3)) & 0x3f;
		if (temp1 < 8) {
			if (enemy_bullet_cooldown[index] == 0)
				fire_enemy_bullet(index, BULLET_LINEAR);
		} else if (temp1 < 14) {
			enemy_y[index] -= 2;
		} else if (temp1 < 24) {
			enemy_y[index] -= 2;
			if (temp1 == 20 && enemy_bullet_cooldown[index] == 0)
				fire_enemy_bullet(index, BULLET_THROW);
		} else if (temp1 < 26) {
			--enemy_y[index];
		} else {
			++enemy_y[index];
			if (temp1 < 62) ++enemy_y[index];
			ENTITY1.x = enemy_x[index];
			ENTITY1.y = enemy_y[index] + 28;
			ENTITY1.width = 28;
			ENTITY1.height = 4;
			if (bg_coll_D()) enemy_y[index] -= eject_D;
		}
		if (enemy_bullet_cooldown[index] > 0) --enemy_bullet_cooldown[index];
		if (enemy_x[index] > ENTITY2.x) {
			ENTITY1.x -= 1; bg_collision_fast();
			if (collision_L) return;
			if (enemy_actual_x[index] == 0) --enemy_room[index];
			--enemy_actual_x[index];
		} else if (enemy_x[index] < ENTITY2.x) {
			ENTITY1.x += 1; bg_collision_fast();
			if (collision_R) return;
			++enemy_actual_x[index];
			if (enemy_actual_x[index] == 0) ++enemy_room[index];
		}
	}
	else if (enemy_type[index] == ENEMY_WASP) {
		//for bg collisions
		ENTITY1.x = enemy_x[index];
		ENTITY1.y = enemy_y[index] + 6; // mid point
		ENTITY1.width = 13;
        ENTITY1.height = 15;
        
        // note, ENTITY2 is the hero's x position
		
		if (enemy_frames & 1) return; // half speed
		if (enemy_x[index] > ENTITY2.x) {
			ENTITY1.x -= 1;
            bg_collision_fast();
			if (collision_L) return;
			if (enemy_actual_x[index] == 0) --enemy_room[index];
			--enemy_actual_x[index];
			enemy_anim[index] = EnemyWaspSprL;
			enemy_dir[index] = 0;
		}
		else if (enemy_x[index] < ENTITY2.x) {
			ENTITY1.x += 1;
            bg_collision_fast();
			if (collision_R) return;
			++enemy_actual_x[index];
			if (enemy_actual_x[index] == 0) ++enemy_room[index];
			enemy_anim[index] = EnemyWaspSprL;
			enemy_dir[index] = 1;
		}
	}
	else if (enemy_type[index] == ENEMY_BOUNCE) {
		temp1 = (enemy_frames + (index << 3)) & 0x3f;
		enemy_anim[index] = (temp1 < 16 || (temp1 >= 42 && temp1 < 44)) ? EnemyBounceSpr : EnemyBounceSpr2;
		if (temp1 >= 16 && temp1 < 44) {
			--enemy_y[index];
			if (temp1 < 22) --enemy_y[index];
		} else if (temp1 >= 44) {
			++enemy_y[index];
			if (temp1 < 62) ++enemy_y[index];
			ENTITY1.x = enemy_x[index]; ENTITY1.y = enemy_y[index];
			ENTITY1.width = 15; ENTITY1.height = 14;
			if (bg_coll_D()) enemy_y[index] -= eject_D;
		}
	}
	else if (enemy_type[index] == ENEMY_WORM) {
		// for bg collisions
		ENTITY1.x = enemy_x[index];
		ENTITY1.y = enemy_y[index];
		ENTITY1.width = 13;
		ENTITY1.height = 13;

		if (enemy_frames & 1) return; // half speed
		if (enemy_dir[index] == 0) { // Moving left
			if (!has_ground_ahead(0)) {
				enemy_dir[index] = 1;
				return;
			}
			ENTITY1.x -= 1;
			bg_collision_fast();
			if (collision_L) {
				enemy_dir[index] = 1;
				return;
			}
			if (enemy_actual_x[index] == 0) --enemy_room[index];
			--enemy_actual_x[index];
		} else { // Moving right
			if (!has_ground_ahead(1)) {
				enemy_dir[index] = 0;
				return;
			}
			ENTITY1.x += 1;
			bg_collision_fast();
			if (collision_R) {
				enemy_dir[index] = 0;
				return;
			}
			++enemy_actual_x[index];
			if (enemy_actual_x[index] == 0) ++enemy_room[index];
		}
		enemy_anim[index] = EnemyWormSprL1;
	}
	else if (enemy_type[index] == ENEMY_THORNS) {
		// Stationary; toggle spikes based on timer
		if (enemy_thorn_timer[index] > 0) {
			--enemy_thorn_timer[index];
		}
		if (enemy_thorn_timer[index] == 0) {
			if (enemy_thorn_out[index]) {
				enemy_thorn_out[index] = 0;
				enemy_thorn_timer[index] = enemy_thorn_delay[index] ? enemy_thorn_delay[index] : (4 * 60);
				enemy_anim[index] = EnemyThornsInSpr;
			} else {
				enemy_thorn_out[index] = 1;
				enemy_thorn_timer[index] = 120;
				enemy_anim[index] = EnemyThornsOutSpr;
			}
		}
	}
	else if (enemy_type[index] == ENEMY_RINGWORM) {
		enemy_anim[index] = EnemyRingWormSpr;
		// X: bounce off screen edges (stay visible once activated)
		temp1 = enemy_x[index] + enemy_vel_x[index];
		if (temp1 < 4 || temp1 > 240)
			enemy_vel_x[index] = -enemy_vel_x[index];
		else {
			enemy_x[index] = temp1;
			enemy_actual_x[index] += enemy_vel_x[index];
			if (enemy_vel_x[index] < 0 && enemy_actual_x[index] > 240) --enemy_room[index];
			if (enemy_vel_x[index] > 0 && enemy_actual_x[index] < 16) ++enemy_room[index];
		}
		// Y: bounce off floor/ceiling tiles
		temp1 = enemy_y[index] + enemy_vel_y[index];
		if (temp1 < 8 || temp1 > 220)
			enemy_vel_y[index] = -enemy_vel_y[index];
		else {
			enemy_y[index] = temp1;
			enemy_actual_y[index] += enemy_vel_y[index];
		}
	}
	else if (enemy_type[index] == ENEMY_HOPWORM) {
		ENTITY1.x = enemy_x[index]; ENTITY1.y = enemy_y[index];
		ENTITY1.width = 13; ENTITY1.height = 13;
		enemy_dir[index] = (enemy_x[index] > ENTITY2.x) ? 0 : 1;
		enemy_anim[index] = EnemyHopWormSprL;
		// Gravity
		if (++enemy_vel_y[index] > 5) enemy_vel_y[index] = 5;
		ENTITY1.y += enemy_vel_y[index];
		coll_enemy = 1;
		if (enemy_vel_y[index] > 0 && bg_coll_D()) {
			ENTITY1.y -= eject_D; enemy_vel_y[index] = 0; enemy_state[index] = 0;
		}
		coll_enemy = 0;
		enemy_y[index] = ENTITY1.y; enemy_actual_y[index] = ENTITY1.y;
		// Hop or move
		if (enemy_state[index] == 0) {
			if (((enemy_frames + (index << 3)) & 0x3f) == 0) { enemy_state[index] = 1; enemy_vel_y[index] = -6; }
		} else {
			ENTITY1.x = enemy_x[index]; ENTITY1.y = enemy_y[index];
			if (enemy_dir[index] == 0) {
				ENTITY1.x -= 1; bg_collision_fast();
				if (!collision_L) { if (enemy_actual_x[index] == 0) --enemy_room[index]; --enemy_actual_x[index]; }
			} else {
				ENTITY1.x += 1; bg_collision_fast();
				if (!collision_R) { ++enemy_actual_x[index]; if (enemy_actual_x[index] == 0) ++enemy_room[index]; }
			}
		}
	}
	else if (enemy_type[index] == ENEMY_POOD) {
		ENTITY1.x = enemy_x[index]; ENTITY1.y = enemy_y[index];
		ENTITY1.width = 13; ENTITY1.height = 13;
		enemy_anim[index] = (enemy_frames & 0x08) ? EnemyPoodSprL1 : EnemyPoodSprL2;
		// Gravity
		if (++enemy_vel_y[index] > 5) enemy_vel_y[index] = 5;
		ENTITY1.y += enemy_vel_y[index];
		coll_enemy = 1;
		if (enemy_vel_y[index] > 0 && bg_coll_D()) { ENTITY1.y -= eject_D; enemy_vel_y[index] = 0; }
		coll_enemy = 0;
		enemy_y[index] = ENTITY1.y; enemy_actual_y[index] = ENTITY1.y;
		// Proximity check
		temp1 = enemy_x[index] > ENTITY2.x ? enemy_x[index] - ENTITY2.x : ENTITY2.x - enemy_x[index];
		if (temp1 < 64) enemy_state[index] = 1;
		if (enemy_state[index] == 0) return;
		// Chase
		enemy_dir[index] = (enemy_x[index] > ENTITY2.x) ? 0 : 1;
		ENTITY1.x = enemy_x[index]; ENTITY1.y = enemy_y[index];
		if (enemy_dir[index] == 0) {
			ENTITY1.x -= 1; bg_collision_fast();
			if (!collision_L) { if (enemy_actual_x[index] == 0) --enemy_room[index]; --enemy_actual_x[index]; }
		} else {
			ENTITY1.x += 1; bg_collision_fast();
			if (!collision_R) { ++enemy_actual_x[index]; if (enemy_actual_x[index] == 0) ++enemy_room[index]; }
		}
		// Jump (less frequent)
		if (enemy_vel_y[index] == 0 && ((enemy_frames + (index << 3)) & 0x7f) == 0) enemy_vel_y[index] = -6;
	}
	else if (enemy_type[index] == ENEMY_SPIDER) {
		enemy_anim[index] = EnemySpiderSpr;
		ENTITY1.x = enemy_x[index]; ENTITY1.y = enemy_y[index];
		ENTITY1.width = 13; ENTITY1.height = 13;
		temp1 = enemy_x[index] > ENTITY2.x ?
			enemy_x[index] - ENTITY2.x : ENTITY2.x - enemy_x[index];
		if (enemy_state[index] == 0) {
			if (temp1 < 40) { enemy_state[index] = 1; enemy_vel_y[index] = 2; }
		} else if (enemy_state[index] == 1) {
			ENTITY1.y += enemy_vel_y[index];
			if (enemy_vel_y[index] < 5) ++enemy_vel_y[index];
			coll_enemy = 1; temp1 = bg_coll_D(); coll_enemy = 0;
			if (temp1) ENTITY1.y -= eject_D;
			enemy_y[index] = ENTITY1.y; enemy_actual_y[index] = ENTITY1.y;
			if (temp1 || ENTITY1.y > 220)
				enemy_state[index] = 2;
		} else {
			if (enemy_y[index] > enemy_origin_y[index]) enemy_y[index] -= 2;
			else enemy_state[index] = 0;
		}
	}
	else if (enemy_type[index] == ENEMY_FLYBUG) {
		enemy_anim[index] = EnemyFlyBugSprL;
		if (enemy_dir[index] == 0) { --enemy_actual_x[index]; if (enemy_x[index] < 8) enemy_dir[index] = 1; }
		else { ++enemy_actual_x[index]; if (enemy_x[index] > 232) enemy_dir[index] = 0; }
	}
	else if (enemy_type[index] == ENEMY_VIRUS) {
		enemy_anim[index] = EnemyVirusSprL;
	}
}


void bg_collision_fast(void) {
	// rewrote this for enemies, bg_collision was too slow
    // test 1 point on each side

	collision_L = 0;
	collision_R = 0;

	if (ENTITY1.y >= 0xf0) return;

	temp5 = ENTITY1.x + scroll_x;
	temp_x = temp5 & 0xff; // low byte x
	temp_room = temp5 >> 8; // high byte x

	if (is_vertical) {
		temp6 = temp5; // save X before vert_set_room_y clobbers temp5
		vert_set_room_y(ENTITY1.y + 6);
		temp5 = temp6; // restore X
	} else {
		temp_y = ENTITY1.y + 6; // y middle
	}

	if (bg_collision_sub() & COLLISION_SOLID) {
		++collision_L;
	}

	// right side
	temp5 += ENTITY1.width;
	temp_x = temp5 & 0xff; // low byte x
	if (!is_vertical) temp_room = temp5 >> 8; // X-based room (horiz only)

	// temp_y and temp_room (vertical) are unchanged
	if (bg_collision_sub() & COLLISION_SOLID) {
		++collision_R;
	}
}



// Helper: convert screen_y to (temp_room, temp_y) for vertical mode
// screen_y + scroll_y_offset -> room number and y-within-room
static void vert_set_room_y(unsigned char screen_y) {
    temp5 = (unsigned int)screen_y + (scroll_y & 0xff);
    temp_room = scroll_y >> 8;
    if (temp5 >= 240) {
        ++temp_room;
        temp_y = (unsigned char)(temp5 - 240);
    } else {
        temp_y = (unsigned char)temp5;
    }
}

char bg_coll_L(void) {
    // check 2 points on the left side
    temp5 = ENTITY1.x + scroll_x;
    temp_x = (char)temp5;
    temp_room = temp5 >> 8;

    eject_L = temp_x | 0xf0;

    if (is_vertical) {
        vert_set_room_y(ENTITY1.y + 2);
    } else {
        temp_y = ENTITY1.y + 2;
    }
    if (bg_collision_sub() & COLLISION_SOLID) return 1;

    if (is_vertical) {
        vert_set_room_y(ENTITY1.y + ENTITY1.height - 2);
    } else {
        temp_y = ENTITY1.y + ENTITY1.height - 2;
    }
    if (bg_collision_sub() & COLLISION_SOLID) return 1;

    return 0;
}

char bg_coll_R(void) {
    // check 2 points on the right side
    temp5 = ENTITY1.x + scroll_x + ENTITY1.width;
    temp_x = (char)temp5;
    temp_room = temp5 >> 8;

    eject_R = (temp_x + 1) & 0x0f;

    if (is_vertical) {
        vert_set_room_y(ENTITY1.y + 2);
    } else {
        temp_y = ENTITY1.y + 2;
    }
    if (bg_collision_sub() & COLLISION_SOLID) return 1;

    if (is_vertical) {
        vert_set_room_y(ENTITY1.y + ENTITY1.height - 2);
    } else {
        temp_y = ENTITY1.y + ENTITY1.height - 2;
    }
    if (bg_collision_sub() & COLLISION_SOLID) return 1;

    return 0;
}

char bg_coll_U(void) {
    // check 2 points on the top side
    temp5 = ENTITY1.x + scroll_x + 2;
    temp_x = (char)temp5;
    temp_room = temp5 >> 8;

    if (is_vertical) {
        vert_set_room_y(ENTITY1.y);
        eject_U = temp_y | 0xf0;
    } else {
        temp_y = ENTITY1.y;
        eject_U = temp_y | 0xf0;
    }
    if (bg_collision_sub() & COLLISION_SOLID) return 1;

    temp5 = ENTITY1.x + scroll_x + ENTITY1.width - 2;
    temp_x = (char)temp5;
    if (!is_vertical) temp_room = temp5 >> 8;
    // In vertical mode, temp_room stays from vert_set_room_y

    if (bg_collision_sub() & COLLISION_SOLID) return 1;

    return 0;
}

char bg_coll_D(void) {
    // check 2 points on the bottom side
    temp5 = ENTITY1.x + scroll_x + 2;
    temp_x = (char)temp5;
    temp_room = temp5 >> 8;

    if (is_vertical) {
        vert_set_room_y(ENTITY1.y + ENTITY1.height);
    } else {
        temp_y = ENTITY1.y + ENTITY1.height;
    }

    if (!coll_enemy && (temp_y & 0x0f) > 3) return 0;
    eject_D = (temp_y + 1) & 0x0f;

    if (bg_collision_sub() & (COLLISION_SOLID | COLLISION_PLATFORM)) return 1;

    temp5 = ENTITY1.x + scroll_x + ENTITY1.width - 2;
    temp_x = (char)temp5;
    if (!is_vertical) temp_room = temp5 >> 8;

    if (bg_collision_sub() & (COLLISION_SOLID | COLLISION_PLATFORM)) return 1;

    return 0;
}

char bg_coll_D2(void) {
    // check 2 points on the bottom side, a little lower for jumping
    temp5 = ENTITY1.x + scroll_x + 2;
    temp_x = (char)temp5;
    temp_room = temp5 >> 8;

    if (is_vertical) {
        vert_set_room_y(ENTITY1.y + ENTITY1.height + 2);
    } else {
        temp_y = ENTITY1.y + ENTITY1.height + 2;
    }
    if (bg_collision_sub() & (COLLISION_SOLID | COLLISION_PLATFORM)) return 1;

    temp5 = ENTITY1.x + scroll_x + ENTITY1.width - 2;
    temp_x = (char)temp5;
    if (!is_vertical) temp_room = temp5 >> 8;

    if (bg_collision_sub() & (COLLISION_SOLID | COLLISION_PLATFORM)) return 1;

    return 0;
}




char bg_collision_sub(void) {
    if (temp_y >= 0xf0) return 0;
    
    coordinates = (temp_x >> 4) + (temp_y & 0xf0);
    // we just need 4 bits each from x and y
    
    map = temp_room&1; // high byte
    if (!map) {
        temp1 = c_map[coordinates];
    }
    else {
        temp1 = c_map2[coordinates];
    }
    
    // Check collision type based on column position
    if (IS_SOLID(temp1)) {
        return COLLISION_SOLID;  // Solid collision from all sides
    }
    else if (IS_PLATFORM(temp1)) {
        // Enemies always land on platforms
        if (coll_enemy) {
            return COLLISION_PLATFORM;
        }
        // For ninja, only return platform collision if falling (vel_y > 0) and not dropping through
        if (ENTITY1.width == HERO_WIDTH) {
            if (NINJA.vel_y > 0 && drop_through == 0) {
                return COLLISION_PLATFORM;
            }
        }
        // Turds pass through platforms entirely (no collision)
        else if (ENTITY1.width == TURD_WIDTH) {  // This is a turd
            // Do nothing - turds ignore platforms
        }
        else if (ENTITY1.width == ENEMY_BULLET_WIDTH) {  // This is an enemy bullet
            // Find which bullet this is by checking its position
            for (index2 = 0; index2 < MAX_ENEMY_BULLETS; ++index2) {
                if (enemy_bullet_active[index2] && 
                    enemy_bullet_x[index2] == ENTITY1.x && 
                    enemy_bullet_y[index2] == ENTITY1.y) {
                    if (enemy_bullet_vel_y[index2] > 0) {
                        return COLLISION_PLATFORM;
                    }
                    break;
                }
            }
        }
        return 0;  // No collision when moving up or horizontally
    }
    
    return 0;  // No collision with background tiles
}

// Background collision check that treats SOLID and PLATFORM as ground for *any* entity type.
// Used by enemies for ledge detection (they should turn around instead of walking off).
static char bg_collision_sub_any_ground(void) {
	if (temp_y >= 0xf0) return 0;

	coordinates = (temp_x >> 4) + (temp_y & 0xf0);
	map = temp_room & 1;
	if (!map) temp1 = c_map[coordinates];
	else      temp1 = c_map2[coordinates];

	if (IS_SOLID(temp1) || IS_PLATFORM(temp1)) return 1;
	return 0;
}

// Check whether there is ground directly below the "front foot" of the current ENTITY1.
// dir: 0 = left, 1 = right. Returns 1 if ground exists, 0 if ledge.
static char has_ground_ahead(unsigned char dir) {
	unsigned int wx;
	unsigned char probe_x;
	unsigned char probe_y;

	if (dir == 0) probe_x = ENTITY1.x - 1;
	else          probe_x = ENTITY1.x + ENTITY1.width + 1;

	probe_y = ENTITY1.y + ENTITY1.height + 4;
	if (probe_y >= 0xf0) return 0;

	wx = (unsigned int)probe_x + scroll_x;
	temp_x = (unsigned char)wx;
	if (is_vertical) {
		temp_room = scroll_y >> 8; // room from vertical scroll
		vert_set_room_y(probe_y);
	} else {
		temp_room = wx >> 8;
		temp_y = probe_y;
	}

	return bg_collision_sub_any_ground();
}



void draw_screen_R(void) {
	// scrolling to the right, draw metatiles as we go
	pseudo_scroll_x = scroll_x + 0x120;
	
	temp1 = pseudo_scroll_x >> 8;
	if (temp1 >= 8) temp1 = 7;

	// Use c_map (even rooms) or c_map2 (odd rooms) for rendering.
	// Decompress on demand if the needed room isn't already loaded.
	if (temp1 & 1) {
		if (temp1 != cmap2_room_id) {
			decompress_room(c_map2, level_main_data[level][temp1]);
			cmap2_room_id = temp1;
		}
		set_data_pointer(c_map2);
	} else {
		if (temp1 != cmap_room_id) {
			decompress_room(c_map, level_main_data[level][temp1]);
			cmap_room_id = temp1;
		}
		set_data_pointer(c_map);
	}
	nt = temp1 & 1;
	x = pseudo_scroll_x & 0xff;
	
	switch(scroll_count) {
		case 0:
			address = get_ppu_addr(nt, x, 0);
			index = 0 + (x >> 4);
			buffer_4_mt(address, index); // ppu_address, index to the data
			
			address = get_ppu_addr(nt, x, 0x20);
			index = 0x20 + (x >> 4);
			buffer_4_mt(address, index); // ppu_address, index to the data
			break;
			
		case 1:
			address = get_ppu_addr(nt, x, 0x40);
			index = 0x40 + (x >> 4);
			buffer_4_mt(address, index); // ppu_address, index to the data
			
			address = get_ppu_addr(nt, x, 0x60);
			index = 0x60 + (x >> 4);
			buffer_4_mt(address, index); // ppu_address, index to the data
			break;
			
		case 2:
			address = get_ppu_addr(nt, x, 0x80);
			index = 0x80 + (x >> 4);
			buffer_4_mt(address, index); // ppu_address, index to the data
			
			address = get_ppu_addr(nt, x, 0xa0);
			index = 0xa0 + (x >> 4);
			buffer_4_mt(address, index); // ppu_address, index to the data
			break;
			
		default:
			address = get_ppu_addr(nt, x, 0xc0);
			index = 0xc0 + (x >> 4);
			buffer_4_mt(address, index); // ppu_address, index to the data
			
			address = get_ppu_addr(nt, x, 0xe0);
			index = 0xe0 + (x >> 4);
			buffer_4_mt(address, index); // ppu_address, index to the data
	}


	++scroll_count;
	scroll_count &= 3; //mask off top bits, keep it 0-3
}


void draw_screen_L(void) {
	// scrolling to the left, draw metatiles behind the left edge
	if (scroll_x < 0x10) return; // nothing to draw at far left

	pseudo_scroll_x = scroll_x - 0x10;

	temp1 = pseudo_scroll_x >> 8;

	// Use c_map (even rooms) or c_map2 (odd rooms) for rendering.
	// Decompress on demand if the needed room isn't already loaded.
	if (temp1 & 1) {
		if (temp1 != cmap2_room_id) {
			decompress_room(c_map2, level_main_data[level][temp1]);
			cmap2_room_id = temp1;
		}
		set_data_pointer(c_map2);
	} else {
		if (temp1 != cmap_room_id) {
			decompress_room(c_map, level_main_data[level][temp1]);
			cmap_room_id = temp1;
		}
		set_data_pointer(c_map);
	}
	nt = temp1 & 1;
	x = pseudo_scroll_x & 0xff;

	switch(scroll_count) {
		case 0:
			address = get_ppu_addr(nt, x, 0);
			index = 0 + (x >> 4);
			buffer_4_mt(address, index);

			address = get_ppu_addr(nt, x, 0x20);
			index = 0x20 + (x >> 4);
			buffer_4_mt(address, index);
			break;

		case 1:
			address = get_ppu_addr(nt, x, 0x40);
			index = 0x40 + (x >> 4);
			buffer_4_mt(address, index);

			address = get_ppu_addr(nt, x, 0x60);
			index = 0x60 + (x >> 4);
			buffer_4_mt(address, index);
			break;

		case 2:
			address = get_ppu_addr(nt, x, 0x80);
			index = 0x80 + (x >> 4);
			buffer_4_mt(address, index);

			address = get_ppu_addr(nt, x, 0xa0);
			index = 0xa0 + (x >> 4);
			buffer_4_mt(address, index);
			break;

		default:
			address = get_ppu_addr(nt, x, 0xc0);
			index = 0xc0 + (x >> 4);
			buffer_4_mt(address, index);

			address = get_ppu_addr(nt, x, 0xe0);
			index = 0xe0 + (x >> 4);
			buffer_4_mt(address, index);
	}

	++scroll_count;
	scroll_count &= 3;
}


void new_cmap(void) {
	// copy a new collision map to one of the 2 c_map arrays
	room = ((scroll_x >> 8) +1); //high byte = room, one to the right

	// Clamp in case room points past the end.
	if (room >= 8) room = 7;

	map = room & 1; //even or odd?
	if (!map) {
		decompress_room(c_map, level_main_data[level][room]);
		cmap_room_id = room;
	} else {
		decompress_room(c_map2, level_main_data[level][room]);
		cmap2_room_id = room;
	}
}


void new_cmap_L(void) {
	// load collision map for the room we just entered scrolling left
	room = (scroll_x >> 8);
	if (room >= 8) room = 7;

	map = room & 1;
	if (!map) {
		decompress_room(c_map, level_main_data[level][room]);
		cmap_room_id = room;
	} else {
		decompress_room(c_map2, level_main_data[level][room]);
		cmap2_room_id = room;
	}
}


void draw_screen_D(void) {
	// Scrolling down - draw rows below the visible area
	// Look ahead by 0x120 pixels below current scroll position
	pseudo_scroll_y = scroll_y + 0x120;
	// Skip 240-255 dead zone (NES rooms are 240px)
	if ((pseudo_scroll_y & 0xff) >= 240) pseudo_scroll_y += 16;

	temp1 = pseudo_scroll_y >> 8; // room number
	if (temp1 >= scroll_limits[level]) temp1 = scroll_limits[level];

	// Decompress room if needed
	if (temp1 & 1) {
		if (temp1 != cmap2_room_id) {
			decompress_room(c_map2, level_main_data[level][temp1]);
			cmap2_room_id = temp1;
		}
		set_data_pointer(c_map2);
	} else {
		if (temp1 != cmap_room_id) {
			decompress_room(c_map, level_main_data[level][temp1]);
			cmap_room_id = temp1;
		}
		set_data_pointer(c_map);
	}
	// For vertical scrolling with horizontal mirroring: nt 0 and 2
	nt = (temp1 & 1) << 1;
	y = pseudo_scroll_y & 0xff;

	// Draw 2 columns of a row per frame, cycle through 4 frames for full row
	switch(scroll_count) {
		case 0:
			address = get_ppu_addr(nt, 0x00, y);
			index = (y & 0xf0) + 0;
			buffer_4_mt(address, index);

			address = get_ppu_addr(nt, 0x20, y);
			index = (y & 0xf0) + 2;
			buffer_4_mt(address, index);
			break;

		case 1:
			address = get_ppu_addr(nt, 0x40, y);
			index = (y & 0xf0) + 4;
			buffer_4_mt(address, index);

			address = get_ppu_addr(nt, 0x60, y);
			index = (y & 0xf0) + 6;
			buffer_4_mt(address, index);
			break;

		case 2:
			address = get_ppu_addr(nt, 0x80, y);
			index = (y & 0xf0) + 8;
			buffer_4_mt(address, index);

			address = get_ppu_addr(nt, 0xa0, y);
			index = (y & 0xf0) + 10;
			buffer_4_mt(address, index);
			break;

		default:
			address = get_ppu_addr(nt, 0xc0, y);
			index = (y & 0xf0) + 12;
			buffer_4_mt(address, index);

			address = get_ppu_addr(nt, 0xe0, y);
			index = (y & 0xf0) + 14;
			buffer_4_mt(address, index);
	}

	++scroll_count;
	scroll_count &= 3;
}


void draw_screen_U(void) {
	// Scrolling up - draw rows above the visible area
	if (scroll_y < 0x10) return;

	pseudo_scroll_y = scroll_y - 0x10;
	// Skip 240-255 dead zone (NES rooms are 240px)
	if ((pseudo_scroll_y & 0xff) >= 240) pseudo_scroll_y -= 16;

	temp1 = pseudo_scroll_y >> 8; // room number

	if (temp1 & 1) {
		if (temp1 != cmap2_room_id) {
			decompress_room(c_map2, level_main_data[level][temp1]);
			cmap2_room_id = temp1;
		}
		set_data_pointer(c_map2);
	} else {
		if (temp1 != cmap_room_id) {
			decompress_room(c_map, level_main_data[level][temp1]);
			cmap_room_id = temp1;
		}
		set_data_pointer(c_map);
	}
	nt = (temp1 & 1) << 1;
	y = pseudo_scroll_y & 0xff;

	switch(scroll_count) {
		case 0:
			address = get_ppu_addr(nt, 0x00, y);
			index = (y & 0xf0) + 0;
			buffer_4_mt(address, index);

			address = get_ppu_addr(nt, 0x20, y);
			index = (y & 0xf0) + 2;
			buffer_4_mt(address, index);
			break;

		case 1:
			address = get_ppu_addr(nt, 0x40, y);
			index = (y & 0xf0) + 4;
			buffer_4_mt(address, index);

			address = get_ppu_addr(nt, 0x60, y);
			index = (y & 0xf0) + 6;
			buffer_4_mt(address, index);
			break;

		case 2:
			address = get_ppu_addr(nt, 0x80, y);
			index = (y & 0xf0) + 8;
			buffer_4_mt(address, index);

			address = get_ppu_addr(nt, 0xa0, y);
			index = (y & 0xf0) + 10;
			buffer_4_mt(address, index);
			break;

		default:
			address = get_ppu_addr(nt, 0xc0, y);
			index = (y & 0xf0) + 12;
			buffer_4_mt(address, index);

			address = get_ppu_addr(nt, 0xe0, y);
			index = (y & 0xf0) + 14;
			buffer_4_mt(address, index);
	}

	++scroll_count;
	scroll_count &= 3;
}


void new_cmap_D(void) {
	// Load collision map for room below (scrolling down)
	room = (scroll_y >> 8) + 1;
	if (room > scroll_limits[level]) room = scroll_limits[level];

	map = room & 1;
	if (!map) {
		decompress_room(c_map, level_main_data[level][room]);
		cmap_room_id = room;
	} else {
		decompress_room(c_map2, level_main_data[level][room]);
		cmap2_room_id = room;
	}
}


void new_cmap_U(void) {
	// Load collision map for room above (scrolling up)
	room = scroll_y >> 8;
	if (room > 0) --room;

	map = room & 1;
	if (!map) {
		decompress_room(c_map, level_main_data[level][room]);
		cmap_room_id = room;
	} else {
		decompress_room(c_map2, level_main_data[level][room]);
		cmap2_room_id = room;
	}
}


void sprite_collisions(void) {
    // Update damage cooldown
    if (damage_cooldown > 0) {
        --damage_cooldown;
    }
    
    // Check coin collisions
    ENTITY1.x = high_byte(NINJA.x);
    ENTITY1.y = high_byte(NINJA.y);
    ENTITY1.width = HERO_WIDTH;
    ENTITY1.height = HERO_HEIGHT;
    
    for (index = 0; index < MAX_COINS; ++index) {
        if (coin_active[index]) {
            if (coin_type[index] == COIN_REG) {
                ENTITY2.width = COIN_WIDTH;
                ENTITY2.height = COIN_HEIGHT;
            } else {
                ENTITY2.width = BIG_COIN;
                ENTITY2.height = BIG_COIN;
            }
            ENTITY2.x = coin_x[index];
            ENTITY2.y = coin_y[index];
            
            if (check_collision(&ENTITY1, &ENTITY2)) {
                coin_y[index] = TURN_OFF;
                coin_active[index] = 0;
				if (coin_type[index] == COIN_END) {
					// Exit marker: advance level, no coin reward.
					++level_up;
					sfx_play(SFX_DING, 0);
				} else {
					++coins;
					sfx_play(SFX_DING, 0);
				}
            }
        }
    }
    
    // Check enemy collisions (player vs enemy/boss)
    ENTITY1.x = high_byte(NINJA.x);
    ENTITY1.y = high_byte(NINJA.y);
    ENTITY1.width = HERO_WIDTH;
    ENTITY1.height = HERO_HEIGHT;

    for (index = 0; index < MAX_ENEMY; ++index) {
        if (!enemy_active[index]) continue;

		if (enemy_type[index] == ENEMY_THORNS && enemy_thorn_out[index] == 0) continue; // safe while retracted

        ENTITY2.x = enemy_x[index];
        ENTITY2.y = enemy_y[index];

        // Set collision box for boss or regular enemy
        if (enemy_type[index] == ENEMY_BOSS1) {
            ENTITY2.width = 28;  // 2x2 boss
            ENTITY2.height = 28;
        } else {
            ENTITY2.width = ENEMY_WIDTH;
            ENTITY2.height = ENEMY_HEIGHT;
        }

        if (check_collision(&ENTITY1, &ENTITY2)) {
            // Only take damage if not in cooldown period
            if (damage_cooldown == 0) {
                unsigned char damage = corn_mode ? (BOSS_DAMAGE_PER_HIT * CORN_DAMAGE_MULTIPLIER) : BOSS_DAMAGE_PER_HIT;
                if (boss_health < damage) boss_health = 0;
                else boss_health -= damage;
                player_health -= 2;
                damage_cooldown = DAMAGE_COOLDOWN_TIME;
                sfx_play(SFX_NOISE, 0);
                if (player_health <= 0) {
                    death = 1;
                }
            }
            // Do NOT remove or damage the enemy/boss!
            break; // Only one collision per frame
        }
    }
    
    // Check enemy collisions
    ENTITY2.x = high_byte(NINJA.x);
    ENTITY2.y = high_byte(NINJA.y);
    ENTITY2.width = HERO_WIDTH;
    ENTITY2.height = HERO_HEIGHT;
    
    for (index = 0; index < MAX_ENEMY; ++index) {
        if (enemy_active[index]) {
            ENTITY1.x = enemy_x[index];
            ENTITY1.y = enemy_y[index];
            
            // Use different collision box for boss
            if (enemy_type[index] == ENEMY_BOSS1) {
                ENTITY2.width = 28;  // 32 pixels - 4 pixels for safety
                ENTITY2.height = 28; // 32 pixels - 4 pixels for safety
                
                if (check_collision(&ENTITY1, &ENTITY2)) {
                    // Hit boss
                    unsigned char damage = corn_mode ? (BOSS_DAMAGE_PER_HIT * CORN_DAMAGE_MULTIPLIER) : BOSS_DAMAGE_PER_HIT;
                    if (boss_health < damage) boss_health = 0;
                    else boss_health -= damage;
                    turd_active[index] = 0;
                    sfx_play(SFX_DING, 0); // Play hit sound
                    
                    // Check if boss is defeated
                    if (boss_health <= 0) {
                        enemy_y[index] = TURN_OFF;
                        enemy_active[index] = 0;
                        ++level_up; // Advance to next level
                    }
                    break;
                }
            }
        }
    }
}



// cc65 is very slow if 2 arrays/pointers are on the same line, so I
// broke them into 2 separate lines with temp1 as a passing variable
void sprite_obj_init(void) {
	unsigned char enemy_stride;
	unsigned char param;
	unsigned int delay_frames;

	pointer = Coins_list[level];
	for(index = 0,index2 = 0;index < MAX_COINS; ++index) {

		coin_x[index] = 0;

		temp1 = pointer[index2]; // byte 0: y_val
		coin_y[index] = temp1;

		if (temp1 == TURN_OFF) break;

		++index2;

		coin_active[index] = 0;

		temp1 = pointer[index2]; // byte 1: room
		coin_room[index] = temp1;

		++index2;

		temp1 = pointer[index2]; // byte 2: x_val
		coin_actual_x[index] = temp1;

		++index2;

		temp1 = pointer[index2]; // byte 3: type
		coin_type[index] = temp1;

		++index2;

		// For vertical levels: y_val = Y-in-room, x_val = screen X
		// For horizontal: y_val = screen Y, x_val = X-in-room
		if (is_vertical) {
			coin_actual_y[index] = coin_y[index]; // store Y-in-room
		} else {
			coin_actual_y[index] = coin_y[index]; // store screen Y (for consistency)
		}
	}

	for(++index;index < MAX_COINS; ++index) {
		coin_y[index] = TURN_OFF;
	}
	
	

	pointer = Enemy_list[level];
	// Detect stride (4 legacy bytes or 5 with param) by scanning to sentinel
	for(index2 = 0; pointer[index2] != TURN_OFF; ++index2) { }
	temp1 = index2; // bytes before sentinel
	enemy_stride = (temp1 % 5 == 0) ? 5 : 4;

	for(index = 0,index2 = 0;index < MAX_ENEMY; ++index) {
		
		enemy_x[index] = 0;

		temp1 = pointer[index2]; // get a byte of data
		enemy_y[index] = temp1;
		
		if (temp1 == TURN_OFF) break;

		++index2;
		
		enemy_active[index] = 0;
		
		temp1 = pointer[index2]; // get a byte of data
		enemy_room[index] = temp1;
		
		++index2;
		
		temp1 = pointer[index2]; // byte 2: x_val
		enemy_actual_x[index] = temp1;

		++index2;

		temp1 = pointer[index2]; // byte 3: type
		enemy_type[index] = temp1;
		++index2;

		// Store actual Y for vertical mode
		if (is_vertical) {
			enemy_actual_y[index] = enemy_y[index];
		} else {
			enemy_actual_y[index] = enemy_y[index];
		}

		param = 0;
		if (enemy_stride == 5) {
			param = pointer[index2];
			++index2;
		} else {
			param = 4; // default delay seconds for thorns in legacy data
		}

		// Per-type initialization - zero all shared state
		enemy_vel_x[index] = 0;
		enemy_vel_y[index] = 0;
		enemy_origin_y[index] = 0;
		enemy_state[index] = 0;
		enemy_thorn_delay[index] = 0;
		enemy_thorn_timer[index] = 0;
		enemy_thorn_out[index] = 0;
		enemy_dir[index] = 0;

		// Default anim lookup (ENEMY_WASP=0x09 through ENEMY_VIRUS=0x13)
		temp1 = enemy_type[index] - ENEMY_WASP;
		if (temp1 < 11) {
			enemy_anim[index] = enemy_init_anim[temp1];
		}
		// Boss2 uses boss1 sprite/AI as placeholder
		if (enemy_type[index] == ENEMY_BOSS2) {
			enemy_type[index] = ENEMY_BOSS1;
			enemy_anim[index] = Boss1SprL;
		}
		// Type-specific extra init
		if (enemy_type[index] == ENEMY_THORNS) {
			delay_frames = (unsigned int)param * 60;
			if (delay_frames == 0) delay_frames = 4 * 60;
			enemy_thorn_delay[index] = delay_frames;
			enemy_thorn_timer[index] = delay_frames;
		} else if (enemy_type[index] == ENEMY_RINGWORM) {
			enemy_vel_x[index] = 1;
			enemy_vel_y[index] = 1;
		} else if (enemy_type[index] == ENEMY_SPIDER) {
			enemy_origin_y[index] = enemy_y[index];
		}
	}
	
	for(++index;index < MAX_ENEMY; ++index) {
		enemy_y[index] = TURN_OFF;
		enemy_active[index] = 0;
		enemy_thorn_out[index] = 0;
		enemy_thorn_timer[index] = 0;
		enemy_thorn_delay[index] = 0;
	}
}


void fire_turd(void) {
    // Find an inactive turd slot
    for(index = 0; index < MAX_TURDS; ++index) {
        if (!turd_active[index]) {
            turd_active[index] = 1;
            
            // Set initial position based on player position and direction
            // Adjust to fire from top half and front of ninja
            if (direction == LEFT) {
                turd_x[index] = high_byte(NINJA.x) - 4; // Slightly in front when facing left
            } else {
                turd_x[index] = high_byte(NINJA.x) + 12; // Slightly in front when facing right
            }
            turd_y[index] = high_byte(NINJA.y) - 2; // From upper body
            
            // Handle diagonal throws (up + left/right)
            if (pad1 & PAD_UP) {
                if (pad1 & PAD_RIGHT) {
                    // Diagonal up-right
                    turd_direction[index] = TURD_UP;
                    turd_vel_x[index] = 3; // Right movement
                    turd_vel_y[index] = -7; // Upward velocity
                } 
                else if (pad1 & PAD_LEFT) {
                    // Diagonal up-left
                    turd_direction[index] = TURD_UP;
                    turd_vel_x[index] = -3; // Left movement
                    turd_vel_y[index] = -7; // Upward velocity
                }
                else {
                    // Just up
                    turd_direction[index] = TURD_UP;
                    turd_vel_x[index] = (direction == LEFT) ? -1 : 1; // Slight horizontal movement
                    turd_vel_y[index] = -7; // Faster upward velocity
                }
            }
            else if (pad1 & PAD_DOWN) {
                if (pad1 & PAD_RIGHT) {
                    // Diagonal down-right
                    turd_direction[index] = TURD_DOWN;
                    turd_vel_x[index] = 3; // Right movement
                    turd_vel_y[index] = 3; // Downward velocity
                }
                else if (pad1 & PAD_LEFT) {
                    // Diagonal down-left
                    turd_direction[index] = TURD_DOWN;
                    turd_vel_x[index] = -3; // Left movement
                    turd_vel_y[index] = 3; // Downward velocity
                }
                else {
                    // Just down
                    turd_direction[index] = TURD_DOWN;
                    turd_vel_x[index] = (direction == LEFT) ? -1 : 1; // Slight horizontal movement
                    turd_vel_y[index] = 3; // Downward velocity
                }
            }
            else if (direction == LEFT) {
                turd_direction[index] = TURD_LEFT;
                turd_vel_x[index] = -3; // Horizontal speed
                turd_vel_y[index] = -3; // Slight initial upward arc
            }
            else { // RIGHT
                turd_direction[index] = TURD_RIGHT;
                turd_vel_x[index] = 3; // Horizontal speed
                turd_vel_y[index] = -3; // Slight initial upward arc
            }
            
            sfx_play(SFX_NOISE, 0); // Play sound effect
            break;
        }
    }
}

// New function to update turd positions
void update_turds(void) {
    for(index = 0; index < MAX_TURDS; ++index) {
        if (turd_active[index]) {
            // Move turd
            turd_x[index] += turd_vel_x[index];
            turd_y[index] += turd_vel_y[index];
            
            // Apply gravity to all turds (reduced gravity amount)
            turd_vel_y[index] += 1; // Integer gravity
          
            // Cap falling speed
            if (turd_vel_y[index] > 5) {
                turd_vel_y[index] = 5;
            }
            
            // Check if turd is off screen
            if (turd_x[index] > 250 || turd_y[index] > 240 || turd_x[index] < 5 || turd_y[index] < 5) {
                turd_active[index] = 0;
                continue;
            }
            
            // Check collision with background
            ENTITY1.x = turd_x[index];
            ENTITY1.y = turd_y[index];
            ENTITY1.width = TURD_WIDTH;
            ENTITY1.height = TURD_HEIGHT;
            
            // Check for collisions using bg_coll_D, just like the ninja
            if (bg_coll_D()) {
                turd_active[index] = 0;
                continue;
            }
            
            // Check collision with enemies
            for (index2 = 0; index2 < MAX_ENEMY; ++index2) {
                if (enemy_active[index2]) {
                    ENTITY2.x = enemy_x[index2];
                    ENTITY2.y = enemy_y[index2];
                    
                    // Use different collision box for boss
                    if (enemy_type[index2] == ENEMY_BOSS1) {
                        ENTITY2.width = 28;  // 32 pixels - 4 pixels for safety
                        ENTITY2.height = 28; // 32 pixels - 4 pixels for safety
                        
                        if (check_collision(&ENTITY1, &ENTITY2)) {
                            // Hit boss
                            unsigned char damage = corn_mode ? (BOSS_DAMAGE_PER_HIT * CORN_DAMAGE_MULTIPLIER) : BOSS_DAMAGE_PER_HIT;
                            if (boss_health < damage) boss_health = 0;
                            else boss_health -= damage;
                            turd_active[index] = 0;
                            sfx_play(SFX_DING, 0); // Play hit sound
                            
                            // Check if boss is defeated
                            if (boss_health <= 0) {
                                enemy_y[index2] = TURN_OFF;
                                enemy_active[index2] = 0;
                                ++level_up; // Advance to next level
                            }
                            break;
                        }
                    } else {
                        ENTITY2.width = ENEMY_WIDTH;
                        ENTITY2.height = ENEMY_HEIGHT;
                        
						if (enemy_type[index2] == ENEMY_THORNS) {
							// Thorns are indestructible; skip collision handling for turds
							continue;
						}

						if (check_collision(&ENTITY1, &ENTITY2)) {
							if (enemy_type[index2] == ENEMY_VIRUS && enemy_state[index2] == 0) {
								// Virus becomes aggro when hit, doesn't die
								enemy_state[index2] = 1;
								turd_active[index] = 0;
								sfx_play(SFX_DING, 0);
								break;
							}
                            // Hit enemy - kill it
                            enemy_y[index2] = TURN_OFF;
                            enemy_active[index2] = 0;
                            turd_active[index] = 0;
                            sfx_play(SFX_DING, 0);
                            break;
                        }
                    }
                }
            }
        }
    }
}

// New function to draw turds
void draw_turds(void) {
	for(index = 0; index < MAX_TURDS; ++index) {
		if (turd_active[index]) {
			if (corn_mode) {
				// Use the corn sprite for corn mode
				oam_meta_spr(turd_x[index], turd_y[index], CoinSpr);
			} else {
				// Use the regular turd sprite
				oam_meta_spr(turd_x[index], turd_y[index], TurdSpr);
			}
		}
	}
}

// Function to fire enemy bullets
void fire_enemy_bullet(unsigned char enemy_index, unsigned char bullet_type) {
    // Find two inactive bullet slots for the boss
    char slots_found = 0;
    char first_slot = -1;
    
    for(index = 0; index < MAX_ENEMY_BULLETS; ++index) {
        if (!enemy_bullet_active[index]) {
            if (slots_found == 0) {
                first_slot = index;
            }
            
            enemy_bullet_active[index] = 1;
            enemy_bullet_type[index] = bullet_type;
            
            // Set initial position based on enemy position
            // Fire from the middle-front of the boss
            if (enemy_x[enemy_index] > ENTITY2.x) {
                // Boss is to the right of player, fire left
                enemy_bullet_x[index] = enemy_x[enemy_index] - 4;
                enemy_bullet_vel_x[index] = -3; // Same speed as ninja's turds
            } else {
                // Boss is to the left of player, fire right
                enemy_bullet_x[index] = enemy_x[enemy_index] + 28;
                enemy_bullet_vel_x[index] = 3; // Same speed as ninja's turds
            }
            enemy_bullet_y[index] = enemy_y[enemy_index] + 16; // From middle of boss
            
            // Different arcs for each bullet
            if (slots_found == 0) {
                // First bullet - higher arc
                enemy_bullet_vel_y[index] = -8; // Higher arc than ninja's turds
            } else {
                // Second bullet - lower arc
                enemy_bullet_vel_y[index] = -6; // Similar to ninja's turds
            }
            
            ++slots_found;
            
            // Break if we've fired both bullets
            if (slots_found >= 2) {
                // Set cooldown for this enemy
                enemy_bullet_cooldown[enemy_index] = ENEMY_BULLET_COOLDOWN;
                
                // Play sound effect only once
                sfx_play(SFX_NOISE, 0);
                break;
            }
        }
    }
    
    // If we only found one slot, still set the cooldown
    if (slots_found == 1) {
        enemy_bullet_cooldown[enemy_index] = ENEMY_BULLET_COOLDOWN;
        sfx_play(SFX_NOISE, 0);
    }
}

// Function to update enemy bullets
void update_enemy_bullets(void) {
    char i;
    
    for (i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if (enemy_bullet_active[i]) {
            // Update position
            enemy_bullet_x[i] += enemy_bullet_vel_x[i];
            enemy_bullet_y[i] += enemy_bullet_vel_y[i];
            
            // Apply gravity (same as ninja's turds)
            enemy_bullet_vel_y[i] += 1;
            
            // Cap falling speed
            if (enemy_bullet_vel_y[i] > 5) {
                enemy_bullet_vel_y[i] = 5;
            }
            
            // Check for collisions
            ENTITY1.x = enemy_bullet_x[i];
            ENTITY1.y = enemy_bullet_y[i];
            ENTITY1.width = ENEMY_BULLET_WIDTH;
            ENTITY1.height = ENEMY_BULLET_HEIGHT;
            
            // Check for collisions using bg_coll_D, just like the ninja
            if (bg_coll_D()) {
                enemy_bullet_active[i] = 0;
                continue;
            }
            
            // Check if bullet is off screen
            if (enemy_bullet_y[i] >= 0xf0 || enemy_bullet_x[i] < 5 || enemy_bullet_x[i] > 250) {
                enemy_bullet_active[i] = 0;
                continue;
            }
            
            // Check collision with player
            ENTITY2.x = high_byte(NINJA.x);
            ENTITY2.y = high_byte(NINJA.y);
            ENTITY2.width = HERO_WIDTH;
            ENTITY2.height = HERO_HEIGHT;
            
            if (check_collision(&ENTITY1, &ENTITY2)) {
                // Only take damage if not in cooldown period
                if (damage_cooldown == 0) {
                    player_health -= 2;
                    damage_cooldown = DAMAGE_COOLDOWN_TIME;
                    
                    // Play damage sound
                    sfx_play(SFX_NOISE, 0);
                    
                    // Only set death flag if health is depleted
                    if (player_health <= 0) {
                        death = 1;
                    }
                }
                enemy_bullet_active[i] = 0;
            }
        }
    }
}

// Function to draw enemy bullets
void draw_enemy_bullets(void) {
    for(index = 0; index < MAX_ENEMY_BULLETS; ++index) {
        if (enemy_bullet_active[index]) {
            // Use the same sprite as player turds for now
            oam_meta_spr(enemy_bullet_x[index], enemy_bullet_y[index], TurdSpr);
        }
    }
}