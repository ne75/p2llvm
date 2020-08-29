ENTRY(__entry)

/*MEMORY {
    ram         (rwx) : ORIGIN = 0x00000, LENGTH = 0x7c000
    debug_rom   (rx)  : ORIGIN = 0x7c000, LENGTH = 0x04000
}*/

STACK_SIZE = 0x2000;
HEAP_SIZE = 0x2000;

SECTIONS {
    . = 0x00000;
    .text : {
        KEEP(*(.text.__entry));
    }

    . = 0x00100;
    .cog : {
        KEEP(__start())
        KEEP(*(cog))
    }

    . = 0x00900;
    .text : {
        KEEP(*(.text.main))
        *(.text*);
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
        . = . + HEAP_SIZE;
    }

    /* end of usable memory is 0x7C000 */
    . = 0x7c000 - STACK_SIZE;
    .stack (NOLOAD) : {
        . = ALIGN(4);
        . = . + STACK_SIZE;
    }

    _end = . ;
}