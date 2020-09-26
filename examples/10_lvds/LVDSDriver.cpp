#include "LVDSDriver.h"

#include <propeller.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Menlo_font.h"

#define xstr(s) str_mac(s)
#define str_mac(s) #s

#define PIX_FREQ        0x20000000
#define PIX_CLK_DAT     0x1100011
#define DE_BIT          8 // = 1 << 3
#define COLOR_BIT_MASK  15 // = ((1<<LVDS_N_COLOR_BITS)-1)

LVDSDriver::LVDSDriver(uint32_t *c, uint8_t p) {
    pin = 8;
    memcpy(colors, c, 4*(1<<LVDS_N_COLOR_BITS));
}

void LVDSDriver::start() {
    stack[0] = (unsigned)this;
    _coginit(0x10, run, (void*)stack);
}

void LVDSDriver::_char(char c, int32_t x, int32_t y, uint8_t bg, uint8_t fg) {
    c = c - 0x20; // don't have first 0x20 characters in the ascii table, they are all blanks

    for (int i = 0; i < 32; i++) {
        uint16_t c_word = Menlo_font[c][i];

        for (int j = 0; j < 16/(LVDS_FB_WORD_SIZE/LVDS_N_COLOR_BITS); j++) { // 16/(LVDS_FB_WORD_SIZE/LVDS_N_COLOR_BITS) bytes per character line
            uint8_t pix_group = 0; // can store (LVDS_FB_WORD_SIZE/LVDS_N_COLOR_BITS) = 2 pixels

            int k = (LVDS_FB_WORD_SIZE/LVDS_N_COLOR_BITS);
            while (k-- > 0) {
                pix_group >>= LVDS_N_COLOR_BITS;
                pix_group |= (c_word & 1) ? fg<<(LVDS_FB_WORD_SIZE-LVDS_N_COLOR_BITS) : bg<<(LVDS_FB_WORD_SIZE-LVDS_N_COLOR_BITS);
                c_word >>= 1;
            }

            uint32_t idx = j + i*(LVDS_COLS/(LVDS_FB_WORD_SIZE/LVDS_N_COLOR_BITS));
            // offset by (x, y) tile coordinates.
            idx += x*16/(LVDS_FB_WORD_SIZE/LVDS_N_COLOR_BITS);
            idx += y*32*(LVDS_COLS/(LVDS_FB_WORD_SIZE/LVDS_N_COLOR_BITS));

            fb[idx] = pix_group;
        }
    }
}

void LVDSDriver::str(const char *s, int32_t x, int32_t y, uint8_t bg, uint8_t fg) {
    while(*s) {
        _char(*s++, x++, y, bg, fg);
    }
}

void LVDSDriver::clear(uint8_t col) {
    uint8_t clear_color = 0;
    for (int i = 0; i < LVDS_FB_WORD_SIZE/LVDS_N_COLOR_BITS; i++) {
        clear_color <<= LVDS_N_COLOR_BITS;
        clear_color |= col;
    }

    for (int i = 0; i < LVDS_ROWS*LVDS_COLS/(LVDS_FB_WORD_SIZE/LVDS_N_COLOR_BITS); i++) {
        fb[i] = clear_color; // clear the frame buffer
    }
}

void LVDSDriver::run(void *p) {
    LVDSDriver *lvds = (LVDSDriver*)p;
    uint8_t *fb_ptr = lvds->fb;

    // setup the streamer frequency and compute the streamer setting
    setxfrq(PIX_FREQ);

    // set streamer_set to 0110 0000 1 [pin] 0
    uint32_t streamer_set = 0x6;
    streamer_set <<= 5;
    streamer_set += 1;
    streamer_set <<= 6;
    streamer_set += lvds->pin;
    streamer_set <<= 17;
    streamer_set += 7;

    // set the LVDS pins as outputs
    dirh(lvds->pin);
    dirh(lvds->pin+1);
    dirh(lvds->pin+2);
    dirh(lvds->pin+3);

    // copy the colors into LUT RAM. can be re-impemented as a block transfer with setq2, but not
    // a time or space critical so simple loop is fine.
    for (int i = 0; i < 1<<LVDS_N_COLOR_BITS; i++) {
        wrlut(lvds->colors[i], i);
    }

    // // setup the streamer
    uint32_t pixel_buf = 0x1100011;
    xinit(streamer_set, pixel_buf);

    rdfast(0, fb_ptr);

    // unfortunately there isn't a good way to name registers, so here's a list of assignements
    // to make the code below easier to read:
    //
    // r16: frame active area line counter
    // r17: de bit overrite control
    // r18: frame vertical blanking line counter
    // r19: offset into each byte of the frame buffer
    // r20: stores clock + de data for every pixel
    // r21: LUT address of the current pixel's color
    // r22: a byte from the pixel buffer. currently, represents 2 pixels
    // r23: (const) clock data for a pixel long
    // r24: pixel buffer storing the data for each pixel to be streamed
    // r25: color data to be placed into the pixel buffer
    // r26: (const) number of active lines
    // r27: (const) number of blank lines
    // r28: (const) number of active pixels/line
    // r29: (const) number of blank pixels/line

    // this is time critical code where every instruction matters
    // so write it all in assembly
    // also, ## immediates are not yet supported. augs must be explictly invoked,
    // or the large immediate should be passed as an "r" input and the compiler will
    // generate an augs/mov combination.
    asm("mov $r23, %0" : : "r"(PIX_CLK_DAT));
    asm("mov $r26, %0" : : "r"(LVDS_TVA));
    asm("mov $r27, %0" : : "r"(LVDS_TVF));
    asm("mov $r28, %0" : : "r"(LVDS_THA));
    asm("mov $r29, %0" : : "r"(LVDS_THF));
    asm(
        // start of frame
        ".Lframe:\n"

        // start of v active area
            "mov $r16, $r26\n"                  // we'll repeat TVA times
            "mov $r17, #" xstr(DE_BIT) "\n"     // allow line to control de

        ".Lvac:\n"
            "call #.Lline\n"
            "djnz $r16, #.Lvac\n"               // write TVA lines

        // start of v blanking
            "mov $r18, $r27\n"
            "mov $r17, #0\n"                    // de override to 0
        ".Lvfp:\n"
            "call #.Lline\n"                    // I made call a relative instruction and calla an absolute
                                                // so call will work within inline assembly
            "djnz $r18, #.Lvfp\n"               // write TVF lines
            "jmp #.Lframe\n"
    );

    /* Draw line sub routine */
    asm(
        // start of h active area
        ".Lline:"
            "mov $r19, #0\n"

            "mov $r20, $r23\n"
            "or $r20, $r17\n"                   // put the value of de into the pixel buffer

            "rep #10, $r28\n"                   // repeat this loop THA times
            "and $r19, #7   wz\n"               // pixel offset into current byte of the frame buffer
            "if_z rfbyte $r22\n"                // r22 = a byte from the pixel buffer
            "mov $r24, $r20\n"                  // setup base pixel data (clock + data enable)
            "add $r19, #" xstr(LVDS_N_COLOR_BITS) "\n"

            "mov $r21, $r22\n"
            "and $r21, #" xstr(COLOR_BIT_MASK) "\n" // LUT address of the color
            "rdlut $r25, $r21\n"

            "or $r24, $r25\n"

            "shr $r22, #" xstr(LVDS_N_COLOR_BITS) "\n"
            "xcont %0, $r24\n"

        // start of h blanking
            "mov $r24, $r23\n"
            "rep #3, $r29\n"                    // repeat this loop THF times
            "test $r18, $r18 wz\n"              // don't have aliased instructions yet, include the register twice

            "if_nz rdfast #0, %1\n"             // setup the RAM fifo at the start of the screen.
                                                // do this here so that it's setup before starting the next frame
            "xcont %0, $r24\n"
            "ret"

        : : "r"(streamer_set), "r"(fb_ptr)
    );

}