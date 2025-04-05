// kenes.h - Custom NES utilities for Ninjaturdle
// A collection of helper functions and definitions for NROM games

#ifndef KENES_H
#define KENES_H

#include "neslib.h"
#include "nesdoug.h"

// Type conversion macros for pointer signedness
#define K_PAL(x) ((const char *)(x))
#define K_SPR(x) ((const char *)(x))
#define K_STR(x) ((const char *)(x))
#define K_CHR(x) ((const char *)(x))

// MMC1 registers (for future reference)
#define MMC1_CONTROL    0x8000
#define MMC1_CHR0       0xA000
#define MMC1_CHR1       0xC000
#define MMC1_PRG        0xE000

// MMC1 control flags
#define MMC1_MIRROR_ONE_SCREEN_LOW    0x00
#define MMC1_MIRROR_ONE_SCREEN_HIGH   0x01
#define MMC1_MIRROR_VERTICAL          0x02
#define MMC1_MIRROR_HORIZONTAL        0x03

// NROM CHR bank definitions
#define CHR_BANK_0 0  // First pattern table (0-4KB)
#define CHR_BANK_1 1  // Second pattern table (4-8KB)

// NROM sprite sheet organization
// Pattern Table 0 (0-4KB): Font, UI elements, and common sprites
// Pattern Table 1 (4-8KB): Background tiles and level-specific sprites

// Function declarations for MMC1 (for future reference)
// These are commented out to avoid compilation errors
/*
void mmc1_write(unsigned int address, unsigned char value);
void mmc1_init(void);
*/

// NROM-specific utilities

// Set up both pattern tables for a specific scene
// bg_bank: 0 or 1 for background pattern table
// spr_bank: 0 or 1 for sprite pattern table
void setup_pattern_tables(unsigned char bg_bank, unsigned char spr_bank) {
    bank_bg(bg_bank);
    bank_spr(spr_bank);
}

// Fade in a specific palette with a delay
// palette: pointer to palette data
// delay_frames: frames to wait between each step
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
                ppu_wait_nmi(); // Wait for NMI to complete
            }
        }
    }
}

// Fade out the current palette
// delay_frames: frames to wait between each step
void fade_out_palette(unsigned char delay_frames) {
    unsigned char i, j;
    unsigned char temp_pal[32];
    
    // Save current palette
    for (i = 0; i < 32; i++) {
        // We can't actually read palette values in NES, so we'll just fade to black
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
        ppu_wait_nmi(); // Wait for NMI to complete
        if (delay_frames > 0) {
            unsigned char k;
            for (k = 0; k < delay_frames; k++) {
                ppu_wait_nmi(); // Wait for NMI to complete
            }
        }
    }
}

// Sprite utilities

// Create a simple sprite with position and attributes
// x, y: position
// tile: tile number
// palette: palette number (0-3)
// flip_h, flip_v: horizontal/vertical flip
// behind: place behind background
// Returns: sprite index
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
// index: sprite index
// x, y: new position
void update_sprite_pos(unsigned char index, unsigned char x, unsigned char y) {
    // Since we can't read sprite attributes, we'll just recreate the sprite
    // This is a simplified version that assumes the sprite is visible
    (void)index; // Suppress unused parameter warning
    oam_spr(x, y, 0, 0); // Use default attributes
}

#endif // KENES_H