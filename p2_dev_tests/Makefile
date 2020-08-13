LLVM = ../build
CC = $(LLVM)/bin/clang
LD = $(LLVM)/bin/ld.lld
LOAD = loadp2

TARGET = fft_bench
#TARGET = test12
OBJDIR = build

CFLAGS = -Os -ffunction-sections -fdata-sections -fno-jump-tables --target=p2 -I../libp2 -I../libc/include -D__propeller2__
LFLAGS = -Tp2.ld -L../libc/build -L../libp2 -lc
LOADFLAGS = -v -ZERO -b 230400 -t

.PHONY: all setup clean load

all: clean setup $(OBJDIR)/$(TARGET).elf load

setup:
	$(shell pkill loadp2)
	mkdir -p $(OBJDIR)

$(OBJDIR)/$(TARGET).elf: $(OBJDIR)/$(TARGET).o $(OBJDIR)/propeller2.o $(OBJDIR)/printf.o $(OBJDIR)/crt0.o
		$(LD) $(LFLAGS) -o $@ $^ #-mllvm -debug

$(OBJDIR)/$(TARGET).o: $(TARGET).cpp ../libp2/propeller2.h
		$(CC) $(CFLAGS) -o $@ -c $< #-mllvm --debug-only=p2-isel-lower,isel-lower,p2-mc-ilower,mc-ilower

$(OBJDIR)/propeller2.o: ../libp2/propeller2.c ../libp2/propeller2.h
		$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR)/printf.o: ../libp2/printf.c ../libp2/printf.h
		$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR)/crt0.o: ../libp2/crt0.c
		$(CC) $(CFLAGS) -o $@ -c $< #-mllvm -debug --debug-only=p2-frame-lower

load:
	$(LOAD) $(LOADFLAGS) $(OBJDIR)/$(TARGET).elf

clean:
	rm -rf ./$(OBJDIR)
