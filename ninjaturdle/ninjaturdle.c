/*	
 * Ninja Turdle II - NES
 */	
 
#include "LIB/neslib.h"
#include "LIB/nesdoug.h"
#include "LIB/kenes.h"
#include "sprites.h"
#include "ninjaturdle.h"
#include "level_data.c"

#define LEFT 0
#define RIGHT 1

unsigned char bounce[] = {
    0, 0, 0, 1, 2, 2, 2, 1
};
	
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
	//level = 2; // debugging, start on level...
	
	has_turd_power = 1; // Default to having turd power
	turd_cooldown = 0;
	player_health = MAX_HEALTH; // Initialize player health
	damage_cooldown = 0;
	boss_health = BOSS_MAX_HEALTH; // Initialize boss health
	corn_mode = 0; // Initialize to turd mode
	
	direction = RIGHT; // Initialize direction to face right
	
	// Initialize turd projectiles
	for (index = 0; index < MAX_TURDS; ++index) {
		turd_active[index] = 0;
	}
	
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
				load_room();
				game_mode = MODE_GAME;
				pal_bg(palette_bg);
				song = SONG_GAME;
				music_play(song);
				scroll_x = 0;
				set_scroll_x(scroll_x);
				ppu_on_all();		
				pal_bright(4); // back to normal brightness	
			}
		}
		
		

		while (game_mode == MODE_GAME) {
			ppu_wait_nmi();
			
			set_music_speed(8);
		
			pad1 = pad_poll(0); // read the first controller
			pad1_new = get_pad_new(0);
			
			movement();
			
			check_spr_objects(); // see which objects are on screen
			
			sprite_collisions();
			
			// Update enemy bullets
			update_enemy_bullets();
			
			// set scroll
			set_scroll_x(scroll_x);
			set_scroll_y(scroll_y);
			
			draw_screen_R();
			
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
				delay(5);
				
				oam_clear();
				game_mode = MODE_GAME_OVER;
				vram_adr(NAMETABLE_A);
				vram_fill(0, 1024); // blank the screen
				ppu_on_all();
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
				if (level < 3) {
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
			
			multi_vram_buffer_horz(END_TEXT, sizeof(END_TEXT), NTADR_A(6,13));
			multi_vram_buffer_horz(END_TEXT2, sizeof(END_TEXT2), NTADR_A(8,15));
			multi_vram_buffer_horz(END_TEXT3, sizeof(END_TEXT3), NTADR_A(11,17));
			temp1 = (coins / 10) + 0x30;
			temp2 = (coins % 10) + 0x30;
			one_vram_buffer(temp1, NTADR_A(18,17));
			one_vram_buffer(temp2, NTADR_A(19,17));
			
			set_scroll_x(0);
			
			music_stop();
            game_mode = MODE_RESET;
		}
		
		while (game_mode == MODE_GAME_OVER) { // you died, death
			ppu_wait_nmi();
			oam_clear();
			
			multi_vram_buffer_horz(DEAD_TEXT, sizeof(DEAD_TEXT), NTADR_A(12,14));
			
			set_scroll_x(0);
			
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
	
	// Set MMC1 banks for gameplay - different banks for BG and sprites
	mmc1_write(MMC1_CONTROL, 0x12);  // 4KB CHR mode
	mmc1_write(MMC1_CHR0, CHR_BANK_MAP);     // Pattern Table 0 (background) uses map tiles
	mmc1_write(MMC1_CHR1, CHR_BANK_SPRITES); // Pattern Table 1 (sprites) uses sprite tiles
	
	// Ensure sprite pattern table is set to 1
	bank_spr(1);
	
	offset = Level_offsets[level];
	
	set_data_pointer(Levels_list[offset]);
	set_mt_pointer(metatiles1);
	for(y=0; ;y+=0x20) { 
		for(x=0; ;x+=0x20) {
			address = get_ppu_addr(0, x, y);
			index = (y & 0xf0) + (x >> 4);
			buffer_4_mt(address, index); // ppu_address, index to the data
			flush_vram_update2();
			if (x == 0xe0) break;
		}
		if (y == 0xe0) break;
	}
	
	
	++offset;
	
	// a little bit in the next room
	set_data_pointer(Levels_list[offset]);
	for(y=0; ;y+=0x20) { 
		x = 0;
		address = get_ppu_addr(1, x, y);
		index = (y & 0xf0);
		buffer_4_mt(address, index); // ppu_address, index to the data
		flush_vram_update2();
		if (y == 0xe0) break;
	}
	
	--offset;
	// copy the room to the collision map
	// the second one should auto-load with the scrolling code
	memcpy (c_map, Levels_list[offset], 240);
	
	
	sprite_obj_init();
	NINJA.x = 0x4000;
	NINJA.y = 0xc400;
	NINJA.vel_x = 0;
	NINJA.vel_y = 0;
	
	map_loaded = 0;
	
	// Reset player health when starting a new level
	player_health = MAX_HEALTH;
	damage_cooldown = 0;
	
	// Reset boss health when starting a new level
	boss_health = BOSS_MAX_HEALTH;
}


void draw_sprites(void) {
	// clear all sprites from sprite buffer
	oam_clear();
	
	temp_x = high_byte(NINJA.x);
	if (temp_x > 0xfc) temp_x = 1;
	if (temp_x == 0) temp_x = 1;
	// draw 1 hero
	if (direction == LEFT) {
		oam_meta_spr(temp_x, high_byte(NINJA.y), NinjaSprL);
	}
	else {
		oam_meta_spr(temp_x, high_byte(NINJA.y), NinjaSprR);
	}
	
	// draw coin sprites
	for (index = 0; index < MAX_COINS; ++index) {
		temp_y = coin_y[index];
		if (temp_y == TURN_OFF) continue;
		if (!coin_active[index]) continue;
		temp_x = coin_x[index];
		if (temp_x > 0xf0) continue;
		
		// bounce the coin
		temp1 = get_frame_count();
		temp1 = (temp1 >> 2) & 7;
		temp1 = bounce[temp1];
		temp_y += temp1;
		
		if (temp_y < 0xf0) {
			if (coin_type[index] == COIN_REG) {
				oam_meta_spr(temp_x, temp_y, CoinSpr);
			}
			else {
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
			oam_meta_spr(temp_x, temp_y, enemy_anim[index2]);
		}
	}
	
	// Draw turds
	draw_turds();
	
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

	
	// handle y

	// gravity

	// NINJA.vel_y is signed
	if (NINJA.vel_y < 0x300) {
		NINJA.vel_y += GRAVITY;
	}
	else {
		NINJA.vel_y = 0x300; // consistent
	}
	NINJA.y += NINJA.vel_y;
	
	ENTITY1.x = high_byte(NINJA.x);
	ENTITY1.y = high_byte(NINJA.y);
	
    if (NINJA.vel_y > 0) {
        if (bg_coll_D() ) { // check collision below
            high_byte(NINJA.y) = high_byte(NINJA.y) - eject_D;
            NINJA.y &= 0xff00;
            if (NINJA.vel_y > 0) {
                NINJA.vel_y = 0;
            }
        }
    }
    else if (NINJA.vel_y < 0) {
        if (bg_coll_U() ) { // check collision above
            high_byte(NINJA.y) = high_byte(NINJA.y) - eject_U;
            NINJA.vel_y = 0;
        }
    }
    
	// check collision down a little lower than hero
	ENTITY1.y = high_byte(NINJA.y); // the rest should be the same
	
	if (pad1_new & PAD_A) {
        if (bg_coll_D2() ) {
			NINJA.vel_y = JUMP_VEL; // JUMP
			sfx_play(SFX_JUMP, 0);
			short_jump_count = 1;
		}
		
	}
	
	// allow shorter jumps
	if (short_jump_count) {
		++short_jump_count;
		if (short_jump_count > 30) short_jump_count = 0;
	}
	if ((short_jump_count) && ((pad1 & PAD_A) == 0) && (NINJA.vel_y < -0x200)) {
		NINJA.vel_y = -0x200;
		short_jump_count = 0;
	}
	
	// do we need to load a new collision map? (scrolled into a new room)
	if ((scroll_x & 0xff) < 4) {
		if (!map_loaded) {
			new_cmap();
			map_loaded = 1; // only do once
		}
		
	}
	else {
		map_loaded = 0;
	}
	
	// scroll
	temp5 = NINJA.x;
	if (NINJA.x > MAX_RIGHT) {
		temp1 = (NINJA.x - MAX_RIGHT) >> 8;
        if (temp1 > 3) temp1 = 3; // max scroll change
		scroll_x += temp1;
		high_byte(NINJA.x) = high_byte(NINJA.x) - temp1;
	}

	if (scroll_x >= MAX_SCROLL) {
		scroll_x = MAX_SCROLL; // stop scrolling right, end of level
		NINJA.x = temp5; // but allow the x position to go all the way right
		if (high_byte(NINJA.x) >= 0xf1) {
			NINJA.x = 0xf100;
		}
	}

	// Handle turd firing
	if (turd_cooldown > 0) {
		--turd_cooldown;
	}
	
	// Toggle corn mode with Select button
	if (pad1_new & PAD_SELECT) {
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
	// mark each object "active" if they are, and get the screen x
	
	for (index = 0; index < MAX_COINS; ++index) {
		coin_active[index] = 0; //default to zero
		if (coin_y[index] != TURN_OFF) {
			high_byte(temp5) = coin_room[index];
			low_byte(temp5) = coin_actual_x[index];
			coin_active[index] = get_position();
			coin_x[index] = temp_x; // screen x coords
		}

	}
	

	for (index = 0; index < MAX_ENEMY; ++index) {
		enemy_active[index] = 0; //default to zero
		if (enemy_y[index] != TURN_OFF) {
			high_byte(temp5) = enemy_room[index];
			low_byte(temp5) = enemy_actual_x[index];
			temp1 = enemy_active[index] = get_position();
			if (temp1 == 0) continue;
			enemy_x[index] = temp_x; // screen x coords
			
			enemy_moves(); // if active, do it's moves now
		}
	}
}


char get_position(void) {
	// is it in range ? return 1 if yes
	temp5 -= scroll_x;
	temp_x = temp5 & 0xff;
	if (high_byte(temp5)) return 0;
	return 1;
}


void enemy_moves(void) {
	if (enemy_type[index] == ENEMY_BOSS1) {
		// Set collision box to center-bottom of the boss
		ENTITY1.x = enemy_x[index];
		ENTITY1.y = enemy_y[index] + 28; // Bottom of the boss (32px height - 4px for safety)
		ENTITY1.width = 28; 
		ENTITY1.height = 4; // Just check the bottom 4 pixels for ground collision
        // note, ENTITY2 is the hero's x position
		
		if (enemy_frames & 1) return; // half speed
		
		// Jump behavior
		temp1 = enemy_frames + (index << 3);
		temp1 &= 0x3f;
		if (temp1 < 8) { // stand still
			// Face the ninja
			if (enemy_x[index] > ENTITY2.x) {
				enemy_anim[index] = Boss1SprL; // Use left-facing sprite
			} else {
				enemy_anim[index] = Boss1SprR; // Use right-facing sprite
			}
            
            // Shoot linear bullet during standing phase
            if (enemy_bullet_cooldown[index] == 0) {
                fire_enemy_bullet(index, BULLET_LINEAR);
            }
		}
		else if (temp1 < 14) {
			--enemy_y[index]; // jump
			--enemy_y[index]; // jump faster
			// Face the ninja
			if (enemy_x[index] > ENTITY2.x) {
				enemy_anim[index] = Boss1SprL;
			} else {
				enemy_anim[index] = Boss1SprR;
			}
		}
		else if (temp1 < 24) {
			--enemy_y[index]; // jump
			--enemy_y[index]; // jump even faster
			// Face the ninja
			if (enemy_x[index] > ENTITY2.x) {
				enemy_anim[index] = Boss1SprL;
			} else {
				enemy_anim[index] = Boss1SprR;
			}
            
            // Shoot thrown bullet at peak of jump
            if (temp1 == 20 && enemy_bullet_cooldown[index] == 0) {
                fire_enemy_bullet(index, BULLET_THROW);
            }
		}
		else if (temp1 < 26) { // use short anim. 2 frames
			--enemy_y[index]; // jump
			// Face the ninja
			if (enemy_x[index] > ENTITY2.x) {
				enemy_anim[index] = Boss1SprL;
			} else {
				enemy_anim[index] = Boss1SprR;
			}
		}
		else {
			++enemy_y[index]; // fall
			if (temp1 < 62) {
				++enemy_y[index]; // fall faster
			}
			// Face the ninja
			if (enemy_x[index] > ENTITY2.x) {
				enemy_anim[index] = Boss1SprL;
			} else {
				enemy_anim[index] = Boss1SprR;
			}
			
			//check ground collision
			ENTITY1.x = enemy_x[index];
			ENTITY1.y = enemy_y[index] + 28; // Bottom of the boss
			
			if (bg_coll_D()) {
				enemy_y[index] -= eject_D;
			}
		}
		
		// Update bullet cooldown
		if (enemy_bullet_cooldown[index] > 0) {
		    --enemy_bullet_cooldown[index];
		}
		
		// Movement towards player
		if (enemy_x[index] > ENTITY2.x) {
			ENTITY1.x -= 1; // test going left
            bg_collision_fast();
			if (collision_L) return;
            // else, no collision, do the move.
			if (enemy_actual_x[index] == 0) --enemy_room[index];
			--enemy_actual_x[index];
		}
		else if (enemy_x[index] < ENTITY2.x) {
			ENTITY1.x += 1; // test going right
            bg_collision_fast();
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
			ENTITY1.x -= 1; // test going left
            bg_collision_fast();
			if (collision_L) return;
            // else, no collision, do the move.
			if (enemy_actual_x[index] == 0) --enemy_room[index];
			--enemy_actual_x[index];
			enemy_anim[index] = EnemyWaspSprL; // Use left-facing sprite
		}
		else if (enemy_x[index] < ENTITY2.x) {
			ENTITY1.x += 1; // test going right
            bg_collision_fast();
			if (collision_R) return;
			++enemy_actual_x[index];
			if (enemy_actual_x[index] == 0) ++enemy_room[index];
			enemy_anim[index] = EnemyWaspSprR; // Use right-facing sprite
		}
	}
	else if (enemy_type[index] == ENEMY_BOUNCE) {
		temp1 = enemy_frames + (index << 3);
        temp1 &= 0x3f;
		if (temp1 < 16) { // stand still
			enemy_anim[index] = EnemyBounceSpr;
		}
        else if (temp1 < 22) {
			--enemy_y[index]; // jump
            --enemy_y[index]; // jump faster
			enemy_anim[index] = EnemyBounceSpr2;
		}
		else if (temp1 < 42) {
			--enemy_y[index]; // jump
			enemy_anim[index] = EnemyBounceSpr2;
		}
        else if (temp1 < 44) { // use short anim. 2 frames
            --enemy_y[index]; // jump
            enemy_anim[index] = EnemyBounceSpr;
        }
		else {
            ++enemy_y[index]; // fall
            if (temp1 < 62) {
                ++enemy_y[index]; // fall faster
            }
			enemy_anim[index] = EnemyBounceSpr2;
			temp1 = enemy_y[index];
			//check ground collision
			ENTITY1.x = enemy_x[index];
			ENTITY1.y = enemy_y[index];
			ENTITY1.width = 15;
			ENTITY1.height = 14;
			
			if (bg_coll_D()) {
				enemy_y[index] -= eject_D;
			}
		}
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
	
	temp_y = ENTITY1.y + 6; // y middle
	
	bg_collision_sub();
	
	if (bg_collision_sub() & COL_ALL) {
		++collision_L;
	}
	
	// right side
	temp5 += ENTITY1.width;
	temp_x = temp5 & 0xff; // low byte x
	temp_room = temp5 >> 8; // high byte x
	
	// temp_y is unchanged
	bg_collision_sub();
	
	if (bg_collision_sub() & COL_ALL) { // find a corner in the collision map
		++collision_R;
	}
}



char bg_coll_L(void) {
    // check 2 points on the left side
    temp5 = ENTITY1.x + scroll_x;
    temp_x = (char)temp5; // low byte
    temp_room = temp5 >> 8; // high byte
    
    eject_L = temp_x | 0xf0;
    temp_y = ENTITY1.y + 2;
    if (bg_collision_sub() & COL_ALL) return 1;
    
    temp_y = ENTITY1.y + ENTITY1.height;
    temp_y -= 2;
    if (bg_collision_sub() & COL_ALL) return 1;
    
    return 0;
}

char bg_coll_R(void) {
    // check 2 points on the right side
    temp5 = ENTITY1.x + scroll_x + ENTITY1.width;
    temp_x = (char)temp5; // low byte
    temp_room = temp5 >> 8; // high byte
    
    eject_R = (temp_x + 1) & 0x0f;
    temp_y = ENTITY1.y + 2;
    if (bg_collision_sub() & COL_ALL) return 1;
    
    temp_y = ENTITY1.y + ENTITY1.height;
    temp_y -= 2;
    if (bg_collision_sub() & COL_ALL) return 1;
    
    return 0;
}

char bg_coll_U(void) {
    // check 2 points on the top side
    temp5 = ENTITY1.x + scroll_x;
    temp5 += 2;
    temp_x = (char)temp5; // low byte
    temp_room = temp5 >> 8; // high byte
    
    temp_y = ENTITY1.y;
    eject_U = temp_y | 0xf0;
    if (bg_collision_sub() & COL_ALL) return 1;
    
    temp5 = ENTITY1.x + scroll_x + ENTITY1.width;
    temp5 -= 2;
    temp_x = (char)temp5; // low byte
    temp_room = temp5 >> 8; // high byte
    
    if (bg_collision_sub() & COL_ALL) return 1;
    
    return 0;
}

char bg_coll_D(void) {
    // check 2 points on the bottom side
    temp5 = ENTITY1.x + scroll_x;
    temp5 += 2;
    temp_x = (char)temp5; // low byte
    temp_room = temp5 >> 8; // high byte
    
    temp_y = ENTITY1.y + ENTITY1.height;
    
    if ((temp_y & 0x0f) > 3) return 0; // bug fix
    // so we don't snap to those platforms
    // don't fall too fast, or might miss it.
    
    eject_D = (temp_y + 1) & 0x0f;
    
    if (bg_collision_sub() ) return 1;
    
    temp5 = ENTITY1.x + scroll_x + ENTITY1.width;
    temp5 -= 2;
    temp_x = (char)temp5; // low byte
    temp_room = temp5 >> 8; // high byte
    
    if (bg_collision_sub() ) return 1;
    
    return 0;
}

char bg_coll_D2(void) {
    // check 2 points on the bottom side
    // a little lower, for jumping
    temp5 = ENTITY1.x + scroll_x;
    temp5 += 2;
    temp_x = (char)temp5; // low byte
    temp_room = temp5 >> 8; // high byte
    
    temp_y = ENTITY1.y + ENTITY1.height;
    temp_y += 2;
    if (bg_collision_sub() ) return 1;
    
    temp5 = ENTITY1.x + scroll_x + ENTITY1.width;
    temp5 -= 2;
    temp_x = (char)temp5; // low byte
    temp_room = temp5 >> 8; // high byte
    
    if (bg_collision_sub() ) return 1;
    
    return 0;
}




char bg_collision_sub(void) {
    if (temp_y >= 0xf0) return 0;
    
	coordinates = (temp_x >> 4) + (temp_y & 0xf0);
    // we just need 4 bits each from x and y
	
	map = temp_room&1; // high byte
	if (!map) {
		collision = c_map[coordinates];
	}
	else {
		collision = c_map2[coordinates];
	}
	
    return is_solid[collision];
}



void draw_screen_R(void) {
	// scrolling to the right, draw metatiles as we go
	pseudo_scroll_x = scroll_x + 0x120;
	
	temp1 = pseudo_scroll_x >> 8;
	
	offset = Level_offsets[level];
	offset += temp1;
	
	set_data_pointer(Levels_list[offset]);
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




void new_cmap(void) {
	// copy a new collision map to one of the 2 c_map arrays
	room = ((scroll_x >> 8) +1); //high byte = room, one to the right
	offset = Level_offsets[level];
	offset += room;
	
	map = room & 1; //even or odd?
	if (!map) {
		memcpy (c_map, Levels_list[offset], 240);
	}
	else {
		memcpy (c_map2, Levels_list[offset], 240);
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
                ++coins;
                sfx_play(SFX_DING, 0);
                
                // Add this line to trigger level transition for big coins
                if (coin_type[index] == COIN_END) ++level_up;
            }
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
                ENTITY1.width = 28;  // 32 pixels - 4 pixels for safety
                ENTITY1.height = 28; // 32 pixels - 4 pixels for safety
            } else {
                ENTITY1.width = ENEMY_WIDTH;
                ENTITY1.height = ENEMY_HEIGHT;
            }
            
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
                        break; // Exit the loop if dead
                    }
                }
            }
        }
    }
}



// cc65 is very slow if 2 arrays/pointers are on the same line, so I
// broke them into 2 separate lines with temp1 as a passing variable
void sprite_obj_init(void) {

	pointer = Coins_list[level];
	for(index = 0,index2 = 0;index < MAX_COINS; ++index) {
		
		coin_x[index] = 0;

		temp1 = pointer[index2]; // get a byte of data
		coin_y[index] = temp1;
		
		if (temp1 == TURN_OFF) break;

		++index2;
		
		coin_active[index] = 0;

		
		temp1 = pointer[index2]; // get a byte of data
		coin_room[index] = temp1;
		
		++index2;
		
		temp1 = pointer[index2]; // get a byte of data
		coin_actual_x[index] = temp1;
		
		++index2;
		
		temp1 = pointer[index2]; // get a byte of data
		coin_type[index] = temp1;
		
		++index2;
	}
	
	for(++index;index < MAX_COINS; ++index) {
		coin_y[index] = TURN_OFF;
	}
	
	

	pointer = Enemy_list[level];
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
		
		temp1 = pointer[index2]; // get a byte of data
		enemy_actual_x[index] = temp1;
		
		++index2;
		
		temp1 = pointer[index2]; // get a byte of data
		enemy_type[index] = temp1;
		
		++index2;
	}
	
	for(++index;index < MAX_ENEMY; ++index) {
		enemy_y[index] = TURN_OFF;
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
            
            // Only check horizontal collisions and ground collisions
            // Allow passing through platforms from below
            if (bg_coll_L() || bg_coll_R() || (turd_vel_y[index] > 0 && bg_coll_D())) {
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
                            if (corn_mode) {
                                // Increased damage in corn mode
                                boss_health -= BOSS_DAMAGE_PER_HIT * CORN_DAMAGE_MULTIPLIER;
                            } else {
                                boss_health -= BOSS_DAMAGE_PER_HIT;
                            }
                            
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
                        
                        if (check_collision(&ENTITY1, &ENTITY2)) {
                            // Hit enemy
                            enemy_y[index2] = TURN_OFF;
                            enemy_active[index2] = 0;
                            turd_active[index] = 0;
                            sfx_play(SFX_DING, 0); // Play hit sound
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
    // Find an inactive bullet slot
    for(index = 0; index < MAX_ENEMY_BULLETS; ++index) {
        if (!enemy_bullet_active[index]) {
            enemy_bullet_active[index] = 1;
            enemy_bullet_type[index] = bullet_type;
            
            // Set initial position based on enemy position
            // Fire from the middle-front of the boss
            if (enemy_x[enemy_index] > ENTITY2.x) {
                // Boss is to the right of player, fire left
                enemy_bullet_x[index] = enemy_x[enemy_index] - 4;
                enemy_bullet_vel_x[index] = -ENEMY_BULLET_SPEED;
            } else {
                // Boss is to the left of player, fire right
                enemy_bullet_x[index] = enemy_x[enemy_index] + 28;
                enemy_bullet_vel_x[index] = ENEMY_BULLET_SPEED;
            }
            enemy_bullet_y[index] = enemy_y[enemy_index] + 16; // From middle of boss
            
            // Always throw with an upward arc like ninja's turds
            enemy_bullet_vel_y[index] = ENEMY_BULLET_JUMP;
            
            // Set cooldown for this enemy
            enemy_bullet_cooldown[enemy_index] = ENEMY_BULLET_COOLDOWN;
            
            // Play sound effect
            sfx_play(SFX_NOISE, 0);
            break;
        }
    }
}

// Function to update enemy bullets
void update_enemy_bullets(void) {
    for(index = 0; index < MAX_ENEMY_BULLETS; ++index) {
        if (enemy_bullet_active[index]) {
            // Move bullet
            enemy_bullet_x[index] += enemy_bullet_vel_x[index];
            enemy_bullet_y[index] += enemy_bullet_vel_y[index];
            
            // Apply gravity to all bullets
            enemy_bullet_vel_y[index] += ENEMY_BULLET_GRAVITY;
            
            // Cap falling speed
            if (enemy_bullet_vel_y[index] > 5) {
                enemy_bullet_vel_y[index] = 5;
            }
            
            // Check if bullet is off screen
            if (enemy_bullet_x[index] > 250 || enemy_bullet_y[index] > 240 || 
                enemy_bullet_x[index] < 5 || enemy_bullet_y[index] < 5) {
                enemy_bullet_active[index] = 0;
                continue;
            }
            
            // Check collision with background
            ENTITY1.x = enemy_bullet_x[index];
            ENTITY1.y = enemy_bullet_y[index];
            ENTITY1.width = ENEMY_BULLET_WIDTH;
            ENTITY1.height = ENEMY_BULLET_HEIGHT;
            
            // Only check horizontal collisions and ground collisions
            // Allow passing through platforms from below
            if (bg_coll_L() || bg_coll_R() || (enemy_bullet_vel_y[index] > 0 && bg_coll_D())) {
                enemy_bullet_active[index] = 0;
                continue;
            }
            
            // Check collision with player
            ENTITY2.x = high_byte(NINJA.x);
            ENTITY2.y = high_byte(NINJA.y);
            ENTITY2.width = HERO_WIDTH;
            ENTITY2.height = HERO_HEIGHT;
            
            if (check_collision(&ENTITY1, &ENTITY2)) {
                if (damage_cooldown == 0) {
                    player_health -= ENEMY_BULLET_DAMAGE;
                    damage_cooldown = DAMAGE_COOLDOWN_TIME;
                    sfx_play(SFX_NOISE, 0);
                    
                    if (player_health <= 0) {
                        death = 1;
                    }
                }
                enemy_bullet_active[index] = 0;
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