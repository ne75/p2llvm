LLVM_BASE = /opt/p2llvm
CC = $(LLVM_BASE)/bin/clang
LD = $(LLVM_BASE)/bin/ld.lld
LOAD = loadp2
LIBP2 = ../libp2
LIBC = ../libc

TARGET = test16
OBJDIR = build

CFLAGS = -Os -ffunction-sections -fdata-sections -fno-jump-tables --target=p2 -Dprintf=__simple_printf
LFLAGS = --target=p2 -Wl,--gc-sections -Wl,--print-gc-sections
LOADFLAGS = -v -ZERO -b 230400 -t

.PHONY: all setup clean load

all: clean setup $(OBJDIR)/$(TARGET).elf load

setup:
	$(shell pkill loadp2)
	mkdir -p $(OBJDIR)

$(OBJDIR)/$(TARGET).elf: $(OBJDIR)/$(TARGET).o
		$(CC) $(LFLAGS) -o $@ $^ #-Wl,--threads=1

$(OBJDIR)/$(TARGET).o: $(TARGET).cpp
		$(CC) $(CFLAGS) -o $@ -c $< -mllvm -debug-only=p2-asm-backend #p2-mc-ilower,p2-asm-printer,mccodeemitter,p2-asm-parser #p2-asm-parser,mccodeemitter

load:
	$(LOAD) $(LOADFLAGS) $(OBJDIR)/$(TARGET).elf

clean:
	rm -rf ./$(OBJDIR)
