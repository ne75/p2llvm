/**
 * SD card driver based on https://obex.parallax.com/obex/sdsd-cc/
 */

// #define _DEBUG
// #define _DEBUG_SDMM
#include <stdio.h>
#include <propeller.h>
#include "sdmmc.h"
#include "diskio.h"

#define _USE_SMARTPINS

int _pin_clk;
int _pin_ss;
int _pin_di;
int _pin_do;
int _pin_do_in;

// 
// The values here were tested empirically to work at 300 MHz with some cards
// but it would be nice to have some "reason" for them :)
//
#define PAUSE()      waitx(16)
#define SHORTPAUSE() waitx(8)

#define DO_INIT()	fltl(PIN_DO)				/* Initialize port for MMC DO as input */

#define _pinr(pin) ({             \
    int result;                   \
    asm volatile (                \
        "testp %1, %0"            \
        : "=r" (result)           /* output operand */ \
        : "ri" (pin)              /* input operand */  \
    );                            \
    result;                       \
})
#define DO		(SHORTPAUSE(), (_pinr(PIN_DO) & 1))	/* Test for MMC DO ('H':true, 'L':false) */

#define DI_INIT()	dirh(PIN_DI)	/* Initialize port for MMC DI as output */
#define DI_H()		drvh(PIN_DI)	/* Set MMC DI "high" */
#define DI_L()		drvl(PIN_DI)	/* Set MMC DI "low" */

#define CK_INIT()	dirh(PIN_CLK)	/* Initialize port for MMC SCLK as output */
#define CK_H()		drvh(PIN_CLK); PAUSE();	/* Set MMC SCLK "high" */
#define	CK_L()		drvl(PIN_CLK); PAUSE();	/* Set MMC SCLK "low" */

#define CS_INIT()	dirh(PIN_SS)	/* Initialize port for MMC CS as output */
#define	CS_H()		drvh(PIN_SS); PAUSE();	/* Set MMC CS "high" */
#define CS_L()		drvl(PIN_SS); PAUSE();	/* Set MMC CS "low" */

// ==== private helper functions ====

/**
 * delay microseconds
 */
static void dly_us(UINT n) {
    waitx(CLKFREQ/1000000*n);
}

/**
 * set up a smart pin
 * 
 * mirror from flexprop
 */
static void _pinsetup(int pin, int mode, int xval, int yval) {
    dirl(pin);
    wrpin(mode, pin);
    wxpin(xval, pin);
    wypin(yval, pin);
}

/**
 * set up and run a smart pin
 * 
 * mirror from flexprop
 */
static void _pinstart(int pin, int mode, int xval, int yval) {
	_pinsetup(pin, mode, xval, yval);
  	dirh(pin);
}

/**
 * clear the smart pin
 */
static void _pinclear(int pin) {
	fltl(pin);
	wrpin(0, pin);
}

// ==== function implementations ====

int sd_setpins(int drv, int pclk, int pss, int pdi, int pdo) {
    if (drv != 0) return -1;

    _pin_clk = pclk;
    _pin_ss  = pss;
    _pin_di = pdi;
    _pin_do = pdo;

#ifdef _DEBUG
    printf("&_pin_clk=%x, _pin_clk = %d\n", (unsigned)&_pin_clk, _pin_clk);
#endif

    return 0;
}

int sd_init(BYTE *card_type) {
    BYTE ty, cmd, buf[10];
	UINT tmr, ck_div, spm_ck, spm_tx, spm_rx;
	DSTATUS s;
	int PIN_CLK = _pin_clk;
	int PIN_SS = _pin_ss;
	int PIN_DI = _pin_di;
	int PIN_DO = _pin_do;
	int SMPIN_DO;

	s = STA_NOINIT;

#ifdef _DEBUG_SDMM
	printf("disk_initialize: PINS=%d %d %d %d\n", PIN_CLK, PIN_SS, PIN_DI, PIN_DO);
#endif

	dly_us(10000);			/* 10ms */

#ifdef _USE_SMARTPINS
	if (_abs(PIN_CLK - PIN_DI) > 3)  return s;

	if (_abs(PIN_CLK - PIN_DO) <= 3) {
		spm_rx = ((PIN_CLK - PIN_DO) & 7) << 24;  // clock pin offset for smartB input to rx smartpin
		spm_rx |= P_SYNC_RX | P_OE | P_INVERT_OUTPUT | P_HIGH_15K | P_LOW_15K;  // rx smartpin mode, with 15 k pull-up
		_pin_do_in = _pin_do = SMPIN_DO = PIN_DO;  // rx pin is smartpin
	} else {
        // ???? what is this magic? 
		if (PIN_CLK > PIN_DO)  { // NOTE:  This can only be accomplished for input pins!
			SMPIN_DO = PIN_CLK - 3;
			if ((SMPIN_DO == PIN_DI) || (SMPIN_DO == PIN_SS))
				SMPIN_DO++;
			if ((SMPIN_DO == PIN_DI) || (SMPIN_DO == PIN_SS))
				SMPIN_DO++;
		} else {
			SMPIN_DO = PIN_CLK + 3;
			if ((SMPIN_DO == PIN_DI) || (SMPIN_DO == PIN_SS))
				SMPIN_DO--;
			if ((SMPIN_DO == PIN_DI) || (SMPIN_DO == PIN_SS))
				SMPIN_DO--;
		}

		if (_abs(PIN_DO - SMPIN_DO) > 3)  return s;

#ifdef _DEBUG_SDMM
		printf("remapped PINS=%d %d %d %d %d\n", PIN_CLK, PIN_SS, PIN_DI, PIN_DO, SMPIN_DO);
#endif

		spm_rx = ((PIN_DO - SMPIN_DO) & 7) << 28;  // rx data pin offset for smartA input to rx smartpin
		spm_rx |= ((PIN_CLK - SMPIN_DO) & 7) << 24;  // clock pin offset for smartB input to rx smartpin
		spm_rx |= P_SYNC_RX;  // rx smartpin mode
		_pin_do_in = PIN_DO;  // remember rx input mapping for later de-init
		_pin_do = SMPIN_DO;  // smartpin in place of the rx pin
	}

	wrpin(PIN_SS, 0);
	drvh(PIN_SS);  // Deselect SD card

//   I/O registering (P_SYNC_IO) the SPI clock pin was found to be vital for stability.  Although it
//   adds extra lag to the tx pin, it also effectively (when rx pin is unregistered) makes a late-late
//   rx sample point.
	ck_div = 0x00100020;  // sysclock/32
	spm_ck = P_PULSE | P_OE | P_INVERT_OUTPUT | P_SCHMITT_A;  // CPOL = 1 (SPI mode 3)
	_pinstart(PIN_CLK, spm_ck | P_SYNC_IO, ck_div, 0);

	spm_tx = P_SYNC_TX | P_OE | (((PIN_CLK - PIN_DI) & 7) << 24);  // tx smartpin mode and clock pin offset
	_pinstart(PIN_DI, spm_tx | P_SYNC_IO, 31, -1);  // rising clock + 5 + 1 ticks lag, 32-bit, continuous mode, initial 0xff

	wrpin( PIN_DO, P_INVERT_OUTPUT | P_HIGH_15K | P_LOW_15K );  // config for 15 k pull-up
	_pinstart( SMPIN_DO, spm_rx, 7 | 32, 0 );  // 8-bit, late-late sampling (post-clock + smartB registration)

#ifdef _DEBUG_SDMM
	printf("smartpin modes:  %d=%08x  %d=%08x  %d=%08x\n", PIN_CLK, spm_ck, PIN_DI, spm_tx, SMPIN_DO, spm_rx);
#endif

#else // not using smart pins
	CS_INIT(); CS_H();		/* Initialize port pin tied to CS */
	CK_INIT(); CK_L();		/* Initialize port pin tied to SCLK */
	DI_INIT();				/* Initialize port pin tied to DI */
	DO_INIT();				/* Initialize port pin tied to DO */
#endif

	sd_rcvr_mmc(buf, 10);  // Apply 80 dummy clocks and the card gets ready to receive command
	sd_send_cmd(CMD0, 0);  // Enter Idle state
	sd_deselect();
	dly_us(100);

	sd_rcvr_mmc(buf, 10);  // Apply 80 dummy clocks and the card gets ready to receive command

	ty = 0;
	if (sd_send_cmd(CMD0, 0) == 1) {			/* Enter Idle state */

#ifdef _DEBUG_SDMM
		printf("idle OK\n");
#endif

		if (sd_send_cmd(CMD8, 0x1AA) == 1) {	/* SDv2? */
#ifdef _DEBUG_SDMM
			printf("SDv2\n");
#endif	
			printf("start RX\n");
			sd_rcvr_mmc(buf, 4);							/* Get trailing return value of R7 resp */
			printf("done RX\n");
			waitx(CLKFREQ/100);
			if (buf[2] == 0x01 && buf[3] == 0xAA) {		/* The card can work at vdd range of 2.7-3.6V */
				for (tmr = 1000; tmr; tmr--) {			/* Wait for leaving idle state (ACMD41 with HCS bit) */
					if (sd_send_cmd(ACMD41, 1UL << 30) == 0) break;
					dly_us(1000);
				}

				if (tmr && sd_send_cmd(CMD58, 0) == 0) {	/* Check CCS bit in the OCR */
					sd_rcvr_mmc(buf, 4);
					ty = (buf[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;	/* SDv2 */
				}

#ifdef _USE_SMARTPINS // if using smart pins, set up based on system frequency
				tmr = CLKFREQ;
    #ifdef _SDHC_45MHZ
			// Performance option for "Default Speed" (Up to 50 MHz SPI clock)
				spm_tx |= P_INVERT_B;  // falling clock + 4 tick lag + 1 tick (smartB registration)
				if( tmr <= 150_000_000 )  ck_div = 0x0002_0004;  // sysclock/4
				else if( tmr <= 200_000_000 )  ck_div = 0x0002_0005;  // sysclock/5
				else if( tmr <= 280_000_000 )  ck_div = 0x0002_0006;  // sysclock/6
				else  ck_div = 0x0003_0008;  // sysclock/8
    #else
			// Reliable option (Up to 30 MHz SPI clock)
				if (tmr <= 100000000)  spm_tx |= P_INVERT_B;  // falling clock + 4 tick lag + 1 tick (smartB registration)
				else if (tmr <= 200000000)  spm_tx |= P_INVERT_B | P_SYNC_IO;  // falling clock + 5 tick lag + 1 tick
				// else:  spm_tx default, rising clock + 4 tick lag + 1 tick (smartB registration)
				if( tmr <= 100000000)  ck_div = 0x00020004;  // sysclock/4
				else if (tmr <= 150000000)  ck_div = 0x00030006;  // sysclock/6
				else if (tmr <= 200000000)  ck_div = 0x00040008;  // sysclock/8
				else if (tmr <= 250000000)  ck_div = 0x0005000a;  // sysclock/10
				else ck_div = 0x0006000c;  // sysclock/12
    #endif
    #ifdef _DEBUG_SDMM
				printf("SDHC %d MHz selected\n", tmr / ((ck_div&0xffff) * 1000000));
    #endif
#endif
			}
		} else {							/* SDv1 or MMCv3 */
#ifdef _DEBUG_SDMM
			printf("SDv1/MMCv3\n");
#endif
			if (sd_send_cmd(ACMD41, 0) <= 1) {
				ty = CT_SD1; cmd = ACMD41;	/* SDv1 */
			} else {
				ty = CT_MMC; cmd = CMD1;	/* MMCv3 */
			}

			for (tmr = 1000; tmr; tmr--) {			/* Wait for leaving idle state */
				if (sd_send_cmd(cmd, 0) == 0) break;
				dly_us(1000);
			}

			if (!tmr || sd_send_cmd(CMD16, 512) != 0) {/* Set R/W block length to 512 */
				//printf("tmr = %d\n", tmr);
				ty = 0;
			}

#ifdef _USE_SMARTPINS
			tmr = CLKFREQ;
			if (tmr <= 100000000)  spm_tx |= P_INVERT_B;  // falling clock + 4 tick lag + 1 tick (smartB registration)
			else if ( tmr <= 200000000)  spm_tx |= P_INVERT_B | P_SYNC_IO;  // falling clock + 5 tick lag + 1 tick
			// else:  spm_tx default, rising clock + 4 tick lag + 1 tick (smartB registration)
		// Reliable option (Up to 25 MHz SPI clock)
			if (tmr <= 100000000) ck_div = 0x00020004;  // sysclock/4
			else if (tmr <= 150000000) ck_div = 0x00030006;  // sysclock/6
			else if (tmr <= 200000000) ck_div = 0x00040008;  // sysclock/8
			else if (tmr <= 250000000) ck_div = 0x0005000a;  // sysclock/10
			else if (tmr <= 300000000) ck_div = 0x0006000c;  // sysclock/12
			else  ck_div = 0x0007000e;  // sysclock/14
    #ifdef _DEBUG_SDMM
			printf("SDSC %d MHz selected\n", tmr / ((ck_div&0xffff) * 1000000));
    #endif
#endif
		}
	}

#ifdef _DEBUG_SDMM
	printf("ty = %d\n", ty);
#endif

    *card_type = ty;
	s = ty ? 0 : STA_NOINIT;

	sd_deselect();

#ifdef _USE_SMARTPINS
	wxpin( PIN_CLK, ck_div );  // update clock smartpin sysclock divider
	wrpin( PIN_DI, spm_tx );  // update tx smartpin clock inversion and data registration
    
    #ifdef _DEBUG_SDMM
	printf( "SPI clock ratio = sysclock/%d\n", ck_div & 0xffff );
    #endif

#endif

	return s;
}

int sd_deinit() {
	int PIN_CLK = _pin_clk;
    int PIN_SS = _pin_ss;
    int PIN_DI = _pin_di;
    int PIN_DO = _pin_do_in;
    int SMPIN_DO = _pin_do;

#ifdef _USE_SMARTPINS
	#ifdef _DEBUG_SDMM
	printf("clear pins %d %d %d %d %d\n", PIN_CLK, PIN_SS, PIN_DI, PIN_DO, SMPIN_DO);
	#endif
    
	_pinclear(SMPIN_DO);
    _pinclear(PIN_DO);
    _pinclear(PIN_DI);
    _pinclear(PIN_CLK);
    _pinclear(PIN_SS);

    dly_us(10000);
#endif
    return 0;
}

void sd_xmit_mmc(const BYTE* buff, UINT bc) {
	int PIN_CLK = _pin_clk;
	int PIN_DI = _pin_di;

#ifdef _USE_SMARTPINS
// Smartpin SPI transmitter using "continuous" mode and 32-bit word size
//   NOTE: data out always has at least a 4 sysclock lag
	asm volatile (
        "dirl %[PIN_DI]\n"		// reset tx smartpin, clears excess data
        "setq #1\n"
        "rdlong	r2, %[buff]\n"	// fetch first data. will write to r2 and r3
        "rev r2\n"
        "movbyts r2, #0x1b\n"	// endian swap
        "wypin	r2, %[PIN_DI]\n"	// first data to tx shifter

        "mov r2, %[bc]\n"
        "shr %[bc], #2  wz\n"	// longword count (rounded down)
        "shl r2, #3\n"		// bit count (exact)
        "wypin r2, %[PIN_CLK]\n"	// begin SPI clocks

        "dirh %[PIN_DI]\n"		// liven tx buffer, continuous mode
        "add %[buff], #8\n"
        "rev r3\n"
        "movbyts r3, #0x1b\n"	// endian swap

        ".Ltx_loop%=:\n"
        "if_nz wypin r3, %[PIN_DI]\n"	// data to tx buffer
        "if_nz rdlong r3, %[buff]\n"		// fetch next data
        "if_nz add %[buff], #4\n"
        "if_nz rev r3\n"
        "if_nz movbyts r3, #0x1b\n"	// endian swap

        ".Ltx_wait%=:\n"
	    "if_nz testp %[PIN_DI]  wc\n"	// wait for tx buffer empty
        "if_nc_and_nz jmp #.Ltx_wait%=\n"
	    "if_nz djnz %[bc], #.Ltx_loop%=\n"

// Wait for completion
        ".Ltx_wait_done%=:\n"
		"testp %[PIN_CLK]  wc\n"
	    "if_nc jmp	#.Ltx_wait_done%=\n"

		"dirl %[PIN_DI]\n"		// reset tx smartpin to clear excess data
		"wypin ##-1, %[PIN_DI]\n"	// TX 0xFF, continuous mode

        : [buff]"+&r"(buff), [bc]"+&r"(bc)
        : [PIN_DI]"ri"(PIN_DI), [PIN_CLK]"ri"(PIN_CLK) 
		: "r2", "r3"
    );

#else        
	do {
		d = *buff++;	/* Get a byte to be sent */
		if (d & 0x80) DI_H(); else DI_L();	/* bit7 */
		CK_H(); CK_L();
		if (d & 0x40) DI_H(); else DI_L();	/* bit6 */
		CK_H(); CK_L();
		if (d & 0x20) DI_H(); else DI_L();	/* bit5 */
		CK_H(); CK_L();
		if (d & 0x10) DI_H(); else DI_L();	/* bit4 */
		CK_H(); CK_L();
		if (d & 0x08) DI_H(); else DI_L();	/* bit3 */
		CK_H(); CK_L();
		if (d & 0x04) DI_H(); else DI_L();	/* bit2 */
		CK_H(); CK_L();
		if (d & 0x02) DI_H(); else DI_L();	/* bit1 */
		CK_H(); CK_L();
		if (d & 0x01) DI_H(); else DI_L();	/* bit0 */
		CK_H(); CK_L();
	} while (--bc);
#endif   

}

void sd_rcvr_mmc(BYTE *buff, UINT bc) {
    int r, bc2;
    int PIN_CLK = _pin_clk;
    int PIN_DO = _pin_do;
    int PIN_DI = _pin_di;

#ifdef _USE_SMARTPINS
    drvl(PIN_CLK);    // smartpin enable, added to support multi-cog operation

// Smartpin SPI byte receiver,
//  SPI clock mode is selected in disk_initialize()
//  NOTE: Has hard coded mode select for post-clock "late" sampling
//

    asm volatile (
		"drvl	%[PIN_DO]\n"
		"drvl	%[PIN_DI]\n"    // enable tx smartpin, DI pin stays high while not transmitting
		"mov	%[bc2], %[bc]  wz\n"

// 32-bit rx (WFLONG), about 10% faster than 8-bit code
		"shr	%[bc2], #2  wz\n"
	    "if_z	jmp	#.Lrx_bytes%=\n"

		"mov	%[r], %[bc2]\n"
		"shl	%[r], #5\n"			// bit count
		"wypin	%[r], %[PIN_CLK]\n"		// begin SPI clocks
		"wxpin	#63, %[PIN_DO]\n"	// 32 bits, sample after rising clock

        ".Lrx_loop_a%=:\n"
        ".Lrx_wait_a%=:\n"
		"testp	%[PIN_DO]  wc\n"		// wait for received byte
	    "if_nc	jmp	#.Lrx_wait_a%=\n"

		"rdpin	%[r], %[PIN_DO]\n"		// longword from rx buffer
		"rev	%[r]\n"
		"movbyts %[r], #0x1b\n"			// endian swap
		"wrlong	%[r], %[buff]\n"			// store longword
		"add	%[buff], #4\n"
		"djnz	%[bc2], #.Lrx_loop_a%=\n"

// 8-bit rx (WFBYTE)
        ".Lrx_bytes%=:\n"
		"and	%[bc], #3  wz\n"		// up to 3 bytes
	    "if_z	jmp #.Lrx_done%=\n"
		"wxpin	#39, %[PIN_DO]\n"		// 8 bits, sample after rising clock
        
        ".Lrx_loop%=:\n"
		"wypin	#8, %[PIN_CLK]\n"		// begin SPI clocks

        ".Lrx_wait%=:\n"
		"testp	%[PIN_DO]  wc\n"		// wait for received byte
	    "if_nc	jmp	#.Lrx_wait%=\n"

		"rdpin	%[r], %[PIN_DO]\n"		// byte from rx buffer
		"rev	%[r]\n"
		"wrbyte	%[r], %[buff]\n"			// store byte
		"add	%[buff], #1\n"
		"djnz	%[bc], #.Lrx_loop%=\n"

		".Lrx_done%=:\n"

		"dirl	%[PIN_DO]\n"			// clear rx buffer
        : [bc2]"+&r"(bc2), [buff]"+&r"(buff), [bc]"+&r"(bc), [r]"+&r"(r)
        : [PIN_DO]"ri"(PIN_DO), [PIN_DI]"ri"(PIN_DI), [PIN_CLK]"ri"(PIN_CLK) 
    );

#else

	DI_H();	/* Send 0xFF */

	do {
		r = 0;	 if (DO) r++;	/* bit7 */
		CK_H(); CK_L();
		r <<= 1; if (DO) r++;	/* bit6 */
		CK_H(); CK_L();
		r <<= 1; if (DO) r++;	/* bit5 */
		CK_H(); CK_L();
		r <<= 1; if (DO) r++;	/* bit4 */
		CK_H(); CK_L();
		r <<= 1; if (DO) r++;	/* bit3 */
		CK_H(); CK_L();
		r <<= 1; if (DO) r++;	/* bit2 */
		CK_H(); CK_L();
		r <<= 1; if (DO) r++;	/* bit1 */
		CK_H(); CK_L();
		r <<= 1; if (DO) r++;	/* bit0 */
		CK_H(); CK_L();
		*buff++ = r;			/* Store a received byte */
	} while (--bc);
#endif     
}

int sd_wait_ready() {
	BYTE d = 0;
	UINT tmr, tmout;

	tmr = _cnt();
	tmout = CLKFREQ >> 1;  // 500 ms timeout
	for(;;) {
		sd_rcvr_mmc( &d, 1 );
		if( d == 0xFF )  return 1;
		if( _cnt() - tmr >= tmout )  return 0;
	}
}

void sd_deselect() {
    BYTE d;
    int PIN_SS = _pin_ss;
    int PIN_CLK = _pin_clk;
    int PIN_DI = _pin_di;
    int PIN_DO = _pin_do;

    CS_H();				/* Set CS# high */
    sd_rcvr_mmc(&d, 1);	/* Dummy clock (force DO hi-z for multiple slave SPI) */

#ifdef _USE_SMARTPINS
    // added to support multi-cog operation
    flth(PIN_SS);    // disable SPI interface
    fltl(PIN_CLK);    // disable clock smartpin
    fltl(PIN_DI);    // disable tx smartpin
    fltl(PIN_DO);    // disable rx smartpin
#endif        
}

int sd_select() {
    BYTE d = 0;
    int PIN_SS = _pin_ss;

#ifdef _USE_SMARTPINS
    drvl(PIN_SS);    // pin low
#else
    CS_L();    // Set CS# low
#endif
	sd_rcvr_mmc(&d, 1);    // Dummy clock (force DO enabled)
    if (sd_wait_ready()) return 1;    // Wait for card ready

    sd_deselect();
    return 0;    // Failed
}

int sd_rcvr_datablock(BYTE *buff, UINT btr) {
	BYTE d[2];
	UINT tmr, tmout;

	tmr = _cnt();
	tmout = CLKFREQ >> 3;  // 125 ms timeout
	for(;;) {
		sd_rcvr_mmc( d, 1 );
		if( d[0] != 0xFF )  break;
		if( _cnt() - tmr >= tmout )  break;
	}
	if (d[0] != 0xFE) return 0;		/* If not valid data token, return with error */

	sd_rcvr_mmc(buff, btr);			/* Receive the data block into buffer */
	sd_rcvr_mmc(d, 2);				/* Discard CRC */

	return 1;				/* Return with success */
}

int sd_xmit_datablock(const BYTE *buff, BYTE token) {
	BYTE d[2];

	if (!sd_wait_ready()) return 0;

	d[0] = token;
	sd_xmit_mmc(d, 1);				/* Xmit a token */
	if (token != 0xFD) {		/* Is it data token? */
		sd_xmit_mmc(buff, 512);	/* Xmit the 512 byte data block to MMC */
		sd_rcvr_mmc(d, 2);			/* Xmit dummy CRC (0xFF,0xFF) */
		sd_rcvr_mmc(d, 1);			/* Receive data response */
		if ((d[0] & 0x1F) != 0x05)	/* If not accepted, return with error */
			return 0;
	}

	return 1;
}

int sd_send_cmd(BYTE cmd, DWORD arg) {
	BYTE n, buf[7];


	if (cmd & 0x80) {	/* ACMD<n> is the command sequense of CMD55-CMD<n> */
		cmd &= 0x7F;
		n = sd_send_cmd(CMD55, 0);
		if (n > 1) return n;
	}

	/* Select the card and wait for ready except to stop multiple block read */
	if (cmd != CMD12) {
		sd_deselect();
		if (!sd_select()) return 0xFF;
	}

	/* Send a command packet */
	buf[0] = 0x40 | cmd;			/* Start + Command index */
	#ifdef __propeller2__
	*(DWORD*)(buf+1) = __builtin_bswap32(arg);
	#else
	buf[1] = (BYTE)(arg >> 24);		/* Argument[31..24] */
	buf[2] = (BYTE)(arg >> 16);		/* Argument[23..16] */
	buf[3] = (BYTE)(arg >> 8);		/* Argument[15..8] */
	buf[4] = (BYTE)arg;				/* Argument[7..0] */
	#endif
	n = 0x01;						/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;		/* (valid CRC for CMD0(0)) */
	if (cmd == CMD8) n = 0x87;		/* (valid CRC for CMD8(0x1AA)) */
	buf[5] = n;
	sd_xmit_mmc(buf, 6);

	/* Receive command response */
	if (cmd == CMD12) sd_rcvr_mmc(buf+6, 1);	/* Skip a stuff byte when stop reading */
	n = 10;								/* Wait for a valid response in timeout of 10 attempts */
	do
		sd_rcvr_mmc(buf+6, 1);
	while ((buf[6] & 0x80) && --n);

	return buf[6];			/* Return with the response value */
}




/*

unsigned long Pins[2];

void SetSD(int drive, int cs, int clk, int mosi, int miso)
{
    Pins[drive] = cs | (clk << 8) | (mosi << 16) | (miso << 24);
}


void EnableSD(int drive)
{
    int cs, clk, mosi, miso;

    // wait(10);
    cs = Pins[drive] & 0xff;
    clk = (Pins[drive] >> 8) & 0xff;
    mosi = (Pins[drive] >> 16) & 0xff;
    miso = (Pins[drive] >> 24) & 0xff;
    drvh(cs);
    drvl(clk);
    dirh(mosi);
    dirl(miso);
}

void SendSD(int drive, const BYTE* buff, unsigned int bc)
{
    int mosi = (Pins[drive] >> 16) & 0xff;
    int clk = (Pins[drive] >> 8) & 0xff;
    int i,j,x;

    asm volatile ("mov %[i], %[bc]\n"
             ".Li: mov %[j], #8\n"
                  "rdbyte %[x], %[b]\n"
                  "shl %[x], #24\n"
             ".Lj: shl %[x], #1 wc\n"
             "if_c drvh %[mosi]\n"
            "if_nc drvl %[mosi]\n"
                  "drvh %[clk]\n"
                  "waitx #2\n"
                  "drvl %[clk]\n"
                  "waitx #2\n"
                  "djnz %[j], #.Lj\n"
                  "add %[b], #1\n"
                  "djnz %[i], #.Li\n"
                  :[b]"+r"(buff), [i]"+r"(i), [j]"+r"(j), [x]"+r"(x),
                  [mosi]"+r"(mosi), [clk]"+r"(clk), [bc]"+r"(bc):);

}

void ReceiveSD(int drive, BYTE *buff, unsigned int bc)
{
    int mosi = (Pins[drive] >> 16) & 0xff;
    int miso = (Pins[drive] >> 24) & 0xff;
    int clk = (Pins[drive] >> 8) & 0xff;
    int i, j, x;

    asm volatile ("drvh %[mosi]\n"
                  "mov %[i], %[bc]\n"
             ".Li2: mov %[x], #0\n"
                  "mov %[j], #8\n"
             ".Lj2: testp %[miso] wc\n"
                  "drvh %[clk]\n"
                  "rcl %[x], #1\n"
                  "drvl %[clk]\n"
                  "waitx #4\n"
                  "djnz %[j], #.Lj2\n"
                  "wrbyte %[x], %[b]\n"
                  "add %[b], #1\n"
                  "djnz %[i], #.Li2\n"
                  :[b]"+r"(buff), [i]"+r"(i), [j]"+r"(j), [x]"+r"(x),
                  [clk]"+r"(clk), [miso]"+r"(miso), [mosi]"+r"(mosi), [bc]"+r"(bc):);
}

//__attribute__ ((section ("lut"), cogtext, no_builtin("memset")))
void SendSDB(int drive, const BYTE* buff, unsigned int bc)
{
    int i;
    unsigned int x;
    int mosi = (Pins[drive] >> 16) & 0xff;
    int clk = (Pins[drive] >> 8) & 0xff;
    int j;

    asm volatile (
            "rdfast #0, %[b]\n"
            "mov   %[i], %[t]\n"
    ".Lxi:   mov   %[j], #8\n"
            "rfbyte %[x]\n"
            "shl %[x], #24\n"
    ".Lxj:   shl %[x], #1  wc\n"
    "if_c    drvh %[mosi]\n"
    "if_nc   drvl %[mosi]\n"
            "drvh %[clk]\n"
            "nop\n"
            "drvl %[clk]\n"
            "nop\n"
            "djnz %[j], #.Lxj\n"
            "djnz %[i], #.Lxi\n"
            :[i]"+r"(i), [j]"+r"(j), [x]"+r"(x), [b]"+r"(buff),
            [mosi]"+r"(mosi), [clk]"+r"(clk), [t]"+r"(bc):);
}

//__attribute__ ((section ("lut"), cogtext, no_builtin("memset")))
void ReceiveSDB(int drive, BYTE *buff, unsigned int bc)
{
    int mosi = (Pins[drive] >> 16) & 0xff;
    int miso = (Pins[drive] >> 24) & 0xff;
    int clk = (Pins[drive] >> 8) & 0xff;
    int j;
    unsigned int v;

    asm volatile (
           "wrfast #0, %[b]\n"
           "drvh %[mosi]\n"
           "mov R9, %[c]\n"
    ".LxI:  mov %[v], #0\n"
           "mov %[j], #8\n"
    ".LxJ:  drvh %[clk]\n"
           "testp %[miso] wc\n"
           "drvl %[clk]\n"
           "rcl %[v], #1\n"
           "waitx #4\n"
           "djnz %[j], #.LxJ\n"
           "wfbyte %[v]\n"
           "djnz R9, #.LxI\n"
           :[j]"+r"(j), [v]"+r"(v), [b]"+r"(buff),
           [clk]"+r"(clk), [miso]"+r"(miso), [mosi]"+r"(mosi), [c]"+r"(bc));
}

int GetStatus(int drive)
{
    int i = 0;
    BYTE b = 0;
    
    for (i=0;i<50000;i++)
    {
        ReceiveSD(drive, &b, 1);
        if (b == 0xff)
        	return 1;
    }
#ifdef _DEBUG
    printf("Drive Not Read\n");
#endif
	return 0;
}

void ReleaseSD(int drive)
{
    int cs;
    BYTE b;
    
    cs = Pins[drive] & 0xff;
    drvh(cs);
    ReceiveSD(drive, &b, 1);
}

int SelectSD(int drive)
{
    int cs;
    BYTE b;
    
    cs = Pins[drive] & 0xff;
    drvl(cs);
    ReceiveSD(drive, &b, 1);
    if (GetStatus(drive))
    	return 1;
    ReleaseSD(drive);
    return 0;
}

int ReceiveBlock(int drive, BYTE *buff, unsigned int bc)
{
    BYTE b[2];
    int i;
   
    for (i=0;i<5000;i++)
    {
        ReceiveSD(drive, b, 1);
        if (*b != 0xff)
        	break;
    }
	if (*b != 0xfe)
		return 0;

    ReceiveSD(drive, buff, bc);
	ReceiveSD(drive, b, 2);    // Ditch CRC
	return 1;
}

int SendBlock(int drive, const BYTE *buff, int token)
{
    BYTE b[2];
    
    if (GetStatus(drive) == 0)
    	return 0;
    
    b[0] = token;
    SendSD(drive, b, 1);
    if (token != 0xfd)
    {
        SendSD(drive, buff, 512);
        ReceiveSD(drive, b, 2);  // CRC value
        ReceiveSD(drive, b, 1);
        if ((*b & 0x1f) != 0x05)
        	return 0;
    }
	return 1;    
}

BYTE SendCommand(int drive, BYTE cmd, unsigned int arg)
{
    BYTE b[6];
    BYTE n;
    
    if (cmd & 0x80)
    {
        cmd = cmd & 0x7f;
        n = SendCommand(drive, CMD55, 0);
        if (n > 1)
            return n;
    }
    
    if (cmd != CMD12)
    {
        ReleaseSD(drive);
        if (!SelectSD(drive)) {
            #ifdef _DEBUG
                printf("Failed to select SD\n");
            #endif
        	return 0xff;
        }
    }
    
    b[0] = 0x40 | cmd;
    b[1] = arg >> 24;
    b[2] = arg >> 16;
    b[3] = arg >> 8;
    b[4] = arg;
    n = 0x01;
    if (cmd == CMD0)
    	n = 0x95;
    if (cmd == CMD8)
    	n = 0x87;
    b[5] = n;
    SendSD(drive, b, 6);
    
    if (cmd == CMD12)
    	ReceiveSD(drive, &n, 1);

    for (int i=0;i<10;i++)
    {
        ReceiveSD(drive, &n, 1);
        if ((n & 0x80) == 0)
        	break;
    }
#ifdef _DEBUG
    printf("Command: %d (%x) --> %d\n", cmd, arg, n);
#endif
    return n;
}
*/