ENTRY(__entry)

/*MEMORY {
    ram         (rwx) : ORIGIN = 0x00000, LENGTH = 0x7c000
    debug_rom   (rx)  : ORIGIN = 0x7c000, LENGTH = 0x04000
}*/

STACK_SIZE = 0xc000;
HEAP_SIZE = 0xc000;

SECTIONS {
    PROVIDE(__stack = 0x7c000 - STACK_SIZE);
    
    . = 0x00000;
    .text : {
        KEEP(*(.text.__entry));

        . = 0x40; /* reserve remainder of first 64 bytes for global information storage (clkmode, clkfreq, etc) that need to be
                    available outside of the link step and compatability with other toolchains */
        
        KEEP(__start0())
        KEEP(__start())
        KEEP(*(cog))

        /* Startup loads this image into LUT; calls use these linker bounds. */
        . = 0x00200;
        __p2_lut_load_start = .;
        KEEP(*(lut))
        KEEP(*(.lut .lut.*))
        __p2_lut_load_end = .;
        ASSERT(__p2_lut_load_end - __p2_lut_load_start <= 0x800,
               "P2 LUT image exceeds 512 longs")

        . = 0x00a00;
        KEEP(*(.text.main))
        *(.text*);
    }

    .rodata : {
        *(.rodata*);
    }

    .data : {
        *(.data*);
        PROVIDE(__enable_p2db = .);
        LONG(0) /* by default, debugging is disabled, but if we link p2db, it will define this symbol, overriding it */

        _init_array_start = . ;
        KEEP(*(.init_array))
        _init_array_end = . ;

        _fini_array_start = .;
        KEEP(*(.fini_array))
        _fini_array_end = .;
    }

    .bss : {
        _bss_start = .;
        *(.bss*);
        _bss_end = .;
    }

    .heap (NOLOAD) : {
        . = ALIGN(4);
        __heap_start = .;
        . = . + HEAP_SIZE - 4;
    }

    /* end of usable memory is 0x7C000 */
    .stack 0x7c000 - STACK_SIZE (NOLOAD) : {
        . = ALIGN(4);
        . = . + STACK_SIZE;
    }
    
    .debug : {
        . = 0x7c800;
        KEEP(*(.debug));
        . = 0x7fc00;
        KEEP(*(.debug_isr));
    }

    _end = . ;
}