ENTRY(__entry)

/*MEMORY {
    ram         (rwx) : ORIGIN = 0x00000, LENGTH = 0x7c000
    debug_rom   (rx)  : ORIGIN = 0x7c000, LENGTH = 0x04000
}*/

STACK_SIZE = 0xc000;
HEAP_SIZE = 0xc000;

SECTIONS {
    . = 0x00000;
    .text : {
        KEEP(*(.text.__entry));
    }

    . = 0x40; /* reserve first 64 bytes for global information storage (clkmode, clkfreq, etc) that need to be
                 available outside of the link step and compatability with other toolchains */
    .cog : {
        KEEP(__start())
        KEEP(*(cog))
    }

    /* If this address gets changed, we need to update lld's relocation function since it's hard coded there */
    . = 0x00200;
    .lut : {
        KEEP(*(lut))
    }

    . = 0x00a00;
    .text : {
        KEEP(*(.text.main))
        *(.text*);
    }

    .init_array : {
        _init_array_start = . ;
        KEEP(*(.init_array))
        _init_array_end = . ;
    }

    .fini_array : {
        _fini_array_start = .;
        KEEP(*(.fini_array))
        _fini_array_end = .;
    }

    .rodata : {
        *(.rodata*);
    }

    .data : {
        *(.data*);
    }

    .bss : {
        *(.bss*);
    }

    .heap (NOLOAD) : {
        . = ALIGN(4);
        __heap_start = .;
        . = . + HEAP_SIZE - 4;
    }

    /* end of usable memory is 0x7C000 */
    . = 0x7c000 - STACK_SIZE;
    .stack (NOLOAD) : {
        . = ALIGN(4);
        __stack = .;
        . = . + STACK_SIZE - 4;
    }
    
    .debug : {
        . = 0x7c800;
        KEEP(*(.debug));
        . = 0x7fc00;
        KEEP(*(.debug_isr));
    }

    _end = . ;
}