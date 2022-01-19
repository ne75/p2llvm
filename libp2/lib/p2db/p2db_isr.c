/**
 * This is the basic isr that will load our debugger app. It must be exactly 16 instructions (to place appropriate amount of space between them)
 * 8 debug ISRs are all the same, but difference addresses based on the cog, so create this simeple macro to make it 8 stimes
 * 
 * It's preceeded by 16 nops for padding. this is where the ROM ISR will save 0x0..0xf
 * 
 * @param cog: cog number (0..7)
 * @param save_addr: upper 11 bits of where to spill cog ram (passed to augs). lower 9 bits will be 0
 */

#define STR(s) #s


#define BASE_ISR(cog, save_addr) __attribute__ ((section (".debug_isr"), noreturn, cogtext)) void __dbg_isr##cog() {  \
    asm(        \
        "nop\n" \
        "nop\n" \
        "nop\n" \
        "nop\n" \
        "nop\n" \
        "nop\n" \
        "nop\n" \
        "nop\n" \
        "nop\n" \
        "nop\n" \
        "nop\n" \
        "nop\n" \
        "nop\n" \
        "nop\n" \
        "nop\n" \
        "nop\n" \
    );          \
    asm(                                                                            \
        "setq    #0xff\n"       /* save ram 0xf0..0x1ef*/                           \
        "augs    #" STR(save_addr) "\n"                                             \
        "wrlong  $0xf0, #0\n"                                                       \
        "setq    #0xff\n"       /* load program into 0xf0..0x1ef from 0xfc800*/     \
        "augs    #0x7e4\n"                                                          \
        "rdlong  $0xf0, #0x0\n"                                                     \
        "call    #\\0xf0\n"      /* jump to debug image */                           \
        "setq    #0xff\n"       /* exit routine. restore ram from 0xfc000*/         \
        "augs    #" STR(save_addr) "\n"                                             \
        "rdlong  $0xf0, #0\n"                                                       \
        "jmp     #\\0x1fd\n"                                                         \
    );                                                                              \
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winvalid-noreturn"
BASE_ISR(7, 0x7e0);     // saves 0x100 longs to 0xfc000
BASE_ISR(6, 0x7e2);     // saves 0x100 longs to 0xfc400
BASE_ISR(5, 0x7e4);     // saves 0x100 longs to 0xfc800
BASE_ISR(4, 0x7e6);     // saves 0x100 longs to 0xfcc00
BASE_ISR(3, 0x7e8);     // saves 0x100 longs to 0xfd000
BASE_ISR(2, 0x7ea);     // saves 0x100 longs to 0xfd400
BASE_ISR(1, 0x7ec);     // saves 0x100 longs to 0xfd800
BASE_ISR(0, 0x7ee);     // saves 0x100 longs to 0xfdc00
#pragma clang diagnostic pop