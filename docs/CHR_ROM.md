Row 0: [00][01][02][03][04][05][06][07][08][09][0A][0B][0C][0D][0E][0F]
Row 1: [10][11][12][13][14][15][16][17][18][19][1A][1B][1C][1D][1E][1F]

Ah, let me clarify the NES CHR ROM structure:
Each NES pattern table is 128×128 pixels (16×16 tiles of 8×8 pixels each)
Pattern Table 0: $0000-$0FFF (4KB)
Pattern Table 1: $1000-$1FFF (4KB)
Together they form a 128×256 pixel area (not 256×256 as I incorrectly implied)
Your CHR files:
ninjaturdle.png and ninjaturdle2.png are each 128×256 pixels
Each one contains TWO pattern tables (top 128×128 and bottom 128×128)
Each file is 8KB (4KB top + 4KB bottom)
You're absolutely right - I was incorrect earlier. Let me revise my understanding:
ninjaturdle.png (8KB, Bank 0):
Top 128×128: Font tiles
Bottom 128×128: GUI/menu tiles
ninjaturdle2.png (8KB, Bank 1):
Top 128×128: Map tiles
Bottom 128×128: Sprite/enemy tiles
The MMC1 can switch these 4KB chunks independently, which is why we're using 4KB bank switching mode. Each 128×128 section is exactly 4KB (16×16 tiles × 8×8 pixels × 2 bits per pixel).


-- reply --
LOL @ myself. I read in one of the NES MMCx wikis that to reset the MMC1 you write $80 to $FF to the register. That's what my reset loop is doing, in a rather clumsy way. Oh dear :D

I was just thinking about the NMI issue, couldn't you increment a flag in the NMI, read it at the start of your bank switching routine and also at the end and if it's changed, write the bank switch again?

Kind of like;
LOL @ myself. I read in one of the NES MMCx wikis that to reset the MMC1 you write $80 to $FF to the register. That's what my reset loop is doing, in a rather clumsy way. Oh dear :D

I was just thinking about the NMI issue, couldn't you increment a flag in the NMI, read it at the start of your bank switching routine and also at the end and if it's changed, write the bank switch again?

Kind of like;

NMI:
  inc NMI_counter
  rti

SET_BANK:
  ldx NMI_counter
  sta $E000
  lsr a
  etc
  cpx NMI_counter
  bne SET_BANK
  rts


-- reply --
I was just thinking about the NMI issue, couldn't you increment a flag in the NMI, read it at the start of your bank switching routine and also at the end and if it's changed, write the bank switch again?
Oh my this is clever, much better than what I did. You'll have to reset the mapper in NMI before writing tough (implying this method can't be used if you use a non-standard bankswitching sheme), and like my complex solution, you can't use the same bankswitching routines inside and outside NMI.

So in summary when you use MMC1 :
- If your NMI timing is VERY tight and if you use the standard banswitching method, do what Neil sugested (and forgot about my complex approach)
- If it is acceptable to have a slightly shorter NMI, disable NMI (via $2000) between the MMC1 writes and enable it back after that.


-- reply --
This NMI handler will not interfere with bankswitching:
Code: Select all

.proc nmi_handler
  inc retraces
  rti
.endproc
Then the main loop can spin on that flag to see when to blast VRAM updates.
