# makefile for nesdoug example code, for Linux
# this version won't move the final files to BUILD folder
# also won't rebuild on changes to neslib/nesdoug/famitone files

CC65 = ../tools/cc65/bin/cc65
CA65 = ../tools/cc65/bin/ca65
LD65 = ../tools/cc65/bin/ld65
NAME = ninjaturdle
CFG = nrom_32k_vert.cfg


.PHONY: default clean

default: $(NAME).nes


#target: dependencies

$(NAME).nes: $(NAME).o crt0.o $(CFG)
	$(LD65) -C $(CFG) -o $(NAME).nes crt0.o $(NAME).o nes.lib -Ln labels.txt --dbgfile dbg.txt
	rm *.o
	@echo $(NAME).nes created

crt0.o: crt0.s ninjaturdle.chr ninjaturdle2.chr MUSIC/TestMusic3.s MUSIC/SoundFx.s
	$(CA65) crt0.s

$(NAME).o: $(NAME).s
	$(CA65) $(NAME).s -g

$(NAME).s: $(NAME).c sprites.h ninjaturdle.h level_data.c NES_ST/title.h BG/Level1.c BG/Level2.c BG/Level3.c
	$(CC65) -Oirs $(NAME).c --add-source

clean:
	rm $(NAME).nes
