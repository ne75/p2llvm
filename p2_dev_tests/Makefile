LLVM_BASE = /opt/p2llvm#~/Github/llvm-propeller2/build
CC = $(LLVM_BASE)/bin/clang
LD = $(LLVM_BASE)/bin/ld.lld
LOAD = loadp2
LIBP2 = ../libp2
LIBC = ../libc

TARGET = simplespi
OBJDIR = build

CFLAGS = -Os -ffunction-sections -fdata-sections -fno-jump-tables --target=p2 -I$(LIBP2) -I$(LIBC)/include -D__propeller2__ -DLLVM
LFLAGS = -Tp2.ld -L$(LIBP2) -L$(LIBC)/build -lc
LOADFLAGS = -v -ZERO -b 230400 -t

.PHONY: all setup clean load

all: clean setup $(OBJDIR)/$(TARGET).elf load

setup:
	$(shell pkill loadp2)
	mkdir -p $(OBJDIR)

$(OBJDIR)/$(TARGET).elf: $(OBJDIR)/$(TARGET).o $(OBJDIR)/propeller2.o $(OBJDIR)/crt0.o $(OBJDIR)/printf.o
		$(LD) -n -N $(LFLAGS) -o $@ $^ #-mllvm -debug

$(OBJDIR)/$(TARGET).o: $(TARGET).cpp $(LIBP2)/propeller2.h
		$(CC) $(CFLAGS) -o $@ -c $< #-mllvm --debug-only=p2-asm-backend,mccodeemitter

$(OBJDIR)/propeller2.o: $(LIBP2)/propeller2.c $(LIBP2)/propeller2.h
		$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR)/printf.o: $(LIBP2)/printf.c $(LIBP2)/printf.h
		$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR)/crt0.o: $(LIBP2)/crt0.c
		$(CC) $(CFLAGS) -o $@ -c $< #-mllvm -debug --debug-only=p2-frame-lower

load:
	$(LOAD) $(LOADFLAGS) $(OBJDIR)/$(TARGET).elf

clean:
	rm -rf ./$(OBJDIR)