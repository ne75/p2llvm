LLVM_BASE = /opt/p2llvm
CC = $(LLVM_BASE)/bin/clang
LD = $(LLVM_BASE)/bin/ld.lld
LOAD = $(LLVM_BASE)/bin/loadp2
LIBP2 = ../libp2
LIBC = ../libc

TARGET = test20
OBJDIR = build

CFLAGS = -ffunction-sections -fdata-sections --target=p2 -Dprintf=__simple_printf
LFLAGS = --target=p2 #-Wl,--gc-sections -Wl,--print-gc-sections
LOADFLAGS = -v -ZERO -FIFO 1024 -b 230400 -t

.PHONY: all setup clean load

all: clean setup $(OBJDIR)/$(TARGET).elf load

setup:
	$(shell pkill loadp2)
	mkdir -p $(OBJDIR)

$(OBJDIR)/$(TARGET).elf: $(OBJDIR)/$(TARGET).o
		$(CC) $(LFLAGS) -o $@ $^

$(OBJDIR)/$(TARGET).o: $(TARGET).cpp
		$(CC) $(CFLAGS) -o $@ -c $<
		# $(CC) $(CFLAGS) -mllvm -debug -o $@ -c $<

load:
	$(LOAD) $(LOADFLAGS) $(OBJDIR)/$(TARGET).elf

clean:
	rm -rf ./$(OBJDIR)
