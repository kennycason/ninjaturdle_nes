#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "../neslib/neslib.h"

// general vars
static uint8_t spr;
static uint8_t input;

// Constants for the sprite grid
#define GRID_WIDTH   16
#define GRID_ROWS    8
#define SPRITE_SIZE  8
#define GRID_SPACING 16
#define START_X      32
#define START_Y      32

// Palette data
const uint8_t sprites_pal[32] = {
    0x0f, 0x17, 0x27, 0x37, // Palette 0
    0x0f, 0x29, 0x21, 0x31, // Palette 1
    0x0f, 0x15, 0x25, 0x35, // Palette 2
    0x0f, 0x19, 0x29, 0x39, // Palette 3
    0x0f, 0x05, 0x10, 0x10, // Palette 4
    0x0f, 0x07, 0x12, 0x14, // Palette 5
    0x0f, 0x30, 0x8f, 0x33, // Palette 6
    0x0f, 0x00, 0x10, 0x30  // Palette 7
};

// Function to handle user input
static void handle_input() {
    input = pad_poll(0);
}

// Function to render the spritesheet grid
static void render_spritesheet() {
    uint8_t x, y, tile, palette;
    uint8_t row, col;
    
    oam_clear();
    spr = 0;
    
    // Display all 128 tiles (16x8 grid)
    for (row = 0; row < GRID_ROWS; row++) {
        for (col = 0; col < GRID_WIDTH; col++) {
            // Calculate position
            x = START_X + col * GRID_SPACING;
            y = START_Y + row * GRID_SPACING;
            
            // Calculate tile number
            tile = row * GRID_WIDTH + col;
            
            // Use different palettes for different rows
            palette = row % 8;
            
            // Draw the sprite
            spr = oam_spr(x, y, tile, palette, spr);
        }
    }
}

void main(void) {
    // Set up the palette
    pal_spr((const char*)sprites_pal);
    
    // Enable rendering
    ppu_on_all();
    
    // Main game loop
    while (1) {
        // Wait for next frame
        ppu_wait_frame();
        
        // Handle input
        handle_input();
        
        // Render the spritesheet
        render_spritesheet();
    }
}
