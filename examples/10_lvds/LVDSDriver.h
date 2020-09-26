#ifndef _LVDSDRIVER_H
#define _LVDSDRIVER_H

#include <stdint.h>

// edit these parameters as necessary
#define LVDS_COLS               1024
#define LVDS_ROWS               600
#define LVDS_FB_WORD_SIZE       8
#define LVDS_N_COLOR_BITS       4

#define LVDS_TEXT_COLS          (LVDS_COLS/16)
#define LVDS_TEXT_ROWS          (LVDS_ROWS/32) // there will be a half tile gap at the bottom

#define LVDS_THA                LVDS_COLS
#define LVDS_THF                160

#define LVDS_TVA                LVDS_ROWS
#define LVDS_TVF                3

#define LVDS_STACK_SIZE         32

class LVDSDriver {
    volatile uint32_t stack[LVDS_STACK_SIZE];

    // first pin of the 4 LVDS pins
    uint8_t pin;

    // color lookup table
    uint32_t colors[1<<LVDS_N_COLOR_BITS];

public:
    // framebuffer. each uint8_t in the frame buffer describes 2 4-bit pixels
    // in order of left to right, top to bottom. lower 4 bits draw first.
    uint8_t fb[LVDS_ROWS*LVDS_COLS/(LVDS_FB_WORD_SIZE/LVDS_N_COLOR_BITS)];

    __attribute__ ((cogmain)) static void run(void *);

    LVDSDriver(uint32_t *colors, uint8_t pin);

    void start();

    /*
     * place a char c at x/y with bg background color and fg foreground color
     */
    void _char(char c, int32_t x, int32_t y, uint8_t bg, uint8_t fg);

    /*
     * place a string s at x/y with bg background color and fg foreground color
     */
    void str(const char *s, int32_t x, int32_t y, uint8_t bg, uint8_t fg);

    /*
     * clear the screen with the specified color
     */
    void clear(uint8_t col);

};

#endif
