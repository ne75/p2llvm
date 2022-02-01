LLVM_BASE = /opt/p2llvm
CC = $(LLVM_BASE)/bin/clang
LD = $(LLVM_BASE)/bin/ld.lld
LOAD = $(LLVM_BASE)/bin/loadp2

TARGET = blink
OBJDIR = ../build

CFLAGS = -Os --target=p2 -Dprintf=__simple_printf -DP2_TARGET_MHZ=200
LFLAGS = --target=p2 -lp2db
LOADFLAGS = -v -ZERO -b 3000000 -FIFO 1024 -t 

.PHONY: all setup clean load

all: clean setup $(OBJDIR)/$(TARGET).elf load

setup:
	$(shell pkill loadp2)
	mkdir -p $(OBJDIR)

# $(OBJDIR)/$(TARGET).elf: $(OBJDIR)/$(TARGET).o
# 		$(CC) $(LFLAGS) -o $@ $^

$(OBJDIR)/$(TARGET).elf: $(TARGET).cpp
		$(CC) $(CFLAGS) $(LFLAGS) -o $@ $<

load:
	$(LOAD) $(LOADFLAGS) $(OBJDIR)/$(TARGET).elf

clean:
	rm -rf ./$(OBJDIR)
