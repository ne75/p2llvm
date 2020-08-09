LLVM_BASE = /opt/p2llvm#~/Github/llvm-propeller2/build
CC = $(LLVM_BASE)/bin/clang
LD = $(LLVM_BASE)/bin/ld.lld
LOAD = loadp2
LIBP2 = ../libp2
LIBC = ../libc

TARGET = test15
OBJDIR = build

CFLAGS = -Os -ffunction-sections -fdata-sections -fno-jump-tables --target=p2 -I$(LIBP2)/include -I$(LIBC)/include
LFLAGS = -Tp2.ld -L$(LIBP2) -L$(LIBP2)/build -lp2 -L$(LIBC)/build -lc
LOADFLAGS = -v -ZERO -b 230400 -t

.PHONY: all setup clean load

all: clean setup $(OBJDIR)/$(TARGET).elf load

setup:
	$(shell pkill loadp2)
	mkdir -p $(OBJDIR)

$(OBJDIR)/$(TARGET).elf: $(OBJDIR)/$(TARGET).o
		$(LD) -n -N $(LFLAGS) -o $@ $^ #-mllvm -debug

$(OBJDIR)/$(TARGET).o: $(TARGET).cpp
		$(CC) $(CFLAGS) -o $@ -c $< #-mllvm -debug #--debug-only=p2-isel-lowering

load:
	$(LOAD) $(LOADFLAGS) $(OBJDIR)/$(TARGET).elf

clean:
	rm -rf ./$(OBJDIR)