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
    // - 4KB CHR bank mode (bit 4 = 0)
    // - 32KB PRG ROM mode (bits 2-3 = 0)
    // - Vertical mirroring (bit 0 = 0)
    mmc1_write(MMC1_CONTROL, 0x0C);  // Changed from 0x0E to 0x0C for 4KB CHR mode
    
    // Set initial CHR banks
    mmc1_write(MMC1_CHR0, CHR_BANK_FONT * 2);      // Pattern table 0: Font (4KB)
    mmc1_write(MMC1_CHR1, CHR_BANK_FONT * 2 + 1);  // Pattern table 0: Font (4KB)
}

// Set up both pattern tables for a specific scene
void setup_pattern_tables(unsigned char bg_bank, unsigned char spr_bank) {
    // In 4KB mode, each bank number needs to be multiplied by 2
    // since each 8KB bank is split into two 4KB banks
    mmc1_write(MMC1_CHR0, bg_bank * 2);      // First 4KB
    mmc1_write(MMC1_CHR1, bg_bank * 2 + 1);  // Second 4KB
}

// NROM-specific utilities

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