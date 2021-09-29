LLVM_BASE = /opt/p2llvm
CC = $(LLVM_BASE)/bin/clang
LD = $(LLVM_BASE)/bin/ld.lld
FASTSPIN = $(LLVM_BASE)/bin/fastspin
LOAD = $(LLVM_BASE)/bin/loadp2

TARGET = ethernet
OBJDIR = build

LWIP = /Users/nikita/Github/slipstream/lwip
CONTRIBDIR=$(LWIP)/contrib
LWIPARCH=$(CONTRIBDIR)/ports/p2

#Set this to where you have the lwip core module checked out from CVS
#default assumes it's a dir named lwip at the same level as the contrib module
LWIPDIR=$(LWIP)/src

CFLAGS = -Os -ffunction-sections -fdata-sections -fno-jump-tables --target=p2 -Dprintf=__simple_printf -DP2_TARGET_MHZ=300
CFLAGS += -I$(LWIPDIR)/include -I$(LWIPDIR)/include/ipv4 -I$(LWIPARCH) -I$(LWIPARCH)/arch

LFLAGS = --target=p2 #-Wl,--gc-sections -Wl,--print-gc-sections
LFLAGS += -L$(LWIP)/lib

LOADFLAGS = -v -ZERO -b 230400 -t

.PHONY: all setup clean load

all: clean setup $(OBJDIR)/$(TARGET).elf load

setup:
	$(shell pkill loadp2)
	mkdir -p $(OBJDIR)

$(OBJDIR)/$(TARGET).elf: $(OBJDIR)/$(TARGET).o $(OBJDIR)/enc424j600.o
		$(CC) $(LFLAGS) -o $@ $^

$(OBJDIR)/$(TARGET).o: $(TARGET).cpp
		$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR)/enc424j600.o: enc424j600.cpp enc424j600.h
		$(CC) $(CFLAGS) -o $@ -c $< #-mllvm --debug-only=p2-isel,isel

load:
	$(LOAD) $(LOADFLAGS) $(OBJDIR)/$(TARGET).elf

clean:
	rm -rf ./$(OBJDIR)
