# makefile for nes game + char bank browser

CC65 = ../tools/cc65/bin/cc65
CA65 = ../tools/cc65/bin/ca65
LD65 = ../tools/cc65/bin/ld65
NAME = ninjaturdle
BANK_SWAP = bank_swap
CFG = mmc1_32k.cfg


.PHONY: default clean

default: $(NAME).nes $(BANK_SWAP).nes


#target: dependencies

$(NAME).nes: $(NAME).o crt0.o $(CFG)
	$(LD65) -C $(CFG) -o $(NAME).nes crt0.o $(NAME).o nes.lib -Ln $(NAME)_labels.txt --dbgfile $(NAME)_dbg.txt

$(BANK_SWAP).nes: $(BANK_SWAP).o crt0.o $(CFG)
	$(LD65) -C $(CFG) -o $(BANK_SWAP).nes crt0.o $(BANK_SWAP).o nes.lib -Ln $(BANK_SWAP)_labels.txt --dbgfile $(BANK_SWAP)_dbg.txt

crt0.o: crt0.s ninjaturdle.chr ninjaturdle2.chr MUSIC/TestMusic3.s MUSIC/SoundFx.s
	$(CA65) crt0.s -g

$(NAME).o: $(NAME).s
	$(CA65) $(NAME).s -g

$(NAME).s: $(NAME).c sprites.h ninjaturdle.h level_data.c NES_ST/title.h BG/Level1.c BG/Level2.c BG/Level3.c BG/w1l1_main.c BG/w1l1_object.c BG/Level1_SP.c
	$(CC65) -Oirs $(NAME).c --add-source

$(BANK_SWAP).o: $(BANK_SWAP).s
	$(CA65) $(BANK_SWAP).s -g

$(BANK_SWAP).s: $(BANK_SWAP).c
	$(CC65) -Oirs $(BANK_SWAP).c --add-source

clean:
	rm -f $(NAME).nes $(BANK_SWAP).nes *.o *_labels.txt *_dbg.txt
