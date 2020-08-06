ENTRY(_entry)

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

    _end = . ;
}