LLVM_BASE = /opt/p2llvm
CC = $(LLVM_BASE)/bin/clang
LD = $(LLVM_BASE)/bin/ld.lld
FASTSPIN = $(LLVM_BASE)/bin/fastspin
LOAD = $(LLVM_BASE)/bin/loadp2

TARGET = lvds
OBJDIR = ../build

CFLAGS = -Os -ffunction-sections -fdata-sections -fno-jump-tables --target=p2 -Dprintf=__simple_printf -DP2_TARGET_MHZ=360
LFLAGS = --target=p2 #-Wl,--gc-sections -Wl,--print-gc-sections
LOADFLAGS = -v -ZERO -b 230400 -t

.PHONY: all setup clean load

all: clean setup $(OBJDIR)/$(TARGET).elf load

setup:
	$(shell pkill loadp2)
	mkdir -p $(OBJDIR)

$(OBJDIR)/$(TARGET).elf: $(OBJDIR)/$(TARGET).o $(OBJDIR)/LVDSDriver.o
		$(CC) $(LFLAGS) -o $@ $^

$(OBJDIR)/$(TARGET).o: $(TARGET).cpp
		$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR)/LVDSDriver.o: LVDSDriver.cpp LVDSDriver.h
		$(CC) $(CFLAGS) -o $@ -c $< #-mllvm --debug-only=p2-isel,isel

load:
	$(LOAD) $(LOADFLAGS) $(OBJDIR)/$(TARGET).elf

clean:
	rm -rf ./$(OBJDIR)
