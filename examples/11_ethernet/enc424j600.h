/**
 * @class P2::ENC424J600
 * @brief Driver for the ENC424J600 ethernet controller
 *
 * General Driver Structure
 * ------------------------
 *
 *     The main loop runs in it's own cog-mode cog. The cog just loops indefinitely, polling
 * The interrupt pin to see if there's data to read, and polling the mailbox for data to write.
 * Because we use the streamer for reading/writing bulk data, the main read/write code needs to
 * live in the main `run()` function, otherwise the code will execute in hub mode (this is due
 * to a current limitation of p2llvm). However, writing individual registers for setup can be
 * bit-banged, so those can (and will) be separate functions that will execute in hub mode.
 *
 * The user of this driver should instantiate it and start it. Access functions should be used
 * to read/write registers and get specific data values from the Special Function Registers (SFRs).
 * To read/write packets to transmission and reception, we will use a streamer to read/write the
 * indirect SRAM access registers.
 *
 * Individual Registers
 * --------------------
 *
 *      Each register is defined with a 15-bit address from 0x7e00 to 0x7fff. We'll treat each
 * as 16-bit address. Each register is a 16 bit value, which we will read as two bytes using 8-bit
 * PSP mode 5.
 *
 * The SRAM buffer
 * ---------------
 *
 *      The SRAM buffer is accessed via the SRAM buffer registers, EGPDATA, ERXDATA, EUDADATA
 * (which access the actual data), and EGPWRPT, ERXWRPT, EUDARDPT, EUDAWRPT, EGPRDPT, ERXRDPT, (which
 * set the pointers corresponding to the registers that access data). There's an RX section at the end
 * the size of which we get to define. It will be set to 12000B (the maximum size of 8 ethernet packet)
 * This leaves the rest of the space for transmit packets. realistically, we don't need a lot of space
 * for Tx, so we could grow the RX area even more. As a result, 0x3120 is the start of the RX area.
 * We won't set a "user defined" area.
 *
 * Writing Individual Registers
 * ----------------------------
 *
 *  1. latch the desired register's low-byte address
 *      1.1 set AD0-AD14 as output.
 *      1.2 set AD0-AD14 to the 15-bit address.
 *      1.3 pulse the AL pin with the required length to latch the address
 *  2. Write the lower byte of the desired value
 *      2.1 set AD0-AD7 to the lower byte of the value
 *      2.2 pulse the WR pin with the required length to write the byte
 *  3. repeat 1 and 2 for the high byte of the address and value
 *
 * Reading Individual Registers
 * ----------------------------
 *
 *  1. latch the desired register's low-byte address
 *  2. read the low-byte of the value
 *      2.1 set RD high
 *      2.2 wait the appropriate time, then read off the the byte from AD0-AD7
 *      2.3 set RD low
 *  3. repeat for the desired value's register high-byte
 *
 * Writing to the SRAM Buffer
 * --------------------------
 *
 * This assumes we are writing to the general purpose area.
 *
 *  1. write the desired SRAM address to EGPWRPT
 *  2. do a streamed write to EGPDATA
 *      2.1 latch EGPDATA
 *      2.2 set up the WR pin as a smart pin to pulse at the desired clock frequency (probably
 *          8-tick period).
 *      2.3 set the streamer to 8-bit RDFAST->pins mode to read from the desired hub address and number
 *          of bytes to write.
 *      2.4 set up an event to trigger when data is done writing
 *      2.5 enable the smart pin and streamer
 *      2.6 wait for the event that signals data is done.
 *  3. signal (TBD how) that writing is done.
 *
 * Reading from the SRAM buffer
 * ----------------------------
 *
 * This assumes we are reading from the RX data area.
 *
 * 1. write the desired SRAM address to ERXRDPT
 * 2. do a streamed read from ERXDATA
 *      2.1 latch ERXDATA
 *      2.2 set up the RD pin as a smart pin (same as when writing)
 *      2.3 set the stream erto 8-bit pins->RDFAST mode to read from AD0-7 into a desired hub location a desired
 *          number of bytes
 *      2.4 set an event to trigger when data is done reading
 *      2.5 enabled the smart pin and streamer
 *      2.6 wait for the event
 * 3. signal (TBD how) that writing is done.
 *
 */

#ifndef _ENC424J600_H
#define _ENC424J600_H

#include <propeller.h>
#include <stdint.h>

namespace P2 {
    class ENC424J600 {

        // register definitions. Only defines the low byte of the register
        constexpr static uint16_t ETXST =       0x7e00;
        constexpr static uint16_t ETXLEN =      0x7e02;
        constexpr static uint16_t ERXST =       0x7e04;
        constexpr static uint16_t ERXTAIL =     0x7e06;
        constexpr static uint16_t ERXHEAD =     0x7e08;
        constexpr static uint16_t EDMAST =      0x7e0a;
        constexpr static uint16_t EDMALEN =     0x7e0c;
        constexpr static uint16_t EDMADST =     0x7e0e;
        constexpr static uint16_t EDMACS =      0x7e10;
        constexpr static uint16_t ETXSTAT =     0x7e12;
        constexpr static uint16_t ETXWIRE =     0x7e14;
        constexpr static uint16_t EUDAST =      0x7e16;
        constexpr static uint16_t EUDAND =      0x7e18;
        constexpr static uint16_t ESTAT =       0x7e1a;
        constexpr static uint16_t EIR =         0x7e1c;
        constexpr static uint16_t ECON1 =       0x7e1e;
        constexpr static uint16_t EHT1 =        0x7e20;
        constexpr static uint16_t EHT2 =        0x7e22;
        constexpr static uint16_t EHT3 =        0x7e24;
        constexpr static uint16_t EHT4 =        0x7e26;
        constexpr static uint16_t EPMM1 =       0x7e28;
        constexpr static uint16_t EPMM2 =       0x7e2a;
        constexpr static uint16_t EPMM3 =       0x7e2c;
        constexpr static uint16_t EPMM4 =       0x7e2e;
        constexpr static uint16_t EPMCS =       0x7e30;
        constexpr static uint16_t EPMO =        0x7e32;
        constexpr static uint16_t ERXFCON =     0x7e34;
        constexpr static uint16_t MACON1 =      0x7e40;
        constexpr static uint16_t MACON2 =      0x7e42;
        constexpr static uint16_t MABBIPG =     0x7e44;
        constexpr static uint16_t MAIPG =       0x7e46;
        constexpr static uint16_t MACLCON =     0x7e48;
        constexpr static uint16_t MAMXFL =      0x7e4a;
        constexpr static uint16_t MICMD =       0x7e52;
        constexpr static uint16_t MIREGADR =    0x7e54;
        constexpr static uint16_t MAADR3 =      0x7e60;
        constexpr static uint16_t MAADR2 =      0x7e62;
        constexpr static uint16_t MAADR1 =      0x7e64;
        constexpr static uint16_t MIWR =        0x7e66;
        constexpr static uint16_t MIRD =        0x7e68;
        constexpr static uint16_t MISTAT =      0x7e6a;
        constexpr static uint16_t EPAUS =       0x7e6c;
        constexpr static uint16_t ECON2 =       0x7e6e;
        constexpr static uint16_t ERXWM =       0x7e70;
        constexpr static uint16_t EIE =         0x7e72;
        constexpr static uint16_t EIDLED =      0x7e74;
        constexpr static uint16_t EGPDATA =     0x7e80;
        constexpr static uint16_t ERXDATA =     0x7e82;
        constexpr static uint16_t EUDADAT =     0x7e84;
        constexpr static uint16_t EGPRDPT =     0x7e86;
        constexpr static uint16_t EGPWRPT =     0x7e88;
        constexpr static uint16_t ERXRDPT =     0x7e8a;
        constexpr static uint16_t ERXWRPT =     0x7e8c;
        constexpr static uint16_t EUDARDPT =    0x7e8e;
        constexpr static uint16_t EUDAWRPT =    0x7e90;

        constexpr static uint32_t STACK_SIZE = 32;
        constexpr static uint32_t RXPACKET_SIZE = 1500;
        constexpr static uint32_t RXBUF_SIZE = 8;

        // at this rate, we can clock out a full size packet (1500 bytes) in about 80 us (12KHz), faster than our 100Mbps link!
        constexpr static uint32_t PSP_CLKFREQ_DIV = 16;
        constexpr static uint32_t PSP_CLKFREQ_NCO = 0x80000000/PSP_CLKFREQ_DIV;

        /**
         * commands for our access functions to talk to the driver
         */
        enum {
            CMD_NONE,       /// do nothing
            CMD_WR_REG,     /// write a value to a register
            CMD_RD_REG,     /// read a value from a regsiter
            CMD_TX          /// transmit a packet
                            // we don't need an RX command because rx_buf will always be populated with anything available
        };

        volatile uint32_t stack[STACK_SIZE];
        volatile uint8_t rx_buf[RXBUF_SIZE][RXPACKET_SIZE];

        uint8_t p_ad0;
        uint8_t p_al;
        uint8_t p_rd;
        uint8_t p_wr;
        uint8_t p_int;
        uint8_t p_clk;

        volatile struct driver_mb_t {
            uint8_t cmd;        /// command to execute

            // these are all sepcific to the command
            uint16_t reg;       /// register to operate on
            uint16_t val;       /// value to write (or store the read value)

            uint8_t *tx_dat;    /// data to transmit
            uint16_t len;       /// length of data

            atomic_t rx_lock;
        } driver_mb;

        /**
         * main cog function. runs in cog mode.
         */
        __attribute__ ((cogmain)) static void run(void *);

        /**
         * set AD0-AD7 to v
         *
         * @param v: value to set to
         *
         */
        void set_8bit_out(uint8_t v);

        /**
         * set AD0-AD14 to v
         *
         * @param v: value to set to
         *
         */
        void set_15bit_out(uint16_t v);

        /**
         * latch the address to the ethernet controller
         *
         * @param addr: address to latch
         *
         */
        void latch_register(uint16_t addr);

        /**
         * write a 16 bit value to a 16 bit register
         *
         * @param addr: address to write to
         * @param v: value to write to addr
         */
        void write_register(uint16_t addr, uint16_t v);

        /**
         * reset the chip with a write/reset/check procedure to make sure chip is functioning
         *
         * @returns if reset was succesful
         */
        bool reset_hw();

    public:

        /**
         * configuration is valid and can be used
         */
        bool valid;

        /**
         * @param ad0: AD0 pin. must be a multiple of 8, and following 14 pins must be AD1-14
         * @param al: AL pin
         * @param rd: RD pin
         * @param wr: WR pin
         * @param in: INT pin
         * @param clk: 25MHz clock pin (optional)
         */
        ENC424J600(uint8_t ad0, uint8_t al, uint8_t rd, uint8_t wr, uint8_t in, uint8_t clk = 255);

        /**
         * start the driver
         */
        void start();

        /*
         * reset the driver
         */
        void reset();

        /**
         * write a word to a register
         *
         * @param addr: address to write to
         * @param v: value to write
         *
         */
        void write(uint16_t addr, uint16_t v) {
            driver_mb.reg = addr;
            driver_mb.val = v;
            driver_mb.cmd = ENC424J600::CMD_WR_REG;
        }

        /**
         * transmit a packet
         *
         * @param dat: pointer to packet data
         * @param l: length of the data (to be written out to the controller)
         */
        void tx(uint8_t *dat, uint32_t l) {
            driver_mb.tx_dat = dat;
            driver_mb.len = l;
            driver_mb.cmd = ENC424J600::CMD_TX;
        }
    };
}

#endif
