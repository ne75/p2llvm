LLVM_BASE = /opt/p2llvm
CC = $(LLVM_BASE)/bin/clang
OBJCOPY = $(LLVM_BASE)/bin/llvm-objcopy
LD = $(LLVM_BASE)/bin/ld.lld
LOAD = $(LLVM_BASE)/bin/loadp2
LIBP2 = ../libp2
LIBC = ../libc

TARGET = test20
OBJDIR = build

CFLAGS = -Os -ffunction-sections -fdata-sections --target=p2 -Dprintf=__simple_printf
LFLAGS = --target=p2 #-Wl,--gc-sections -Wl,--print-gc-sections
LOADFLAGS = -v -ZERO -b 230400 -t

.PHONY: all setup clean load

all: clean setup $(OBJDIR)/$(TARGET).bin load

setup:
	$(shell pkill loadp2)
	mkdir -p $(OBJDIR)

$(OBJDIR)/$(TARGET).bin: $(OBJDIR)/$(TARGET).elf
		$(OBJCOPY) -O binary $< $@

$(OBJDIR)/$(TARGET).elf: $(OBJDIR)/$(TARGET).o
		$(CC) $(LFLAGS) -o $@ $^

$(OBJDIR)/$(TARGET).o: $(TARGET).cpp
		$(CC) $(CFLAGS) -o $@ -c $<
		# $(CC) $(CFLAGS) -o $@ -c $<

load:
	$(LOAD) $(LOADFLAGS) $(OBJDIR)/$(TARGET).bin

clean:
	rm -rf ./$(OBJDIR)
