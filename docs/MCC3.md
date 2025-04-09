mmc3
This mapper allows even more PRG and CHR banks to be selected, but doesn’t have the 5-write-quirk of MMC1. It does, though, have a scanline counter, which can trigger an IRQ on decrementing.

First of all, the addressable banks:

CPU $6000-$7FFF: 8 KB PRG RAM bank (optional)
CPU $8000-$9FFF (or $C000-$DFFF): 8 KB switchable PRG ROM bank
CPU $A000-$BFFF: 8 KB switchable PRG ROM bank
CPU $C000-$DFFF (or $8000-$9FFF): 8 KB PRG ROM bank, fixed to the second-last bank
CPU $E000-$FFFF: 8 KB PRG ROM bank, fixed to the last bank
PPU $0000-$07FF (or $1000-$17FF): 2 KB switchable CHR bank
PPU $0800-$0FFF (or $1800-$1FFF): 2 KB switchable CHR bank
PPU $1000-$13FF (or $0000-$03FF): 1 KB switchable CHR bank
PPU $1400-$17FF (or $0400-$07FF): 1 KB switchable CHR bank
PPU $1800-$1BFF (or $0800-$0BFF): 1 KB switchable CHR bank
PPU $1C00-$1FFF (or $0C00-$0FFF): 1 KB switchable CHR bank 
To select the banks, we can write to the Bank Register (0x8000-0x9fff). The writes are differentiated in even and odd, e.g. even 0x8000, 0x8202, etc, odd 0x8001, 0x8333 etc.

Even writes

7  bit  0 
---- ---- 
CPMx xRRR 
|||   ||| 
|||   +++- Specify which bank register to update on next write to Bank Data register 
|||        0: Select 2 KB CHR bank at PPU $0000-$07FF (or $1000-$17FF); 
|||        1: Select 2 KB CHR bank at PPU $0800-$0FFF (or $1800-$1FFF); 
|||        2: Select 1 KB CHR bank at PPU $1000-$13FF (or $0000-$03FF); 
|||        3: Select 1 KB CHR bank at PPU $1400-$17FF (or $0400-$07FF); 
|||        4: Select 1 KB CHR bank at PPU $1800-$1BFF (or $0800-$0BFF); 
|||        5: Select 1 KB CHR bank at PPU $1C00-$1FFF (or $0C00-$0FFF); 
|||        6: Select 8 KB PRG ROM bank at $8000-$9FFF (or $C000-$DFFF); 
|||        7: Select 8 KB PRG ROM bank at $A000-$BFFF 
||+------- Nothing on the MMC3, see MMC6 
|+-------- PRG ROM bank mode (0: $8000-$9FFF swappable, 
|                                $C000-$DFFF fixed to second-last bank; 
|                             1: $C000-$DFFF swappable, 
|                                $8000-$9FFF fixed to second-last bank) 
+--------- CHR A12 inversion (0: two 2 KB banks at $0000-$0FFF,                                  four 1 KB banks at $1000-$1FFF;
                              1: two 2 KB banks at $1000-$1FFF,                                  four 1 KB banks at $0000-$0FFF) 
So, with even writes we will dictate which bank will be changed, which PRG mode will be used, and if CHR A12 inversion is used or not.


The selection of these banks isn’t straightforward though, it’s depending on PRG mode and CHR A12 inversion, as can be seen in the following table.

CHR A12 inversion	unused	used
PPU Bank	Value of MMC3 register
$0000-$03FF	R0	R2
$0400-$07FF	R3
$0800-$0BFF	R1	R4
$0C00-$0FFF	R5
$1000-$13FF	R2	R0
$1400-$17FF	R3
$1800-$1BFF	R4	R1
$1C00-$1FFF	R5
PRG bank mode	0	1
CPU Bank	Value of MMC3 register
$8000-$9FFF	R6	(-2)
$A000-$BFFF	R7	R7
$C000-$DFFF	(-2)	R6
$E000-$FFFF	(-1)	(-1)
(-1) : the last bank
(-2) : the second last bank 
Odd writes

These writes, simply enough, are the bank number to the bank position that was select with the even write before.

irq / scanline counter
The scanline counter is used, for example, by Super Mario Bros. 3, to identify the position of the UI at the bottom. The UI will look garbled, if this function is not implemented, as we will see later on.

IRQ latch (0xc000-0xdfff, even)
This register holds the value, that will be loaded into the actual scanline counter when a reload is forced, or when the counter reached zero.

IRQ reload (0xc000-0xdfff, odd)
This register resets the actual scanline counter, and will push the IRQ latch value to the counter in the next scanline.

IRQ disable (0xe000-0xffff, even)
Writing to this register will disable the IRQ generation and also remove any pending IRQs.

IRQ enable (0xe000-$ffff, odd)
Writing to this address area will enable IRQ generation again.

In code, this will look something like this:

else if (adr >= 0xc000 && adr <= 0xdfff) {
    //  set reload value | even
    if ((adr % 2) == 0) {
        irq_latch = val;
    }
    //  actually set reload value to scanline counter | odd
    else {
        irq_reload_pending = true;
        irq_counter = 0;
    }
}
else if (adr >= 0xe000 && adr <= 0xffff) {
    //  IRQ disable | even
    if ((adr % 2) == 0) {
        irq_enabled = false;
        irq_assert = false;
    }
    //  IRQ enable | odd
    else {
        irq_enabled = true;
    }
}
scanline counter

The actual ticking of the scanline counter happens on every visible scanline, plus the prep-scanline, on cycle 260 of each line. The actual evaluation of the scanline counter is shown in code here, as this can often leads to problems and bugs, which may be hard to debug, due to conflicting documentation.

Note that, the nextScanline() function is only called, when either BG or Sprites are to be drawn (dictated from PPUMASK).

if (ppuScanline >= 0 && ppuScanline <= 239) {
    if (ppuCycles == 230) {
        renderScanline(ppuScanline);
    }
    else if (ppuCycles == 260 && (PPU_MASK.show_bg || PPU_MASK.show_sprites)) {
        nextScanline();
    }
}
virtual void nextScanline() {
    if (irq_counter == 0 || irq_reload_pending) {
        irq_counter = irq_latch;
        irq_reload_pending = false;
    }
    else {
        irq_counter--;
    }
 
    if (irq_counter == 0 && irq_enabled) {
        irq_assert = true;
    }
}
virtual bool IRQ() {
    bool isIRQ = irq_assert;
    irq_assert = false;
 
    return isIRQ;
}
This way, we are able to check for pending IRQs in our CPU. If the CPU detects a pending IRQ, it will (almost) do the same procedure as on an NMI. It will push the PC to the stack, push P / PSW to the stack, and jump to the Interrupt Vector, that is located at 0xfffe / 0xffff, and finally enable the interruptDisable flag of the PPUSTATUS.

If the IRQ and the scanline counter are implemented correctly, we will be able to boot Super Mario Bros. 3.