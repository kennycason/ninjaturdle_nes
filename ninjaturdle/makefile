##############################################################################
# Paths to cc65 tools
##############################################################################
CC65 = ../tools/cc65/bin/cc65
CA65 = ../tools/cc65/bin/ca65
LD65 = ../tools/cc65/bin/ld65

# Linker config (adjust if needed)
CFG  = mmc1_32k.cfg

# Compiler/assembler flags
CFLAGS = -Oirs --add-source
AFLAGS =

# Runtime library path
RUNTIME = ../tools/cc65/lib/nes.lib

# Disable default .c/.o rules so it won't fall back to clang
.SUFFIXES:

##############################################################################
# Source files
##############################################################################
CSRC = \
  ninjaturdle.c \
  bank_swap.c

# Level files to compile
LEVEL_FILES = \
  map/w1l1.c \
  map/w1l2.c \
  map/w1l3.c

# Object files from level files
LEVEL_OBJS = $(LEVEL_FILES:.c=.o)

# All object files
OBJS = $(CSRC:.c=.o) $(LEVEL_OBJS) crt0.o

##############################################################################
# Default target
##############################################################################
all: ninjaturdle.nes

##############################################################################
# Rule: compile .c -> .o (cc65 -> .s -> ca65 -> .o)
##############################################################################
%.o: %.c
	$(CC65) $(CFLAGS) $< -o $(<:.c=.s)
	$(CA65) $(AFLAGS) $(<:.c=.s) -o $@

##############################################################################
# Rule: assemble .s -> .o
##############################################################################
%.o: %.s
	$(CA65) $(AFLAGS) $< -o $@

##############################################################################
# Special rule for crt0.s
##############################################################################
crt0.o: crt0.s ninjaturdle.chr ninjaturdle2.chr sound/TestMusic3.s sound/SoundFx.s
	$(CA65) $(AFLAGS) $< -o $@

##############################################################################
# Link
##############################################################################
ninjaturdle.nes: ninjaturdle.o bank_swap.o $(LEVEL_OBJS) crt0.o
	$(LD65) -C $(CFG) -o $@ -Ln ninjaturdle.lbl ninjaturdle.o $(LEVEL_OBJS) crt0.o $(RUNTIME)

##############################################################################
# Cleanup
##############################################################################
clean:
	rm -f $(OBJS) ninjaturdle.s bank_swap.s map/w1l1.s map/w1l2.s map/w1l3.s *.lbl ninjaturdle.nes