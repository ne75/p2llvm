ENTRY(asm_address_add)
SECTIONS {
    .text 0x1000 : { *(.text) }
    .lut_image 0x2000 : {
        __p2_lut_load_start = .;
        *(lut)
        __p2_lut_load_end = .;
    }
    .data 0x3000 : { *(.data) }
}
