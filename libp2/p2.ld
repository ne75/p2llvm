ENTRY(_entry)

STACK_SIZE = 0x2000;

SECTIONS {
    . = 0x00000;
    .text : {
        KEEP(*(.text._entry));
    }

    . = 0x00100;
    .cog : {
        KEEP(_start())
        KEEP(*(COG))
    }

    . = 0x00400;
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

    /* end of usable memory is 0xFC000
     *
     * eventually should make this all dynamic so just changing the stack size will adjust
     * the location to start the stack at and make the startup code use a relocation
     */
    . = 0xFA000;
    .stack (NOLOAD) : {
        . = ALIGN(4);
        . = . + STACK_SIZE;
        . = ALIGN(4);
    }

    _end = . ;
}