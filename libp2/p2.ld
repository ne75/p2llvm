ENTRY(_entry)

STACK_SIZE = 0x2000;

SECTIONS {
    . = 0x00000;
    .text : {
        *(.text._entry);
    }

    . = 0x00100;
    .cog : {
        _start
        *(COG);
    }

    . = 0x00400;
    .text : {
        *(.text.main);
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

    /* end of usable memory is 0xFC000 */
    . = 0xFA000;
    .stack (NOLOAD) : {
        . = ALIGN(4);
        . = . + STACK_SIZE;
        . = ALIGN(4);
    }

    _end = . ;
}