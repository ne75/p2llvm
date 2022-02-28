LLVM_BASE = /opt/p2llvm
CC = $(LLVM_BASE)/bin/clang
LD = $(LLVM_BASE)/bin/ld.lld
LOAD = $(LLVM_BASE)/bin/loadp2

TARGET = objects
OBJDIR = ../build

CFLAGS = -Os --target=p2 -Dprintf=__simple_printf
LFLAGS = --target=p2 -Wl,--gc-sections
LOADFLAGS = -v -ZERO -b 2000000 -t

.PHONY: all setup clean load

all: clean setup $(OBJDIR)/$(TARGET).elf load

setup:
	$(shell pkill loadp2)
	mkdir -p $(OBJDIR)

$(OBJDIR)/$(TARGET).elf: $(OBJDIR)/$(TARGET).o $(OBJDIR)/Blinker.o
		$(CC) $(LFLAGS) -o $@ $^

$(OBJDIR)/$(TARGET).o: $(TARGET).cpp
		$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR)/Blinker.o: Blinker.cpp Blinker.h
		$(CC) $(CFLAGS) -o $@ -c $<

load:
	$(LOAD) $(LOADFLAGS) $(OBJDIR)/$(TARGET).elf

clean:
	rm -rf ./$(OBJDIR)
