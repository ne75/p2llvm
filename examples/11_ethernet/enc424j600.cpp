#include "enc424j600.h"

#include <propeller.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace P2;

ENC424J600::ENC424J600(uint8_t ad0, uint8_t al, uint8_t rd, uint8_t wr, uint8_t in, uint8_t clk) {
    p_ad0 = ad0;
    p_al = al;
    p_rd = rd;
    p_wr = wr;
    p_int = in;
    p_clk = clk;

    driver_mb.rx_lock = _locknew();
}

void ENC424J600::start() {
    stack[0] = (unsigned)this;
    _coginit(0x10, run, (void*)stack);
}

void ENC424J600::set_8bit_out(uint8_t v) {
    if (p_ad0 < 32) {
        OUTA |= (v & 0xff) << p_ad0;
    } else {
        OUTB |= (v & 0xff) << p_ad0;
    }
}

void ENC424J600::set_15bit_out(uint16_t v) {
    if (p_ad0 < 32) {
        OUTA |= (v & 0x7fff) << p_ad0;
    } else {
        OUTB |= (v & 0x7fff) << p_ad0;
    }
}

void ENC424J600::latch_register(uint16_t addr) {
    // 1.1 set AD0-AD14 as output.
    // 1.2 set AD0-AD14 to the 15-bit address.
    // 1.3 pulse the AL pin with the required length to latch the address
    dirh(p_ad0 | (15 << 6)); // set 15 pins, starting at ad0, to outputs
    outl(p_ad0 | (15 << 6)); // set them all low
    set_15bit_out(addr);

    // min time is 1ns, so no need to wait
    outh(p_al);
    outl(p_al);
}

void ENC424J600::write_register(uint16_t addr, uint16_t v) {
    latch_register(addr);
    set_8bit_out(v & 0xff);
    outh(p_wr);
    outl(p_wr);

    latch_register(addr+1);
    set_8bit_out(v >> 8);
    outh(p_wr);
    outl(p_wr);
}

void ENC424J600::run(void *p) {
    ENC424J600 *eth = (ENC424J600*)p;

    if (eth->p_clk != 255) {
        // we need to generate a clock, set it up here.
        // F = 25 MHz, Fclk = 300 MHz. Let N = 31
        // X = 2^(N-32) * Fclk/F = 6
        // Y = 2^N. Require this to avoid jitter in the output
        const uint32_t t_base = 6; // base clock unit
        wrpin(SP_NCO_FREQ_MODE, eth->p_clk);
        wxpin(t_base, eth->p_clk);
        wypin(1<<31, eth->p_clk);
        dirh(eth->p_clk);
    }

    // pre-compute the read/write streamer modes  // mode, submode, dac, en_pins, pins
    const uint32_t stream_rd = GET_STREAMER_CONFIG_8(0xe, 0xe, 0, 1, eth->p_ad0/8);
    const uint32_t stream_wr = GET_STREAMER_CONFIG_8(0xa, 0xe, 0, 1, eth->p_ad0/8);
    setxfrq(PSP_CLKFREQ_NCO);               // set up the streamer frequency

    // set up rd, wr, and al pins as outputs
    dirh(eth->p_rd);
    dirh(eth->p_wr);
    dirh(eth->p_al);

    while(1) {
        switch (eth->driver_mb.cmd) {
            case ENC424J600::CMD_WR_REG: {
                printf("write reg\n");
                eth->write_register(eth->driver_mb.reg, eth->driver_mb.val); // test values
                eth->driver_mb.cmd = ENC424J600::CMD_NONE;
                break;
            }

            case ENC424J600::CMD_RD_REG: {

                eth->driver_mb.cmd = ENC424J600::CMD_NONE;
                break;
            }

            case ENC424J600::CMD_TX: {

                eth->write_register(EGPWRPT, 0); // always tx out of SRAM address 0 (no reason to do anything else, yet).
                                                 //We likely don't need to set this either, just let is keep wrapping in memory
                eth->latch_register(EGPDATA);
                uint32_t n = eth->driver_mb.len;
                uint8_t *d = eth->driver_mb.tx_dat;

                // set up the HUB FIFO for reading
                rdfast(0, d);

                dirl(eth->p_wr);                        // reset the smart pin
                // set up the write clock
                wrpin(SP_TRANS_OUT_MODE, eth->p_wr);    // set to transition mode
                wxpin(PSP_CLKFREQ_DIV/2, eth->p_wr);    // set the clock frequency. should be twice the streamer frequency
                dirh(eth->p_wr);                        // release the reset
                uint8_t p = eth->p_wr;                  // pre-compute values. pin to clock
                uint32_t n_edge = (n<<1);               // number of edges = 2x number of bytes
                uint32_t cmd = stream_wr + n;           // streamer command

                /*** CRITICALLY TIMED CODE. DO NOT CHANGE WITHOUT THOROUGH TESTING ***/

                // run this as an asm block to make sure no instructions get inserted between the two
                // wypin starts the wr clock, xinit starts the data stream
                asm("xinit #0, #0\n"        // reset the streamer
                    "waitx #0\n"            // wait 2 clocks ticks (nop doesn't work yet for some reason)
                    "wypin %2, %3\n"        // start the wr clock
                    "xinit %0, %1"          // start the data streamer
                    : : "r"(cmd), "r"(0), "r"(n_edge), "r"(p));

                // wait for it to finish. TODO testing IN doesn't seem to work, so just wait cause we know how long it will take
                waitx(n*PSP_CLKFREQ_DIV);
                wrpin(0, eth->p_wr); // reset the pin to normal mode

                /*** END OF CRITICALLY TIMED CODE ***/

                eth->driver_mb.cmd = ENC424J600::CMD_NONE;
                break;
            }

            default:
                break;
        }
    }
}