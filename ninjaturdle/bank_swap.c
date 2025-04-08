#include "LIB/neslib.h"
#include "LIB/nesdoug.h"
#include "LIB/kenes.h"  // Add this include for MMC1 definitions

// MMC1 registers
#define MMC1_CTRL    0x8000
#define MMC1_CHR0    0xA000  // First 4KB bank ($0000-$0FFF)
#define MMC1_CHR1    0xC000  // Second 4KB bank ($1000-$1FFF)
#define MMC1_PRG     0xE000

// Control register bits
#define CTRL_CHR_4K    0x10  // 4KB CHR bank mode
#define CTRL_PRG_32K   0x00  // 32KB PRG ROM mode
#define CTRL_MIRROR_V  0x02  // Vertical mirroring

// Bank definitions
#define BANK0_TOP     0    // Font tiles (top of first bank)
#define BANK0_BOTTOM  1    // GUI/menu tiles (bottom of first bank)
#define BANK1_TOP     2    // Map tiles (top of second bank)
#define BANK1_BOTTOM  3    // Sprite tiles (bottom of second bank)

// Global state
unsigned char current_bank = 0;    // Current bank (0-3 for 4KB banks)
unsigned char show_bottom_half = 0;  // 0 = top half, 1 = bottom half
unsigned char current_palette = 0;  // Current palette (0-9)

// String constants
const unsigned char str_bank[] = "BANK:";
const unsigned char str_pt[] = "PT:";
const unsigned char str_pal[] = "PAL:";

// Palettes
const unsigned char palettes[] = {
    0x0F, 0x00, 0x10, 0x30,   // Default grayscale
    0x0F, 0x06, 0x16, 0x26,   // Red tones
    0x0F, 0x09, 0x19, 0x29,   // Green tones
    0x0F, 0x01, 0x11, 0x21,   // Blue tones
    0x0F, 0x08, 0x18, 0x28,   // Yellow tones
    0x0F, 0x0A, 0x1A, 0x2A,   // Purple tones
    0x0F, 0x07, 0x17, 0x27,   // Brown tones
    0x0F, 0x02, 0x12, 0x22,   // Deep blue
    0x0F, 0x05, 0x15, 0x25,   // Magenta
    0x0F, 0x03, 0x13, 0x23    // Dark green
};

void draw_text(const unsigned char* str, unsigned char len) {
    unsigned char i;
    for(i = 0; i < len; ++i) {
        vram_put(str[i] - 0x20);
    }
}

void draw_number(unsigned char num) {
    vram_put('0' + num - 0x20);
}

void draw_border_and_text(void) {
    unsigned char row, col;
    
    // Always use bank 0 for text/border
    mmc1_write(MMC1_CHR0, 0);
    mmc1_write(MMC1_CHR1, 0);  // Keep both banks on font
    
    // Draw border
    vram_adr(NTADR_A(1,3));
    for(col = 0; col < 18; ++col) vram_put(0x00);
    
    for(row = 0; row < 16; ++row) {
        vram_adr(NTADR_A(1, row + 4));
        vram_put(0x00);
        vram_adr(NTADR_A(18, row + 4));
        vram_put(0x00);
    }
    
    vram_adr(NTADR_A(1,20));
    for(col = 0; col < 18; ++col) vram_put(0x00);
    
    // Draw status text
    vram_adr(NTADR_A(2,2));
    draw_text(str_bank, 5);
    draw_number(current_bank);
    
    vram_adr(NTADR_A(18,2));
    draw_text(str_pal, 4);
    draw_number(current_palette);
}

void draw_tile_grid(void) {
    unsigned char row, col;
    unsigned char tile_offset = show_bottom_half ? 128 : 0;  // Start at 128 for bottom half
    
    // Set palette
    pal_col(0, palettes[current_palette * 4 + 0]);
    pal_col(1, palettes[current_palette * 4 + 1]);
    pal_col(2, palettes[current_palette * 4 + 2]);
    pal_col(3, palettes[current_palette * 4 + 3]);
    
    ppu_off();
    
    // First draw text using font bank
    mmc1_write(MMC1_CHR0, BANK0_TOP);
    mmc1_write(MMC1_CHR1, BANK0_TOP);
    draw_border_and_text();
    
    // Now draw tiles using current bank
    mmc1_write(MMC1_CHR0, current_bank);
    mmc1_write(MMC1_CHR1, current_bank);
    
    // Draw 16x16 grid of tiles
    for (row = 0; row < 16; ++row) {
        vram_adr(NTADR_A(2, row + 4));
        for (col = 0; col < 16; ++col) {
            vram_put(tile_offset + row * 16 + col);
        }
    }
    
    // Keep current bank active when turning PPU on
    ppu_on_all();
}

void main(void) {
    unsigned char pad_current;
    unsigned char pad_prev = 0;
    
    // Initialize MMC1
    mmc1_write(MMC1_CTRL, CTRL_MIRROR_V | CTRL_PRG_32K | CTRL_CHR_4K);
    mmc1_write(MMC1_CHR0, BANK0_TOP);  // Start with font bank
    mmc1_write(MMC1_CHR1, BANK0_TOP);  // Keep both banks on font
    mmc1_write(MMC1_PRG, 0);   // PRG bank 0
    
    // Initial palette setup
    pal_col(0, palettes[0]);
    pal_col(1, palettes[1]);
    pal_col(2, palettes[2]);
    pal_col(3, palettes[3]);
    
    // Initial draw
    draw_tile_grid();
    
    // Main loop
    while(1) {
        ppu_wait_nmi();
        pad_current = pad_poll(0);
        
        if (pad_current != pad_prev) {
            if ((pad_current & PAD_A) && !(pad_prev & PAD_A)) {
                // Cycle through banks 0-3
                current_bank = (current_bank + 1) & 3;
                draw_tile_grid();
            }
            
            if ((pad_current & PAD_B) && !(pad_prev & PAD_B)) {
                // Toggle between top/bottom half
                show_bottom_half ^= 1;
                draw_tile_grid();
            }
            
            if ((pad_current & PAD_UP) && !(pad_prev & PAD_UP)) {
                current_palette = (current_palette + 9) % 10;
                draw_tile_grid();
            }
            
            if ((pad_current & PAD_DOWN) && !(pad_prev & PAD_DOWN)) {
                current_palette = (current_palette + 1) % 10;
                draw_tile_grid();
            }
        }
        
        pad_prev = pad_current;
    }
} 