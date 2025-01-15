/**
 * @file FdSerial.c
 * Full Duplex Serial adapter module.
 *
 * Copyright (c) 2008, Steve Denson
 * See end of file for terms of use.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/driver.h>
#include <propeller.h>
#include <debug.h>

#include "FdSerial.h"

static FdSerial_t *coglist;

/**
 * the actual driver. this is a reimplementation of the original assembly driver,
 * written in C for P2
 * 
 * If this driver is active on debug pins and debugging is used, it might capture debug data if running in the background
 * so we include a brk to force it to stop on entry when debugging is enabled
 * 
 */
__attribute__ ((cogmain)) static void _Fdserial_driver(void *p) {
    INIT_RTLIB;
    brk(0);

    FdSerial_t *dat = (FdSerial_t*)p;

    int rxpin = dat->rx_pin;
    int txpin = dat->tx_pin;
    while(1) {
        int have_data;
        testp(rxpin, have_data);
        if (have_data) {
            // we have a character, add it to the receive buffer
            volatile int c = 0;
            rdpin(c, rxpin);
            dat->rxbuff[dat->rx_head] = (c>>24) & 0xff;
            dat->rx_head = (dat->rx_head+1) & FDSERIAL_BUFF_MASK;
        }

        if (dat->tx_head != dat->tx_tail) {
            int busy;
            asm volatile (
                "rdpin r1, %[pin] wc\n"
                "wrc %[busy]\n"
                : [busy]"=r"(busy) : [pin]"r"(txpin) : "r1"
            );

            if (!busy) {
                wypin(dat->txbuff[dat->tx_tail], txpin);
                dat->tx_tail = (dat->tx_tail+1) & FDSERIAL_BUFF_MASK;
            }
        }
    }   
}

/**
 * start initializes and starts native assembly driver in a cog.
 * @param rxpin is pin number for receive input
 * @param txpin is pin number for transmit output
 * @param mode is interface mode. see header FDSERIAL_MODE_...
 * @param baudrate is frequency of bits ... 115200, 57600, etc...
 * @returns non-zero on success
 */
int _FdSerial_start(FdSerial_t *data, int rxpin, int txpin, int mode, int baudrate)
{
    memset(data, 0, sizeof(FdSerial_t));
    data->stack[0] = (int)data;
    data->rx_pin  = rxpin;                  // receive pin
    data->tx_pin  = txpin;                  // transmit pin
    data->mode    = mode;                   // interface mode
    data->baud   =  baudrate;    // baud
    data->buffptr = (int)&data->rxbuff[0];
    data->cogId = _coginit(COGINIT_MODE_COG, _Fdserial_driver, (void*)data->stack);
    data->users = 1;

    return data->cogId;
}

/**
 * stop stops the cog running the native assembly driver 
 */
void _FdSerial_stop(FdSerial_t *data) {
    if (data->cogId > 0) {
        cogstop(data->cogId - 1);
        data->cogId = 0;
    }
}
/**
 * rxflush empties the receive queue 
 */
void _FdSerial_rxflush(FdSerial_t *data) {
    // clear out queue by receiving all available 
    while(_FdSerial_rxcheck(data) >= 0);
}

/**
 * Gets a byte from the receive queue if available
 * Function does not block. We move rxtail after getting char.
 * @returns receive byte 0 to 0xff or -1 if none available 
 */
int _FdSerial_rxcheck(FdSerial_t *data) {
    int rc = -1;
    if (data->rx_tail != data->rx_head) {
        rc = data->rxbuff[data->rx_tail];
        data->rx_tail = (data->rx_tail+1) & FDSERIAL_BUFF_MASK;
    }
    return rc;
}

/**
 * Wait for a byte from the receive queue. blocks until something is ready
 * (unless _IONONBLOCK is set in the flags, in which case it returns EOF
 * immediately if no data is ready)
 * @returns received byte 
 */
int _FdSerial_getbyte(FILE *fp) {
    FdSerial_t *data = (FdSerial_t *)fp->drvarg[0];
    int rc = _FdSerial_rxcheck(data);
    while(rc < 0 && !(fp->_flag & _IONONBLOCK)) {
        rc = _FdSerial_rxcheck(data);
    }
    return rc;
}

/**
 * putbyte sends a byte on the transmit queue.
 * @param txbyte is byte to send. 
 */
int _FdSerial_putbyte(int txbyte, FILE *fp) {
    FdSerial_t *data = (FdSerial_t *)fp->drvarg[0];

    volatile unsigned char* txbuff = data->txbuff;

    while(data->tx_tail == ((data->tx_head+1) & FDSERIAL_BUFF_MASK)); // wait for space in queue

    txbuff[data->tx_head] = txbyte;
    data->tx_head = (data->tx_head+1) & FDSERIAL_BUFF_MASK;
    // if(data->mode & FDSERIAL_MODE_IGNORE_TX_ECHO)
    //     _FdSerial_getbyte(fp); // why not rxcheck or timeout ... this blocks for char
    return txbyte;
}

/**
 * drain waits for all bytes to be sent
 */
void
_FdSerial_drain(FdSerial_t *data) {
  while(data->tx_tail != data->tx_head);

  // wait for character to be transmitted
  // strictly speaking we only need to wait 10*data->ticks,
  // but give ourselves a bit of margin here
  waitx((CLKFREQ/data->baud) << 4);
}

/*
 * standard driver interface
 */

static int fdserial_fopen(FILE *fp, const char *name, const char *mode) {
    unsigned int baud = _dbgbaud;
    unsigned int txpin = DBG_UART_TX_PIN;
    unsigned int rxpin = DBG_UART_RX_PIN;
    int setBaud = 0;
    FdSerial_t *data;
    int mode_i = 0;
    int r;

    if (name && *name) {
        baud = atoi(name);
        setBaud = 1;
        while (*name && *name != ',') name++;
        if (*name) {
            name++;
            rxpin = atoi(name);
            while (*name && *name != ',') name++;
            if (*name) {
	            name++;
	            txpin = atoi(name);
                while (*name && *name != ',') name++;
                if (*name) {
                    name++;
                    mode_i = atoi(name);
                }
	        }
        }
    }

    /* look for an existing cog that handles these pins */
    for (data = coglist; data; data = data->next) {
        if (data->tx_pin == txpin || data->rx_pin == rxpin) {
	        // if the baud differs, tell the cog to change it
	        if (setBaud) {
	            data->baud = baud;
	        }
	        data->users++;
	        break;
	    }
    }

    if (!data) {
        data = malloc(sizeof(FdSerial_t));
        if (!data)
	        return -1;

        _uart_init(rxpin, txpin, baud, mode_i);
        r = _FdSerial_start(data, rxpin, txpin, mode_i, baud);
        if (r <= 0) {
	        free(data);
	        return -1;
	    }

        data->next = coglist;
        coglist = data;
    }
  
    fp->drvarg[0] = (unsigned long)data;
    fp->_flag |= _IODEV;
    fp->_lock = -1;
    return 0;
}

static int fdserial_fclose(FILE *fp) {
    FdSerial_t *data = (FdSerial_t *)fp->drvarg[0];
    FdSerial_t **prev_p, *p;
    fp->drvarg[0] = 0;

    data->users--;
    if (data->users > 0) {
      /* still other open handles */
      return 0;
    }

    /* wait for all data to be transmitted */
    _FdSerial_drain(data);

    /* now stop */
    _FdSerial_stop(data);

    /* remove from the list */
    prev_p = &coglist;
    p = coglist;
    while (p) {
        if (p == data) {
	        *prev_p = p->next;
	        break;
	    }
        
        prev_p = &p->next;
        p = *prev_p;
    }

    /* release memory */
    free(data);
    return 0;
}

const char _FullSerialName[] = "FDS:";

_Driver _FullDuplexSerialDriver = {
    _FullSerialName,
    fdserial_fopen,
    fdserial_fclose,
    _term_read,
    _term_write,
    NULL,       /* seek, not needed */
    NULL,       /* remove */
    _FdSerial_getbyte,
    _FdSerial_putbyte,
};

/*
+------------------------------------------------------------------------------------------------------------------------------+
�                                                   TERMS OF USE: MIT License                                                  �                                                            
+------------------------------------------------------------------------------------------------------------------------------�
�Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation    � 
�files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy,    �
�modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software�
�is furnished to do so, subject to the following conditions:                                                                   �
�                                                                                                                              �
�The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.�
�                                                                                                                              �
�THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE          �
�WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR         �
�COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,   �
�ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                         �
+------------------------------------------------------------------------------------------------------------------------------+
*/
