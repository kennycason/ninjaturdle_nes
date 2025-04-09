MMC1
Jump to navigationJump to search
MMC1
SxROM
Company	Nintendo, others
Games	390 in NesCartDB
Complexity	ASIC
Boards	SKROM, SLROM,
SNROM, others
Pinout	MMC1 pinout
PRG ROM capacity	256 KiB (512 KiB)
PRG ROM window	16 KiB + 16 KiB fixed or 32 KiB
PRG RAM capacity	32 KiB
PRG RAM window	8 KiB
CHR capacity	128 KiB
CHR window	4 KiB + 4 KiB or 8 KiB
Nametable mirroring	H, V, or 1, switchable
Bus conflicts	No
IRQ	No
Audio	No
iNES mappers	001, 105, 155
The Nintendo MMC1 is a mapper ASIC used in Nintendo's SxROM, NES-EVENT and 2ME circuit boards. It first appeared in the April of 1987.

iNES Mapper 1 and 155 denote the SxROM and 2ME circuit boards mounting the MMC1B (iNES Mapper 1) or MMC1A (iNES Mapper 155) revision of the ASIC; Mapper 1 is used if the revision is not known or irrelevant. Three submappers of mapper 1/155 are valid:

Submapper 5 denotes the SEROM/SHROM/SH1ROM circuit boards with 32 KiB of unbanked PRG-ROM.
Submapper 6 denotes the Famicom Network System's 2ME circuit board.
Submapper 7 denotes the Kaiser KS-7058 that mounts the KS 203 MMC1 clone, but with hard-wired nametable arrangement.
All other SxROM variants are denoted by their functional PRG/CHR-ROM/RAM sizes in the NES 2.0 header. Without NES 2.0, the PRG-RAM size has to be assumed; 32 KiB are sufficient for compatibility with all known titles.

INES Mapper 105 denotes the NES-EVENT circuit board.


Contents
1	Banks
2	Interface
2.1	Examples
2.1.1	Shift register
2.1.2	Reset
2.2	Consecutive-cycle writes
3	Registers
3.1	Load register ($8000-$FFFF)
3.2	Control (internal, $8000-$9FFF)
3.3	CHR bank 0 (internal, $A000-$BFFF)
3.4	CHR bank 1 (internal, $C000-$DFFF)
3.5	PRG bank (internal, $E000-$FFFF)
4	SxROM connection variants
4.1	SEROM, SHROM and SH1ROM
4.2	SNROM
4.2.1	CHR bank 0 (internal, $A000-$BFFF)
4.2.2	CHR bank 1 (internal, $C000-$DFFF)
4.3	SOROM, SUROM and SXROM
4.3.1	CHR bank 0 (internal, $A000-$BFFF)
4.3.2	CHR bank 1 (internal, $C000-$DFFF)
4.4	SZROM
4.4.1	CHR bank 0 (internal, $A000-$BFFF)
4.4.2	CHR bank 1 (internal, $C000-$DFFF)
4.5	Summary
5	2ME
5.1	Control (internal, $8000-9FFF)
5.2	CHR bank 0 (internal, $A000-BFFF)
5.3	PRG bank (internal, $E000-FFFF)
6	SxROM board types
7	ASIC Revisions
8	References
9	See also
Banks
CPU $6000-$7FFF: 8 KB PRG-RAM bank, (optional)
CPU $8000-$BFFF: 16 KB PRG-ROM bank, either switchable or fixed to the first bank
CPU $C000-$FFFF: 16 KB PRG-ROM bank, either fixed to the last bank or switchable
PPU $0000-$0FFF: 4 KB switchable CHR bank
PPU $1000-$1FFF: 4 KB switchable CHR bank
Through writes to the MMC1 control register, it is possible for the program to swap the fixed and switchable PRG-ROM banks or to set up 32 KB PRG bankswitching (like BNROM), but most games use the default setup, which is similar to that of UxROM.

Interface
Unlike almost all other mappers, the MMC1 is configured through a serial port in order to reduce its pin count. CPU $8000-$FFFF is connected to a common shift register. Writing a value with bit 7 set ($80 through $FF) to any address in $8000-$FFFF clears the shift register to its initial state. To change a register's value, the CPU writes five times with bit 7 clear and one bit of the desired value in bit 0 (starting with the low bit of the value). On the first four writes, the MMC1 shifts bit 0 into a shift register. On the fifth write, the MMC1 copies bit 0 and the shift register contents into an internal register selected by bits 14 and 13 of the address, and then it clears the shift register. Only on the fifth write does the address matter, and even then, only bits 14 and 13 of the address matter because the mapper doesn't see the lower address bits (similar to the mirroring seen with PPU registers). After the fifth write, the shift register is cleared automatically, so writing again with bit 7 set to clear the shift register is not needed.

Examples
Shift register
To switch a bank, a program will execute code similar to the following:

;
; Sets the switchable PRG-ROM bank to the value of A.
;
              ;  A          MMC1_SR  MMC1_PB
setPRGBank:   ;  000edcba    10000             Start with an empty shift register (SR).  The 1 is used
  sta $E000   ;  000edcba -> a1000             to detect when the SR has become full.
  lsr a       ; >0000edcb    a1000
  sta $E000   ;  0000edcb -> ba100
  lsr a       ; >00000edc    ba100
  sta $E000   ;  00000edc -> cba10
  lsr a       ; >000000ed    cba10
  sta $E000   ;  000000ed -> dcba1             Once a 1 is shifted into the last position, the SR is full.
  lsr a       ; >0000000e    dcba1             
  sta $E000   ;  0000000e    dcba1 -> edcba    A write with the SR full copies D0 and the SR to a bank register
              ;              10000             ($E000-$FFFF means PRG bank number) and then clears the SR.
  rts
But because only the fifth write sets the destination register, the following equivalent (if obfuscated) subroutine changes the PRG-ROM bank in the same manner:

setPRGBank:
  sta $8765
  lsr a
  sta $FACE
  lsr a
  sta $BA11
  lsr a
  sta $AD2E
  lsr a
  sta $EAD5
  rts
Reset
To reset the mapper, which clears the shift register and sets the PRG-ROM bank mode to 3 (fixing the last bank at $C000 and allowing the 16 KB bank at $8000 to be switched), one need only do a single write to any ROM address with a 1 in bit 7:

resetMapper:
  lda #$80
  sta $8000
  rts
Commonly, reset is done with an increment on a negative value to save 2 bytes. Because increment performs two writes, first writing the old value before the incremented one, it is recommended (such as for wider emulator compatibility) that the increment target a value that is negative on both writes, such as the INC opcode ($EE) itself:

resetMapper:
  inc resetMapper
  rts
Note that some games do a reset write to each of the 4 registers, even though it is only necessary to do 1 reset write to any register. It is suspected these games do this because of discrete logic implementations of MMC1 on development boards such as the MMC MULTI CHECKER-02, which uses separate shift registers for each MMC1 register.[1]

Consecutive-cycle writes
When the serial port is written to on consecutive cycles, it ignores every write after the first. In practice, this only happens when the CPU executes read-modify-write instructions, which first write the original value before writing the modified one on the next cycle.[2] This restriction only applies to the data being written on bit 0; the bit 7 reset is never ignored. Bill & Ted's Excellent Adventure does a reset by using INC on a ROM location containing $FF and requires that the $00 write on the next cycle is ignored. Shinsenden, however, uses illegal instruction $7F (RRA abs,X) to set bit 7 on the second write and will crash after selecting the みる (look) option if this reset is ignored.[3] This write-ignore behavior appears to be intentional and is believed to ignore all consecutive write cycles after the first even if that first write does not target the serial port.[4]

Registers
The following description generally applies to all boards mounting an MMC1 ASIC; refer to the connection variants section on how some registers bits have different meanings on particular circuit boards.

Load register ($8000-$FFFF)
7  bit  0
---- ----
Rxxx xxxD
|       |
|       +- Data bit to be shifted into shift register, LSB first
+--------- A write with bit set will reset shift register
            and write Control with (Control OR $0C), 
            locking PRG-ROM at $C000-$FFFF to the last bank.
On consecutive-cycle writes, writes to the shift register (D0) after the first are ignored. See Consecutive-cycle writes for more details.

Control (internal, $8000-$9FFF)
4bit0
-----
CPPMM
|||||
|||++- Nametable arrangement: (0: one-screen, lower bank; 1: one-screen, upper bank;
|||               2: horizontal arrangement ("vertical mirroring", PPU A10); 
|||               3: vertical arrangement ("horizontal mirroring", PPU A11) )
|++--- PRG-ROM bank mode (0, 1: switch 32 KB at $8000, ignoring low bit of bank number;
|                         2: fix first bank at $8000 and switch 16 KB bank at $C000;
|                         3: fix last bank at $C000 and switch 16 KB bank at $8000)
+----- CHR-ROM bank mode (0: switch 8 KB at a time; 1: switch two separate 4 KB banks)
Although some tests have found that all versions of the MMC1 seems to reliably power on in the last bank (by setting the "PRG-ROM bank mode" to 3); other tests have found that this is fragile. Several commercial games have reset vectors every 32 KiB, but not every 16, so evidently PRG-ROM bank mode 2 doesn't seem to occur randomly on power-up.

CHR bank 0 (internal, $A000-$BFFF)
4bit0
-----
CCCCC
|||||
+++++- Select 4 KB or 8 KB CHR bank at PPU $0000 (low bit ignored in 8 KB mode)
MMC1 can do CHR banking in 4KB chunks. Known carts with CHR-RAM have 8 KiB, so that makes 2 banks. RAM vs ROM doesn't make any difference for address lines. For carts with 8 KiB of CHR (be it ROM or RAM), MMC1 follows the common behavior of using only the low-order bits: the bank number is in effect ANDed with 1.

CHR bank 1 (internal, $C000-$DFFF)
4bit0
-----
CCCCC
|||||
+++++- Select 4 KB CHR bank at PPU $1000 (ignored in 8 KB mode)
PRG bank (internal, $E000-$FFFF)
4bit0
-----
RPPPP
|||||
|++++- Select 16 KB PRG-ROM bank (low bit ignored in 32 KB mode)
+----- MMC1B and later: PRG-RAM chip enable (0: enabled; 1: disabled; ignored on MMC1A)
       MMC1A: Bit 3 bypasses fixed bank logic in 16K mode (0: fixed bank affects A17-A14;
       1: fixed bank affects A16-A14 and bit 3 directly controls A17)
The high bit does not select a PRG-ROM bank. MMC1 with 512 KiB was supported by re-using a line from the CHR banking controls. (See below.)

MMC1A's 'R' bit logic was originally designed to support a hypothetical circuit board that connects the MMC1's A17 output to PRG-RAM A13, meaning a circuit board with 128 KiB of PRG-ROM and 16 KiB of PRG-RAM. Such a circuit board was never produced; games needing 16 KiB of PRG-RAM ended up using the SOROM or SZROM circuit boards that support more than 128 KiB of PRG-ROM and bank-switch PRG-RAM using other means.

SxROM connection variants
SEROM, SHROM and SH1ROM
Boards designed for 32 KiB PRG-ROM (SEROM, SHROM, and SH1ROM), such as Dr. Mario, do not connect PRG A14 to the MMC1; instead A14 is connected directly to the NES. As a result, register $E000 is completely without function, and the entire 32 KiB of PRG-ROM appear unbanked from CPU $8000-$FFFF. For compatibility with these, an emulator may switch to PRG bank 0 at power-on.

SNROM
SNROM, which only supports 8 KiB of CHR-ROM/-RAM, uses the upper CHR bank select line coming out of the mapper (CHR A16; bit 4 of bank number) as an additional chip enable for the PRG-RAM.[1]

CHR bank 0 (internal, $A000-$BFFF)
4bit0
-----
ExxxC
|   |
|   +- Select 4 KB CHR-RAM bank at PPU $0000 (ignored in 8 KB mode)
+----- PRG-RAM disable (0: enable, 1: open bus)
CHR bank 1 (internal, $C000-$DFFF)
4bit0
-----
ExxxC
|   |
|   +- Select 4 KB CHR-RAM bank at PPU $1000 (ignored in 8 KB mode)
+----- PRG-RAM disable (0: enable, 1: open bus) (ignored in 8 KB mode)
SOROM, SUROM and SXROM
All three address only 8 KiB of CHR-ROM/-RAM. They repurpose the upper CHR lines to increase the PRG-RAM size from 8 to 16 KiB (SOROM) or 32 KiB (SXROM), and to increase the addressable PRG-ROM size from 256 to 512 KiB (SUROM and SXROM).

CHR bank 0 (internal, $A000-$BFFF)
4bit0
-----
PSSxC
||| |
||| +- Select 4 KB CHR-RAM bank at PPU $0000 (ignored in 8 KB mode)
|++--- Select 8 KB PRG-RAM bank
+----- Select 256 KB PRG-ROM bank
CHR bank 1 (internal, $C000-$DFFF)
4bit0
-----
PSSxC
||| |
||| +- Select 4 KB CHR-RAM bank at PPU $1000 (ignored in 8 KB mode)
|++--- Select 8 KB PRG-RAM bank (ignored in 8 KB mode)
+----- Select 256 KB PRG-ROM bank (ignored in 8 KB mode)
The SOROM board only implements the upper S bit, while the SUROM board only implements the P bit. For SXROM, the upper S (bit 3) selects the SRAM's A14, and the lower S (bit 2) selects A13[5].

The 256 KB PRG bank selection applies to all the PRG area, including the normally "fixed" bank. It is therefore sometimes described as an "outer bank".

In 4KB CHR bank mode, SNROM's E bit and SO/U/XROM's P and S bits in both CHR bank registers must be set to the same values, or the PRG-ROM and/or RAM will be bankswitched/enabled as the PPU renders. As there is not much of a reason to use 4 KB bankswitching with CHR-RAM, it is wise for programs to just set 8 KB bankswitching mode in the Control register.

SZROM
SZROM addresses only 64 KiB of CHR-ROM/-RAM and uses the remaining CHR line to increase the PRG-RAM size from 8 to 16 KiB.

CHR bank 0 (internal, $A000-$BFFF)
4bit0
-----
RCCCC
|||||
|++++- Select 4 KB CHR-ROM bank at PPU $0000 (low bit ignored in 8 KB mode)
+----- Select 8 KB PRG-RAM bank
CHR bank 1 (internal, $C000-$DFFF)
4bit0
-----
RCCCC
|||||
|++++- Select 4 KB CHR-ROM bank at PPU $1000 (ignored in 8 KB mode)
+----- Select 8 KB PRG-RAM bank (ignored in 8 KB mode)
SZROM behaves similarly to SOROM, except that the PRG-RAM bank is a controlled by a different bit, and enough CHR is present that 4K bankswitching is desirable. The only currently-known game on SZROM is A Ressha de Ikou.

SZROM can be detected by a NES2.0 header specifying 8 KiB of PRG-RAM, 8 KiB of PRG-NVRAM, and 16 KiB or more of CHR.

Summary
The connection variants of SxROM that apply to registers $A000 and $C000 can be amalgamated as follows:

4bit0
-----
EDCBA
|||||
||||+- CHR A12
|||+-- CHR A13 if CHR >= 16 KiB
||+--- CHR A14 if CHR >= 32 KiB; and PRG-RAM A13 if PRG-RAM size is 32 KiB
|+---- CHR A15 if CHR >= 64 KiB; and PRG-RAM A13 if PRG-RAM size is 16 KiB
|                                or PRG-RAM A14 if PRG-RAM size is 32 KiB
+----- CHR A16 if CHR = 128 KiB; and PRG-ROM A18 if PRG-ROM size is 512 KiB
The E bit also acts as a PRG-RAM disable for SNROM (PRG-ROM size <= 256 KiB, CHR-RAM size = 8 KiB, PRG-RAM size = 8 KiB), though this is merely for write protection and not strictly required for compatible emulation.

The D bit used by SOROM (16k PRG-RAM) and SXROM (32 KiB PRG-RAM) controls a different address line depending on the board type. Using A13 for D with both boards and A14 for C will work, but will break SXROM battery save file compatibility with standard implementations.

2ME
This board, used with the Famicom Network System (FCNS), features 64 big-endian 16-bit words of EEPROM in addition to up to 32 KB of battery-backed PRG-RAM. Because FCNS cartridges are not on the PPU bus, all PPU-related MMC1 outputs are repurposed for EEPROM and PRG-RAM storage. The EEPROM's data output is mapped to $6000-7FFF bit 0 when enabled, and its command and data interface can be found in the 93LC46 datasheet. Note that open bus for this board is card open bus, which is open bus on the card side of the FCNS, not the console side.

2ME is assigned NES 2.0 MMC1 submapper 6 and uses an MMC1B.

Control (internal, $8000-9FFF)
7  bit  0
---- ----
...I PPEE
   | ||||
   | ||++- EEPROM CS (01 = enabled, otherwise disabled)
   | ++--- PRG-ROM bank mode (see standard register definition)
   +------ EEPROM DI enable (0 = DI forced to 0, 1 = DI output enabled)
The graphics-related bits here are repurposed for EEPROM enables. Because the MMC1's PPU A12-10 inputs are grounded on the 2ME board, settings that would normally vary based on PPU fetches such as the nametable arrangement and active CHR register are instead fixed.

The CIRAM A10 output is used as the EEPROM's CS input and will be always true in upper-bank single screen arrangement and always false otherwise.
The lowest CHR register bit is used for the EEPROM's DI input, but can only be non-zero in 4 KB banking mode. The second CHR register ($C000-DFFF) is unused in this mode because the PPU inputs are fixed such that they'll never point into the upper half of the pattern table space.
CHR bank 0 (internal, $A000-BFFF)
7  bit  0
---- ----
...O RRCI
   | ||||
   | |||+- EEPROM DI
   | ||+-- EEPROM CLK
   | |+--- PRG-RAM A13
   | +---- PRG-RAM A14
   +------ PRG-RAM /CE, and EEPROM DO +OE (0 = PRG-RAM enabled, 1 = EEPROM DO enabled)
EEPROM DI allows sending commands or data to the EEPROM. It is forced to 0 if 4 KB banking is not enabled via $8000 bit 4.
EEPROM CLK is used to transition to the next bit when reading from or writing to the EEPROM.
PRG-RAM banking uses the same bit order as SXROM.
The PRG-RAM /CE and EEPROM DO +OE bit acts as a selector for $6000-7FFF between PRG-RAM access and EEPROM read. The PRG-RAM enable matches the behavior of SNROM. While EEPROM is selected, bits 7-1 always return card open bus and bit 0 returns any data currently outputted by the EEPROM or card open bus otherwise.
PRG bank (internal, $E000-FFFF)
7  bit  0
---- ----
...D PPPP
   | ||||
   | ++++- PRG-ROM bank (see standard register definition)
   +------ $6000-7FFF disable (0 = enabled, 1 = disabled)
If $6000-7FFF is disabled, then neither PRG-RAM nor EEPROM can be accessed through this region and reads return card open bus. EEPROM can still be written, however, because the DI input does not use this region.

SxROM board types
The following SxROM boards are known to exist:

Board	PRG-ROM	PRG-RAM	CHR	Comments
SAROM	64 KB	8 KB	16 / 32 / 64 KB ROM	NES only
SBROM	64 KB		16 / 32 / 64 KB ROM	NES only
SCROM	64 KB		128 KB ROM	NES only
SC1ROM	64 KB		128 KB ROM	Uses 7432 for 28-pin CHR-ROM
SEROM	32 KB		16 / 32 / 64 KB ROM	
SFROM	128 / 256 KB		16 / 32 / 64 KB ROM	
SF1ROM	256 KB		64 KB ROM	PRG uses standard 32-pin EPROM pinout
SFEXPROM	256 KB		64 KB ROM	Patches PRG at runtime to correct a bad mask ROM run.
SGROM	128 / 256 KB		8 KB RAM/ROM	
SHROM	32 KB		128 KB ROM	NES only
SH1ROM	32 KB		128 KB ROM	Uses 7432 for 28-pin CHR-ROM
SIROM	32 KB	8 KB	16 / 32 / 64 KB ROM	Japan Only
SJROM	128 / 256 KB	8 KB	16 / 32 / 64 KB ROM	
SKROM	128 / 256 KB	8 KB	128 KB ROM	
SLROM	128 / 256 KB		128 KB ROM	
SL1ROM	64 / 128 / 256 KB		128 KB ROM	Uses 7432 for 28-pin CHR-ROM
SL2ROM	128 / 256 KB		128 KB ROM	CHR uses standard 32-pin EPROM pinout
SL3ROM	256 KB		128 KB ROM	Uses 7432 for 28-pin CHR-ROM
SLRROM	128 / 256 KB		128 KB ROM	Difference from SLROM unknown
SMROM	256 KB		8 KB RAM	Japan Only
SNROM	128 / 256 KB	8 KB	8 KB RAM/ROM	
SOROM	128 / 256 KB	16 KB	8 KB RAM/ROM	
STROM	Not MMC1, actually NROM
SUROM	512 KB	8 KB	8 KB RAM/ROM	
SXROM	128 / 256 / 512 KB	32 KB	8 KB RAM/ROM	Japan Only
SZROM	128 / 256 KB	16 KB	16-64 KB ROM	Japan Only
SLxROM boards are functionally identical to SLROM, but with different chip pinouts. Some of them have an additional 74HC32 chip to combine PPU /RD and PPU /A13 into a single enable signal for the CHR-ROM chip that has only 28 pins.
SMROM is functionally identical to SGROM, but features two 128 KB PRG-ROM chips instead of one 256 KB. Only a very early MMC1 game in Japan, Hokkaidou Rensa Satsujin: Ohoutsuku ni Shouyu, is known to have used this board, and one of very few Nintendo-made boards which combine smaller ROM chips to get a bigger ROM.
One SNROM game for Famicom, Morita Shogi, uses an 8 KiB CHR-ROM instead of CHR-RAM. The 6264 pinout is nearly identical to the pinout of an 8 KiB mask ROM, except for pins 26 and 27. On the 6264, these are a positive chip enable (CS2) and negative write enable (/WE) respectively; on the mask ROM, they may be additional positive chip enables. Either way, they're high during reads.
Solder pad config (SAROM, SJROM, SKROM, SNROM, SUROM, and SXROM only)
PRG-RAM retaining data : 'SL' disconnected, Battery, D1, D2, R1 R2 and R3 present.
PRG-RAM not retaining data : 'SL' connected, leave slots for Battery, D1, D2, R1, R2 and R3 free.
Even if the SOROM and SZROM boards utilizes a battery, it is connected to only one PRG-RAM chip. The first RAM chip will not retain its data, but the second one will.

ASIC Revisions
At least seven different versions of the MMC1 are known to exist: MMC1, MMC1A, MMC1B1, MMC1B1-H, MMC1B2, MMC1B2F, MMC1B3. The known differences relate to bit 4 of $E000.

MMC1A
PRG-RAM is always enabled at $6000-$7FFF. Bit 4 of $E000 causes bit 3 to directly control PRG-ROM A17 instead of going through the fixed bank logic.
MMC1B
PRG-RAM is enabled by default but can by disabled by bit 4 of $E000. $E000 bit 3 never bypasses the fixed bank.
The MMC1 most commonly exists in a 24-pin shrink-DIP package. An SOIC-24 incarnation MMC1B2F has been observed inside of a JRA-PAT Famicom Network System card.

Boards using an MMC1 may contain a battery connected to the PRG-RAM's power line to preserve the data. Boards doing so will allow extra circuitry to be used, with 2 diodes and 2 resistors. A diode is needed from both voltage sources: The battery and the NES 5V, so that one cannot supply current to the other, and there is a resistor in series with the battery as part of UL compliance. A pull-down resistor is needed on the CE line so that the SRAM is disabled when the MMC1 isn't powered. Finally, the battery powered SRAMs have an additional larger decoupling capacitor to make sure voltage transitions are smooth. Very early NES-SNROM-03 and lower revisions lacks that capacity, and saves are lost much more easily on those boards.

Nintendo transitioned from the original MMC1 (manufactured by ROHM) to the MMC1A (manufactured probably by Ricoh) around the 39th week of 1988. (Based on comparison of otherwise identical SMB/DH/WCTM carts from 38th and 39th weeks of '88). Two games (1, 2) are known to rely on MMC1A behavior, writing values between $10 and $1F to the PRG bank register, write to WRAM at $6000-$7FFF, and expect the write to succeed.

AX5904 is a third-party clone of the MMC1A.

References

23. Advanced mapper MMC1
A mapper is some circuitry on the cartridge that allows you to “map” more than 32k of PRG ROM and/or more than 8k of CHR ROM to the NES. By dividing a larger ROM into smaller “banks” and redirecting read/writes to different banks, you can trick the NES into allowing much larger ROMs. MMC1 was the most common mapper.

MMC1 – 681 games
MMC3 – 600 games
UxROM – 270 games
NROM – 248 games
CNROM – 155 games
AxROM – 76 games
*source BootGod

NES_SLROM

I borrowed this from Kevtris’s website. The smaller chip on the bottom left says “Nintendo MMC1A”.

MMC1 has the ability to change PRG banks and CHR banks. It can have PRG sizes up to 256k and CHR sizes up to 128k. (some rare variants could go up to 512k in PRG, but that won’t be discussed). It can change the mirroring from Horizontal to Vertical to One Screen. Metroid and Kid Icarus were MMC1, and they switch mirroring to scroll in different directions.

The MMC1 boards frequently had WRAM of 8k ($2000) bytes at $6000-7FFF, which could be battery backed to save a game. When I tried to make an NROM demo with WRAM, several emulators (and my PowerPak) decided that the WRAM didn’t exist because no NROM games ever had WRAM. But you wouldn’t have that problem with MMC1.

(In the most common arrangement…) The last PRG bank is fixed to $C000-FFFF and the $8000-BFFF can be mapped to any of the other banks. PRG banks are 16k ($4000) in size. Graphics can be swapped too. You can either change the entire pattern tables (PPU $0-1FFF) or change each separately (PPU $0-FFF and $1000-1FFF). CHR banks are 4k ($1000) in size. One thing you can do with swappable CHR banks is animate the background like Kirby’s Adventure does (by changing CHR banks every few frames).

I chose a 128k PRG ROM and 128k CHR ROM, and have it set to change each tileset separately.

Behind the scenes, the MMC1 mapper has registers at $8000,$A000,$C000, and $E000. It has to write 5 times to each, because it technically can only send 1 bit at a time. The $8000 register is the MMC1 control, the $A000 register changes the first CHR bank (tileset #0), the $C000 register changes the second CHR bank (tileset #1) (does nothing if CHR are in 8k mode), and the $E000 register changes which PRG bank is mapped to $8000-BFFF.

This is all tricky to program, in general, and more so for cc65. It is important to keep the main C code and all libraries in the fixed bank, including the init code (crt0.s) where the reset code and vectors are and neslib.s where the nmi code is. Level data should go in swapped banks. Infrequently used code should go in swapped banks.

Music is special. You would typically reserve an entire bank for music code and data. And all the music functions have to swap the music code/data in place to use it. You will need to explicitly put the music in a certain bank and change the SOUND_BANK definition to match it (in crt0.s).

Most of this new code was written by cppchriscpp with slight modification by me. Here’s the link to Chris’s code…

https://github.com/cppchriscpp/nes-starter-kit/tree/master/source

Things I changed.
I included all the files in the MMC1 folder. The .c and .h file at the top of the main .c file. The .asm files are included near the bottom of crt0.s.

In the header (crt0.s)…Flag 6 indicates the mapper # = 1 (MMC1). The NES_MAPPER symbol is defined in the .cfg file. Flags 8, indicate 1 PRG RAM (WRAM) bank. At the top of crt0.s the SOUND_BANK bank will need to be correct, and music put in the corresponding segment.

Also in crt0.s, I added the MMC1 reset code, and include the 2 .asm files in the MMC1 folder. I put the music in BANK 6, and now bank 6 is swapped before the music init code is called. All CHR files are put in the CHARS segment, which is 128k in size (it’s not completely filled).

Advertisement

The neslib.s file in the LIB folder has also been changed, specifically the nmi code and the music functions.

Each segment is defined in the .cfg file… MMC1_128_128.cfg. In the asm files, you just have to put a .segment “BANK4” to put everything below that in BANK 4. In the .c and .h files, you have to do this…

#pragma rodata-name (“BANK4”)
#pragma code-name (“BANK4”)

RODATA for Read Only data, like constant arrays.
CODE for code, of course.

Look at the ROM in a hex editor, and you can see how the linker constructed the ROM. I specifically wrote strings called “BANK0” in bank #0 and “BANK1” in bank #1, etc.

HexView

What’s new?
Banked calls are necessary, when calling a function in another bank.

banked_call(unsigned char bankId, void (*method)(void));

What this does is push the current PRG bank on an array, swap a new one in place, call the function with a function pointer, return, and pop the old bank back into place, then return. You can even nest banked_calls from one swapped bank to another, but there is a limit of 10 deep before it breaks. In fact, these banked_calls are very slow, so try to stay in one bank as much as possible before switching.

Also, music functions are in their own bank, and it has to do a similar…save bank, swap new one, jump there, return, pop bank, return… thing for any music or sfx call. So, try to minimize how many sfx you call in a frame.

set_prg_bank(unsigned char bank_id);

Use this to read data from a swappable bank (from the fixed bank). It sets a specific bank at $8000, and then you can access the data there.

set_chr_bank_0(unsigned char bank_id);
set_chr_bank_1(unsigned char bank_id);

Use these to change the CHR banks. bank_0 for the first set (which I used for background). bank_1 for the second set (which I used for sprites).

set_mirroring() to change the mirroring from Horizontal to Vertical to Single Screen.

If you are doing a split screen, like with a sprite zero hit, you could set the CHR bank that shows at the top of the screen with this function.

set_nmi_chr_tile_bank()

And turn it off with this function.

unset_nmi_chr_tile_bank()

These have been removed as of October 2022. See below.

Look at the code…
in main(), it uses

banked_call(BANK_0, function_bank0);

This function swaps bank #0 into place, then calls function_bank0(), which prints some text, “BANK0”, on the screen.

banked_call(BANK_1, function_bank1);

Does the same, but if you look at function_bank1(), it also calls

banked_call(BANK_2, function_bank2);

to show that you can nest one banked call inside another…up to 10 deep.

And we see that both “BANK1” and “BANK2” printed, so both of those worked.

Next we see that this banked_call() can’t take any extra arguments, so
you would have to pass arguments with global variables…I called them arg1 and arg2.

arg1 = ‘G’; // must pass arguments with globals
arg2 = ‘4’;
banked_call(BANK_3, function_bank3);

function_bank3() prints “BANK3” and “G4”, so we know that worked. Passing arguments by global is error prone, so be careful.

Skipping to banked_call(BANK_5, function_bank5);

function_bank5() also calls function_2_bank5() which is also in the same bank. You would use standard function calls for that, and not banked_call(). It printed “BANK5” and “ALSO THIS” so we know it worked alright. Use regular functions if it’s in the same bank.

Finally, banked_call(BANK_6, function_bank6); reads 2 bytes from the WRAM at $6000-7FFF. Just to have an example of it working. In the .cfg file I stated that there is a BSS segment there called XRAM. At the top of this .c file I declared a large array wram_array[] of 0x2000 bytes. You can read and write to it as needed.

It printed “BANK6” and “AC” (our test values) correctly.

Once we return to the main() function, we know we are in the fixed bank. Without using banked_call() we could swap a bank in place using set_prg_bank(). We could do that to read data in bank… like, for example, level data. You just read from it normally, as if that bank was always there.

I recommend you never use set_prg_bank() and then jumping to it without using banked_call(). The bank isn’t saved to the internal bank variable. If an NMI is triggered, the nmi code swaps the music code in place and then uses the internal bank variable to reset the swapped bank before returning… and that would be the wrong bank, and it would crash. Actually, this scenario might work without crashing. But I still recommend using the banked_call().

There is an infinite loop next, that reads the controller, and processes each button press.

Start button = changes the CHR bank. It calls this function…

set_chr_bank_0(char_state);

Which changes the background tileset. You notice that the sprite (the round guy) never changes. Sprites are using the second tileset. If we wanted to change the second tileset, we would use…

set_chr_bank_1();

MMC1_A

MMC1_B

MMC1_C

I am also testing the music code. Start calls a DMC sample. Button A calls the song play function music_play(). Button B calls a sound effect sfx_play(). and Select pauses and unpauses the music with music_pause().

Advertisement

I just wanted to make sure that the music code is working correctly, because I rewrote the function code. All the music data and code is in bank #6, and the code swaps in bank #6 and then calls the music function. Then swaps banks back again before returning.

I didn’t show any examples of changing the mirroring, but that is possible too.

Link to the code…

https://github.com/nesdoug/32_MMC1

I’m glad I got this working. Now on to actual game code. Oh, also… I was using MMC1_128_128.cfg but you could double the PRG ROM to 256k, by using the MMC1_256_128.cfg (edit the compile.bat linker command line arguments).

You could easily turn the WRAM at $6000-7FFF into save RAM by editing the header. Flags 6, indicate contains battery-backed SRAM, set bit 1… so add 2 to flags 6 in the header in crt0.s.

Maybe next time I will make an MMC3 demo, which can easily use 512k PRG ROM and 256k CHR ROM, and has a scanline counter. Also, there is a homebrew mapper 30, the oversized UNROM 512 board with extra CHR-RAM and (optional) 4-screen mirroring. Either would be easy to adapt the banked call system.

ONE MORE THING YOU SHOULD KNOW!
It has been brought to my attention that some of the MMC1 boards do not always reliably boot with the last bank in the fixed position at $c000. Some MMC1 games put reset vectors in every bank, which put the correct bank in place. My example code does not do that. It assumes the last bank is fixed and in place. Every emulator that I know of will boot it with the last bank fixed at $c000. But, this assumption may break if loaded onto a real cartridge. The user might have to hit reset a few times to get it to work. Or, they might get angry and play another game.

Update October 2022
I have made some changes (bug fixes) to the MMC1 example code to make it more stable (less likely to crash). A poorly timed bank switch that was interrupted by NMI would crash the game. This has been fixed. Also, the last bank pop from exiting a banked call wasn’t restoring the default bank (as you might have expected). That has been fixed.

Some other things have been changed. The startup code is different. Multiple instances of resetting the shift register have been added. The NMI code has changed.

set_nmi_chr_tile_bank() and unset_nmi_chr_tile_bank() have been removed. The CHR banks will automatically be rewritten at the top of each frame. If you need to change the CHR bank mid screen (say, with a sprite zero hit) for a split screen effect, use these functions after the sprite zero hit.

split_chr_bank_0() and split_chr_bank_1()

Which will temporarily change a CHR bank until the next NMI is reached.

I hope these changes work for you. I have not been able to test them on a real MMC1 board, but they are working well in all the emulators I’ve tested.


To continue our quest, we want to implement some of the most used (or at least used for the most popular games) mappers of the NES. For now, we will choose MMC1 and MMC3, a rather simple, and a rather complex mapper.

mmc1
The MMC1 mapper is a mapper, that offers banking for PRG and CHR ROM. This means, the actual memory behind the (mapped) addresses can be changed from the mapper, thus giving us possibly a lot more space on the cartridge.

CPU $6000-$7FFF: 8 KB PRG RAM bank, (optional)
CPU $8000-$BFFF: 16 KB PRG ROM bank, either switchable or fixed to the first bank
CPU $C000-$FFFF: 16 KB PRG ROM bank, either fixed to the last bank or switchable
PPU $0000-$0FFF: 4 KB switchable CHR bank
PPU $1000-$1FFF: 4 KB switchable CHR bank 
We get the amount of banks of the PRG banks (16kb) and of the CHR banks (8kb) from the iNES header, more precisely byte 4 and 5 of the header.

MMC1 has a quirk, where it will take 5 writes to collect all the bits for the actual write that sets a bank. These 5 writes all are made to the Load register (0x8000-0xffff) that consists of a shift register. Each write to the load register shifts the rightmost bit onto said shift register, and on the 5th write, the whole shift register is evaluated.

if (adr >= 0x8000 && adr <= 0xffff) {
        //  resetting shift register
        if (val & 0b10000000) {
            shift_register = 0b10000;
            sr_write_counter = 0;
            return;
        }
        //  shifting 4 times, writing on the 5th
        else {
            sr_write_counter++;
            shift_register = ((shift_register >> 1) | ((val & 1) << 4)) & 0b11111;
...
There are 4 address areas which control the following settings:

Control (0x8000-0x9fff)
4bit0 
----- 
CPPMM 
||||| 
|||++- Mirroring (0: one-screen, lower bank; 1: one-screen, upper bank; 
|||               2: vertical; 3: horizontal) 
|++--- PRG ROM bank mode (0, 1: switch 32 KB at $8000, ignoring low bit of bank number; 
|                         2: fix first bank at $8000 and switch 16 KB bank at $C000; 
|                         3: fix last bank at $C000 and switch 16 KB bank at $8000) 
+----- CHR ROM bank mode (0: switch 8 KB at a time; 1: switch two separate 4 KB banks) 
CHR bank 0 (0xa000-0xbfff)
4bit0 
----- 
CCCCC 
||||| 
+++++- Select 4 KB or 8 KB CHR bank at PPU $0000 (low bit ignored in 8 KB mode) 
CHR bank 1 (0xc000-0xdfff)
4bit0 
----- 
CCCCC 
||||| 
+++++- Select 4 KB CHR bank at PPU $1000 (ignored in 8 KB mode) 
PRG bank (0xe000-0xffff)
4bit0 
----- 
RPPPP 
||||| 
|++++- Select 16 KB PRG ROM bank (low bit ignored in 32 KB mode) 
+----- PRG RAM chip enable (0: enabled; 1: disabled; ignored on MMC1A) 
So, we are able to decide, which memory addresses have a fixed bank, and which are switchable. And the banks that are switchable, we can select through the bank selector. So, when we read from memory, we can tell our mapper to select the appropriate PRG and CHR banks on-the-fly.

virtual unsigned char read(uint16_t adr) {
    if (PRGsize == 0x4000) {
        //  First PRG area
        if (adr >= 0x8000 && adr <= 0xbfff) {
            //  this area is fixed to the first PRG bank
            if (PRGswitchAddr == 0xc000) {
                return m[(adr % 0x8000) + 0x10];
            }
            //  this area is switchable
            else {
                return m[(adr % 0x8000) + PRGid * PRGsize + 0x10];
            }
        }
        //  Second PRG area
        else if (adr >= 0xc000 & adr <= 0xffff) {
            //  this area is fixed to the last PRG bank
            if (PRGswitchAddr == 0x8000) {
                return m[(adr % 0xc000) + 0x10 + (romPRG16ks - 1) * PRGsize];
            }
            //  this area is switchable
            else {
                return m[(adr % 0x8000) + PRGid * PRGsize + 0x10];
            }
        }
        else {
            return memory[adr];
        }
    }
    //  32k PRG banks
    else if (PRGsize == 0x8000) {
        if (adr >= 0x8000 && adr <= 0xffff) {
            return m[(adr % 0x8000) + 0x10 + PRGid * PRGsize];
        }
        else {
            return memory[adr];
        }
    }
}
Note: 
The '0x10' in the reads, is the skipped iNES header.
The 'romPRG16ks' is the amount of banks we get from the iNES header.
This is the whole concept of the MMC1 mapper. After we successfully implemented this mapper, we are able to boot “Zelda“.





Programming MMC1
Jump to navigationJump to search
MMC1 was Nintendo's first ASIC mapper for the NES.


Contents
1	Quick setup for UNROM style
2	PRG banks
3	Interrupts
4	See also
5	References
6	External links
Quick setup for UNROM style
If you are using the SGROM or SNROM board to provide an environment similar to UNROM, with 8 KB of CHR RAM, a fixed PRG ROM bank at $C000, and a 16 KB switchable PRG ROM bank at $8000, do this in your init code after the mapper has been reset:

  lda #$0E   ; vertical mirroring, fixed $C000, 8 KB CHR pages
  sta $8000  ; (use $0F instead for horizontal mirroring)
  lsr a
  sta $8000
  lsr a
  sta $8000
  lsr a
  sta $8000
  lsr a
  sta $8000
Games that use CHR RAM switch to another PRG bank before they copy tile data into CHR RAM.

PRG banks
Some revisions of the MMC1 IC might power up in a mode other than fixed-$C000, requiring that the vectors and the start of the init code be placed in all banks, much as in BxROM or AxROM or GxROM. Other revisions guarantee that the fixed bank is loaded at power on. To make sure your code works on all MMC1 revisions, put the following code in the last 16 bytes of each 16384 byte bank. (Barbie uses almost identical code.)

reset_stub:
  sei
  ldx #$FF
  txs        ; set the stack pointer
  stx $8000  ; reset the mapper
  jmp reset  ; must be in $C000-$FFED
  .addr nmiHandler, reset_stub, irqHandler
Then to switch PRG ROM banks, load the bank number (0-15) into A and call this subroutine:

mmc1_load_prg_bank:
  sta $E000
  lsr a
  sta $E000
  lsr a
  sta $E000
  lsr a
  sta $E000
  lsr a
  sta $E000
  rts
Interrupts
If an NMI or IRQ can interrupt a series of writes, it is not easy to know what state the serial register was in before the interruption. One technique for coping with this problem involves using a flag variable to indicate that a serial write should be retried[1]:

The normal serial write routine should have an "interrupted" flag:
Clear the flag before beginning the series of writes.
When completed, check the flag. If an interrupt is indicated, reset the serial register, clear the interrupted flag and begin the serial writes again.
If the NMI or IRQ needs to switch banks, it should reset the serial register and set the "interrupted" flag to indicate that it has done this.
See also



---- 

logo
Home Forum Sections Community Submissions Changes Search Help
Welcome to Romhacking.net. 
 Log in
 
 Sign up
April 09, 2025, 12:16:58 PM
News:
11 March 2016 - Forum Rules

Home
Romhacking.net► Romhacking► Newcomer's Board► Adventures in NES Bank Switching: Episode 4 (Hokuto no Ken 3)
Adventures in NES Bank Switching: Episode 4 (Hokuto no Ken 3)
Started by BlackPaladin, March 20, 2023, 10:35:18 PM

Previous topic - Next topic
PrintGo Down Pages1
BlackPaladin

****
Sr. Member
Posts: 377
Logged

March 20, 2023, 10:35:18 PM
I may have hit a roadblock in my attempt at translating Hokuto no Ken 3 NES when it comes to implementing DTE.  I'll explain what I've done so far.

First off, using the debugger, I looked for the "sweet spot" when it comes to the game's text. ("B1 A2" in 0x3D8E7).  Where 0x3D8E5-0x3D8E8 is ("A0 00 B1 A2"), added "20 7B FF EA" (JSR $FF76 / NOP) and pasted "A0 00 B1 A2" onto a small area of unused nearby space (0x3FF86) with a "60" (RTS) right after.  I tested it out, and that seems to work fine currently. 

But here's the thing, this space doesn't have enough space for DTE code and/or accompanying DTE table.  However, there is unused space in $2FC5D-$2FFFF.  Because of this, I had to do bank switching.  After looking up some information at nesdev on Mapper 1 (the mapper this game uses) and help from a couple of people on Discord, the register for text may have been discovered.  Here's the code from the debugger...

Spoiler

Well, using this information, I also looked up what code to use when it comes to bank swapping.  Here's the code I came up with (with help from a couple of people on Discord). This was pasted over 0x3FF86. (Where "A0 00 B1 A2 60" is)  Here's how I interpreted how this code is supposed to work...

CodeSelect
  03FF86: A2 0B     LDX #$0B   <- Loads Bank 0B
  03FF88: 8A        TXA        <- Moves X Register to A
  03FF89: 20 E3 FF  JSR $FFE3  <- Jumps to Bank Switching subroutine
  03FF8C: 4C 5F FC  JMP $FC5F  <- Jumps to $2FC5F in Bank 0B
  03FF8F: A2 0F     LDX #$0F   <- Loads Bank 0F,
  03FF91: 8A        TXA        <- Moves X Register to A
  03FF95: 60        RTS        <- Ends Routine

I also pasted "20 7B FF EA 60" (JSR $FF76 / NOP / RTS) onto 0x2FC5F.  I had figured if I had done this, I can use the free space in $2FC5D-$2FFFF so that I can repurpose the DTE code I had used in Last Armageddon into this game as well as add an accompanying DTE table.  Anyway, when I put this into practice, this is what happened...



Yeah, not what I had expected.  :(

I don't know what went wrong here.  I thought this worked.  Can someone help me with what's going wrong?  (Thanks to leina and Gyroballer for helping out and getting me to this point so far.)

Cyneprepou4uk

*****
Hero Member
Posts: 1,468
The baldest romhacker
Logged
#1

March 21, 2023, 05:27:43 AM
CodeSelect
03FF8C: 4C 5F FC  JMP $FC5F  <- Jumps to $2FC5F in Bank 0B

Unless your mapper is in 32k PRG mode (which is unlikely), you're jumping to the last fixed bank, not to bank 0B.

In 16k PRG mode you can swap 8000-BFFF area only.
 https://github.com/cyneprepou4uk/NES-Games-Disassembly
BlackPaladin

****
Sr. Member
Posts: 377
Logged
#2

March 21, 2023, 10:02:17 AM
Quote from: Cyneprepou4uk on March 21, 2023, 05:27:43 AM
CodeSelect
03FF8C: 4C 5F FC  JMP $FC5F  <- Jumps to $2FC5F in Bank 0B

Unless your mapper is in 32k PRG mode (which is unlikely), you're jumping to the last fixed bank, not to bank 0B.

In 16k PRG mode you can swap 8000-BFFF area only.

So this means that this game has the following information in the mapper...

Quote
PRG ROM: 16 x 16KiB = 256 KiB
 CHR ROM: 0 x  8KiB = 0 KiB
 ROM CRC32: 0x4e7cad28
 ROM MD5:  0x39b549ef5d37dd661c0aeddca2b84c9f
 Mapper #: 1
 Mapper name: MMC1
 Mirroring: Horizontal
 Battery-backed: Yes
 Trained: No
 NES2.0 Extensions
 Sub Mapper #: 0
 Total WRAM size: 8 KiB
 Total VRAM size: 8 KiB
 WRAM backed by battery: 8 KiB
 VRAM backed by battery: 0 KiB

There's no way I can bank swap to $2FC5F?  Does it mean if I wanted to bank swap, it has to be in $8000-$BFFF, $18000-1BFFF, $28000-$2BFFF, $38000-3BFFF, etc.?

Cyneprepou4uk

*****
Hero Member
Posts: 1,468
The baldest romhacker
Logged
#3

March 21, 2023, 12:32:50 PM
Last Edit: March 22, 2023, 05:52:32 AM by Cyneprepou4uk
Seems that you still don't understand what PRG bankswitching is all about.

Basically, CPU can access 0000-FFFF range via memory bus, and not more than that. 8000-FFFF is used for PRG, which is 32k, and CPU can access PRG within that range only (some mappers allow 6000-7FFF as well). However, games can have larger PRG than 32k, and by default CPU cannot access 0x2FC5F or whatever. So by using mappers you can connect different PRG banks to memory bus via their registers.

For example, your game allows you to connect 16k banks to 8000-BFFF range. If you connect bank 00 and execute JMP $8000, you will be jumping to 0x00010. If you connect bank 01 and execute the same JMP $8000, this time you will be jumping to 0x04010.

Here you can see how after the last write to FFFF register, memory at 8000-BFFF is changing to a different one in Hex Editor. I can "Go Here In ROM File" to see which PRG bank is connected at the moment. At the same time it's still 8000-BFFF for CPU, which means that your instructions must operate with 8000-BFFF range in order to do something with connected bank.



Also, your game's mapper configuration connects last 16k of PRG to C000-FFFF, and you can't swap it to another bank. It's called a fixed bank. So your JMP $FC5F will always jump to 0x3FC6F despite what is loaded into 8000-BFFF.

A fixed bank is used as a transitional place between bankswitching, because bad things will happen if you execute bankswitching code while being inside that bankswitching area (unless you know what you're doing). Also it contains common code for other banks in order for them to have a constant access to (math calculations for example). And of course it has interrupt vectors and their handlers. And sometimes DPCM music data.
 https://github.com/cyneprepou4uk/NES-Games-Disassembly
BlackPaladin

****
Sr. Member
Posts: 377
Logged
#4

March 22, 2023, 12:02:12 AM
Last Edit: March 22, 2023, 12:44:19 PM by BlackPaladin
Quote from: Cyneprepou4uk on March 21, 2023, 12:32:50 PM
Seems that you still don't understand what PRG bankswitching is all about.

Basically, CPU can access 0000-FFFF range via memory bus, and not more than that. 8000-FFFF is used for PRG, which is 32k, and CPU can access PRG within that range only (some mappers allow 6000-7FFF as well). However, games can have larger PRG than 32k, and by default CPU cannot access 0x2FC5F or whatever. So by using mappers you can connect different PRG banks to memory bus via their registers.

For example, your game allows you to connect 16k banks to 8000-BFFF range. If you connect bank 00 and execute JMP $8000, you will be jumping to 0x00010. If you connect bank 01 and execute the same JMP $8000, this time you will be jumping to 0x04010.

Here you can see how after the last write to FFFF register, memory at 8000-BFFF is changing to a different one in Hex Editor. I can "Go Here In ROM File" to see which PRG bank is connected at the moment. At the same time it's still 8000-BFFF for CPU, which means that your instructions must operate with 8000-BFFF range in order to do something with connected bank.



Also, your game's mapper configuration connects last 16k of PRG to C000-FFFF, and you can't swap it to another bank. It's called a fixed bank. So your JMP $FC5F will always jump to 0x3FC6C despite what is loaded into 8000-BFFF.

A fixed bank is used as a transitional place between bankswitching, because bad things will happen if you execute bank switching code while being in that bankswitching area (unless you know what you're doing). Also it contains common code for other banks in order for them to have a constant access to (math calculations for example). And of course it has interrupt vectors and their handlers. And sometimes DPCM music data.

I guess this means I cannot use the same DTE hack I used in Last Armageddon in this game as the sweet spot is in a fixed bank and cannot be moved and bank-switched out of it.

That's unfortunate.  I thought I could add DTE, but it looks like using KingMike's documentation on the subject (which I used for LA) cannot be used here.  *sigh*  Disappointed... :(

Cyneprepou4uk

*****
Hero Member
Posts: 1,468
The baldest romhacker
Logged
#5

March 23, 2023, 06:40:53 AM
Looks doable to me. I saw your screenshot in Discord



You read a byte and make a decision on whether DTE needs to be used. If yes, you swap bank and do your stuff, then swap back to read next byte. 900 free bytes in bank 0B should be enough.

Based on the screenshot, code which is reading text is located in a fixed bank, which means that you will have access to it from bank 0B if necessary. I don't know what are you complaining about.
 https://github.com/cyneprepou4uk/NES-Games-Disassembly
BlackPaladin

****
Sr. Member
Posts: 377
Logged
#6

March 23, 2023, 09:14:29 PM
Now, I'm confused.  Can you contact me on Discord?

PrintGo Up Pages1
Romhacking.net► Romhacking► Newcomer's Board► Adventures in NES Bank Switching: Episode 4 (Hokuto no Ken 3)
Jump to
=> Newcomer's Board
 
Help | Terms and Rules | Go Up ▲
SMF 2.1.4 © 2023, Simple Machines
Page created in 0.720 seconds with 20 queries.