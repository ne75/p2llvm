/*
 * A basic test of using libcalls (for math functions). Eventually expand this example to include floats
 * there's no float hardware so it will take a decent amount of work to implement.
 */
#include <propeller.h>
#include <sys/p2es_clock.h>
#include <stdio.h>

#include "LVDSDriver.h"

#define RX_PIN 63
#define TX_PIN 62

#define LVDS_PIN_START 8

uint32_t colors[] = {
    0x0000000,
    0x0000020,
    0x0000400,
    0x0000420,
    0x0008000,
    0x0008020,
    0x0008400,
    0x008C620,
    0x0008420,
    0x2222220,
    0x4444402,
    0x6666622,
    0x8888044,
    0xAAAA264,
    0xCCCC446,
    0xEEEE666
};

LVDSDriver lvds(colors, LVDS_PIN_START);

int main() {
    _clkset(_SETFREQ, _CLOCKFREQ);
    _uart_init(RX_PIN, TX_PIN, 230400); // FIXME: this is SUPPOSED to called by the C standard lib but it isn't working

    printf("==== LVDS Demo ====\n");

    // enable the display (should move this into the driver)
    dirh(12);
    outh(12);

    // clear the framebuffer with black
    lvds.clear(0);

    // start the driver
    lvds.start();

    // print a bunch of characters in a bunch of colors
    auto x = 0;
    auto y = 0;
    auto j = 0;
    auto i = ' ';
    for (int j = 0; j < LVDS_TEXT_ROWS*LVDS_TEXT_COLS; j++) {
        lvds._char(i, x, y, 0, i&15);
        x++;
        i++;
        if (x == LVDS_TEXT_COLS) {
            y++;
            x = 0;
        }
        if (i == '~') i = ' ';
    }

    while(1);
}