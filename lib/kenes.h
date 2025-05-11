// kenes.h - Custom NES utilities for Ninjaturdle
// A collection of helper functions and definitions for NROM games
// Kenny Cason

#ifndef KENES_H
#define KENES_H

#include "neslib.h"
#include "nesdoug.h"

// Type conversion macros for pointer signedness
#define K_PAL(x) ((const char *)(x))
#define K_SPR(x) ((const char *)(x))
#define K_STR(x) ((const char *)(x))
#define K_CHR(x) ((const char *)(x))

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

// MMC1 CHR bank definitions
#define CHR_BANK_FONT      0  // Font tiles (top half of ninjaturtle2.png)
#define CHR_BANK_TITLE     1  // Title screen tiles (bottom half of ninjaturtle2.png)
#define CHR_BANK_MAP       2  // Map tiles (top half of ninjaturtle.png)
#define CHR_BANK_SPRITES   3  // Sprite tiles (bottom half of ninjaturtle.png)

// Bank Definitions
#define BANK_FONT      0
#define BANK_SPRITE1   1
#define BANK_SPRITE2   2
#define BANK_SPRITE3   3

// Choose a blank tile index for the borders
#define TILE_BLANK     0xFF  // Make sure this tile is truly blank in every bank

// MMC1 functions
void mmc1_write(unsigned int address, unsigned char value) {
    char i;
    // Write 5-bit value to MMC1 register, one bit at a time
    for (i = 0; i < 5; ++i) {
        *((unsigned char*)address) = value & 1;
        value = value >> 1;
    }
}

void mmc1_init(void) {
    // Reset MMC1 by writing any value with bit 7 set
    *((unsigned char*)MMC1_CONTROL) = 0x80;
    
    // Configure MMC1:
    // - 4KB CHR bank mode (bit 4 = 1)
    // - 32KB PRG ROM mode (bits 2-3 = 0)
    // - Vertical mirroring (bits 0-1 = 2)
    mmc1_write(MMC1_CONTROL, 0x12);  // 0001 0010
    
    // Set initial CHR banks
    mmc1_write(MMC1_CHR0, 0);  // First 4KB bank
    mmc1_write(MMC1_CHR1, 0);  // Second 4KB bank
}

// Set up both pattern tables for a specific scene
void setup_pattern_tables(unsigned char bg_bank, unsigned char spr_bank) {
    // In 4KB mode, each bank number needs to be multiplied by 2
    // since each 8KB bank is split into two 4KB banks
    mmc1_write(MMC1_CHR0, bg_bank * 2);      // First 4KB
    mmc1_write(MMC1_CHR1, bg_bank * 2 + 1);  // Second 4KB
}

// Fade in a specific palette with a delay
void fade_in_palette(const unsigned char *palette, unsigned char delay_frames) {
    unsigned char i;
    unsigned char temp_pal[32];
    
    // Clear palette first
    pal_clear();
    
    // Fade in one color at a time
    for (i = 0; i < 32; i++) {
        temp_pal[i] = palette[i];
        pal_col(i, temp_pal[i]);
        ppu_wait_nmi(); // Wait for NMI to complete
        if (delay_frames > 0) {
            unsigned char j;
            for (j = 0; j < delay_frames; j++) {
                ppu_wait_nmi();
            }
        }
    }
}

// Fade out the current palette
void fade_out_palette(unsigned char delay_frames) {
    unsigned char i, j;
    unsigned char temp_pal[32];
    
    // Save current palette
    for (i = 0; i < 32; i++) {
        temp_pal[i] = 0x0F; // Default to black
    }
    
    // Fade out one color at a time
    for (i = 0; i < 32; i++) {
        for (j = 0; j < 32; j++) {
            if (temp_pal[j] > 0) {
                temp_pal[j]--;
                pal_col(j, temp_pal[j]);
            }
        }
        ppu_wait_nmi();
        if (delay_frames > 0) {
            unsigned char k;
            for (k = 0; k < delay_frames; k++) {
                ppu_wait_nmi();
            }
        }
    }
}

// Create a simple sprite with position and attributes
unsigned char create_sprite(unsigned char x, unsigned char y, unsigned char tile, 
                          unsigned char palette, unsigned char flip_h, 
                          unsigned char flip_v, unsigned char behind) {
    unsigned char attr = palette;
    if (flip_h) attr |= OAM_FLIP_H;
    if (flip_v) attr |= OAM_FLIP_V;
    if (behind) attr |= OAM_BEHIND;
    
    oam_spr(x, y, tile, attr);
    return oam_get();
}

// Update sprite position
void update_sprite_pos(unsigned char index, unsigned char x, unsigned char y) {
    (void)index; // Suppress unused parameter warning
    oam_spr(x, y, 0, 0); // Use default attributes
}

// Draw a border around the specified area
void draw_border(unsigned char x, unsigned char y, unsigned char width, unsigned char height) {
    unsigned char row, col;
    for(row = 0; row < height; row++) {
        vram_adr(NTADR_A(x, y + row));
        for(col = 0; col < width; col++) {
            vram_put(TILE_BLANK);
        }
    }
}

// stub asm function call
void __fastcall__ buffer_stub();


#endif // KENES_H