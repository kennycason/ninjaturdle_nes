# CHR Bank Viewer Project Status

## Current Features
1. Displays CHR ROM banks in a 16x16 grid
2. Supports 4 banks (0-3) switchable with A button
3. Shows top/bottom pattern tables with B button
4. 10 color palettes cycled with Up/Down buttons
5. Status display showing:
   - Current bank number
   - CHR0/CHR1 bank numbers
   - Current half (T/B)
   - Current palette number

## Known Issues
1. Border tile (0th tile) shows up in non-font banks
2. Font rendering not working correctly in non-zero banks
3. Need to optimize text rendering strategy

## Current Implementation
- Using MMC1 mapper in 4KB CHR bank mode
- Font tiles stored in bank 0
- Pattern table switching using bank_spr/bank_bg
- Global state tracking for bank, palette, and half selection

## Needed Improvements
1. Skip rendering empty positions (especially border tile)
2. Better font handling strategy:
   - Keep commonly used numbers (0-9) in main sprite sheets
   - Minimize text rendering during gameplay
   - Batch text operations when needed
3. Proper bank switching for font rendering

## File Structure
Main file: `ninjaturdle/bank_swap.c`
Dependencies:
- neslib.h
- nesdoug.h
- kenes.h (MMC1 support)

## Control Scheme
- A: Next CHR bank
- B: Toggle top/bottom half
- Up: Previous palette
- Down: Next palette 