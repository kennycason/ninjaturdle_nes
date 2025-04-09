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

// Number of palettes
#define NUM_PALETTES 10

// Global state
static unsigned char current_block = 0;  // 0-3: represents which 128x128 block we're viewing
static unsigned char current_palette = 0;  // Palette index

// String constants
const char str_bank[] = "BANK:";
const char str_pt[] = "PT:";
const char str_pal[] = "PAL:";

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

// Function to draw text at a specific position
void draw_text_at(unsigned char x, unsigned char y, const char* str) {
    vram_adr(NTADR_A(x, y));
    while (*str) {
        vram_put(*str - 0x20);
        str++;
    }
}

// Function to draw a number at a specific position
void draw_number_at(unsigned char x, unsigned char y, unsigned char num) {
    vram_adr(NTADR_A(x, y));
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
    draw_text_at(2, 2, "BANK:");
    draw_number_at(7, 2, current_block);
    
    vram_adr(NTADR_A(18,2));
    draw_text_at(18, 2, "PAL:");
    draw_number_at(22, 2, current_palette);
}

void draw_tile_grid(void) {
    unsigned char row, col;
    
    // Set palette
    pal_col(0, palettes[current_palette * 4 + 0]);
    pal_col(1, palettes[current_palette * 4 + 1]);
    pal_col(2, palettes[current_palette * 4 + 2]);
    pal_col(3, palettes[current_palette * 4 + 3]);
    
    ppu_off();
    
    // Set both pattern tables to the current bank
    // current_block maps to 4KB banks:
    // 0 = first bank top (0-127)
    // 1 = first bank bottom (128-255)
    // 2 = second bank top (0-127)
    // 3 = second bank bottom (128-255)
    mmc1_write(MMC1_CTRL, 0x12);  // Ensure 4KB mode is set
    mmc1_write(MMC1_CHR0, current_block);
    mmc1_write(MMC1_CHR1, current_block);
    
    // Draw 16x16 grid of tiles (always showing tiles 0-127)
    for (row = 0; row < 16; ++row) {
        vram_adr(NTADR_A(2, row + 4));
        for (col = 0; col < 16; ++col) {
            vram_put(row * 16 + col);
        }
    }
    
    // Keep current bank active when turning PPU on
    ppu_on_all();
}

void main(void)
{
    // Initial setup routines (PPU, etc.)
    ppu_off();
    
    // Initialize MMC1 with 4KB CHR bank mode
    mmc1_write(MMC1_CTRL, 0x80);  // Reset
    mmc1_write(MMC1_CTRL, 0x12);  // 4KB mode, vertical mirroring
    mmc1_write(MMC1_CHR0, 0);
    mmc1_write(MMC1_CHR1, 0);
    
    // Set initial palette
    pal_all(palettes);
    
    // Draw the initial screen
    draw_tile_grid();
    
    while(1)
    {
        // Poll for controller input
        char input = pad_trigger(0);
        
        // Cycle through blocks with A button
        if (input & PAD_A) {
            current_block = (current_block + 1) % 4;  // Cycle through all 4 blocks
            draw_tile_grid();
        }
        
        // Cycle through palettes with Up/Down buttons
        if (input & PAD_UP) {
            current_palette = (current_palette + 1) % NUM_PALETTES;
            draw_tile_grid();
        }
        
        if (input & PAD_DOWN) {
            current_palette = (current_palette + NUM_PALETTES - 1) % NUM_PALETTES;
            draw_tile_grid();
        }
        
        // Wait for NMI to complete
        ppu_wait_nmi();
    }
}