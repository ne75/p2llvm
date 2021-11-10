LLVM = /opt/p2llvm
# LLVM = ../llvm-project/build_debug
CC = $(LLVM)/bin/clang
LD = $(LLVM)/bin/ld.lld
AR = $(LLVM)/bin/llvm-ar
OBJCOPY = $(LLVM)/bin/llvm-objcopy
LOAD = /opt/p2llvm/bin/loadp2

OBJDIR = build

CFLAGS = -Os -fdata-sections -fno-jump-tables --target=p2 -Dprintf=__simple_printf #-mllvm -debug
LFLAGS = --target=p2 -Wl,--verbose
LOADFLAGS = -v -ZERO -l 3000000 -b 3000000 -FIFO 1024

.PHONY: all setup clean

OBJS = p2db_isr.o p2db.o p2db_isr.bin p2db.bin

all: clean setup $(OBJDIR)/test.bin

setup:
	mkdir -p $(OBJDIR)

$(OBJDIR)/test.bin: $(OBJDIR)/test.elf
		$(OBJCOPY) -O binary $< $@

$(OBJDIR)/test.elf: $(OBJDIR)/test.o
		$(CC) $(LFLAGS) -o $@ $^

$(OBJDIR)/%.o: %.c
		$(CC) $(CFLAGS) -o $@ -c $<

load:
	$(LOAD) $(LOADFLAGS) $(OBJDIR)/test.bin
	
# $(LOAD) $(LOADFLAGS) @0=$(OBJDIR)/test.bin,@fc800=$(OBJDIR)/p2db.bin,@ffc00=$(OBJDIR)/p2db_isr.bin

clean:
	rm -rf ./$(OBJDIR)
