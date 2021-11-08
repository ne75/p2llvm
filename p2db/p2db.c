#include <propeller.h>
#include <debug.h>
#include <stdio.h>

/**
 * this function must not call others in it's final form. it will use its own functions for reading and writing over UART
 */

#define INT_CAST(x) (*(int*)(&x))
#define TX(c) wypin(c, _uart_tx_pin); waitx(_uart_clock_per_bit*10);

#define tx_char(c) asm( \
            ".Lser_tx%=:\n"   \
		    "wypin %0, %1\n"    \
		    "waitx #20\n"   \
            ".Ltxflush%=:\n" \
		    "testp %1 wc\n" \
	"if_nc	jmp	#.Ltxflush%=\n" : : "r"(c), "r"(_uart_tx_pin))

#define rx_char(c) asm(  \
           ".Lser_rx%=:\n" \
           "testp %1 wc\n"  \
	"if_nc  jmp #.Lser_rx%=\n"    \
  	       "rdpin %0, %1\n" \
	       "shr %0, #24" : "=r"(c) : "r"(_uart_rx_pin))

/**
 * The debug app
 * 
 * written entirely in asm for space concerns and prevent the optimizer from doing weird stuff
 * 
 * hub address 0xfe400..0xfe411 will be used for the 18 byte stat array
 * hub address 0xfe412..0xfe418 will be used for the 6 byte rx array
 * 
 */
__attribute__ ((cogmain, noreturn)) void __dbg_run() {
    asm(    
            // r0 will hold the address of stat.
            "augs #0x7f2\n"      
            "mov $r0, #0\n"    

            // r1 will hold the address of rx.     
            "augs #0x7f2\n"     
            "mov $r1, #0x12\n"      

            // set up by writing '~' and 'g' to our stat array
            "wrbyte #0x7e, $r0\n"   
            "mov $r2, $r0\n"
            "add $r2, #1\n"
            "wrbyte #0x67, $r2\n"

            // fill up stat array, stat1
            "add $r2, #1\n"         
            "getbrk $r3 wcz\n"      
            "wrlong $r3, $r2\n"

            // fill up stat array, stat2
            "add $r2, #4\n"         
            "getbrk $r4 wc\n"
            "wrlong $r4, $r2\n"

            // fill up stat array, stat3
            "add $r2, #4\n"         
            "getbrk $r4 wz\n"
            "wrlong $r4, $r2\n"

            // record PC 
            "add $r2, #4\n"         
            "wrlong $inb, $r2\n"     // in the debug ISR, inb becomes iret0, the return address of the debug ISR, i.e. the instruction after the breakpoint PC

            // get coginit status bit
            "shr $r3, #23\n"        
            "and $r3, #1\n"

            // if this is a coginit interrupt, set up for a brk interrupt and return 
            "tjz $r3, #.Lstat_dump\n"
            "brk %0\n"
            "ret\n"

    ".Lstat_dump:\n"
            // dump current data         
            "mov $r3, #18\n"  
            "call #.Ltx_bytes\n"

            // main loop. we stay here forever or return from the interrupt
    ".Lmain_loop:\n"
            // get a byte, if we didn't get a '$', jump back to the start of the routine
            "call #.Lser_rx\n"
            "cmp $r31, #0x24 wz\n"      
    "if_nz  jmp #.Lmain_loop\n"        
            
            // read in 5 bytes
            "mov $r2, $r1\n"
            "mov $r3, #5\n"

    ".Lrx_cmd:"
            "call #.Lser_rx\n"
            "wrbyte $r31, $r2\n"
            "add $r2, #1\n"
            "djnz $r3, #.Lrx_cmd\n"

            // get the command byte
            "mov $r2, $r1\n"
            "rdbyte $r3, $r2\n"
            "add $r2, #1\n"

            // switch statement
            "cmp $r3, #0x68 wz\n"   // h
    "if_z   jmp #.Lcase_h\n"
            "cmp $r3, #0x72 wz\n"   // r
    "if_z   jmp #.Lcase_r\n"
            "cmp $r3, #0x62 wz\n"   // b
    "if_z   jmp #.Lcase_b\n"

    // h command case
    ".Lcase_h:"
            "mov $r4, $r0\n"
            "add $r4, #1\n"
            "wrbyte #0x68, $r4\n"
            "add $r4, #1\n"
            "rdlong $r5, $r2\n"   // r5 is the address we want to get 
            "rdbyte $r5, $r5\n"   // r5 is the value we wanted to get
            "wrbyte $r5, $r4\n"

            // transmite 3 bytes from stat         
            "mov $r3, #3\n" 
            "call #.Ltx_bytes\n"

            "jmp #.Lmain_loop\n"

    // r command case
    ".Lcase_r:"
            "mov $r4, $r0\n"
            "add $r4, #1\n"
            "wrbyte #0x72, $r4\n"
            "add $r4, #1\n"
            "rdlong $r5, $r2\n"     // r5 is the register address we want
            "alts $r5, #0\n"        // next instruction's s field will be the value in r5, i.e the register we want to read
            "mov $r5, $0\n"         // r5 is the value we wanted to get
            "wrlong $r5, $r4\n"

            // transmit 6 bytes from stat         
            "mov $r3, #6\n" 
            "call #.Ltx_bytes\n"

            "jmp #.Lmain_loop\n" 

    // b command case
    ".Lcase_b:"
            "rdlong $r5, $r2\n"   // r5 is the brk value 
            "brk $r5\n"
            // nothing to transmit, exit the debug isr

            "ret\n"   

    /**
     * tx bytes from stat. r3 should contain the number of bytes to send. uses r2 and r30
     */     
    ".Ltx_bytes:\n"
            "mov $r2, $r0\n"
    ".Ltx_loop:\n"
            "rdbyte $r30, $r2\n"
            "call #.Lser_tx\n"
            "add $r2, #1\n"
            "djnz $r3, #.Ltx_loop\n"
            "ret\n" 

    /**
     * tx_ser subroutine. place byte to tx in r10
     */
    ".Lser_tx:\n" 
            "wypin $r30, %1\n"   
            "waitx #20\n"  
    ".Ltxflush:\n"
            "testp %1 wc\n"
    "if_nc	jmp	#.Ltxflush\n"
            "ret\n"

    /**
     * rx_ser subroutine. read byte, blocks until one is received. 
     * byte is stored in r31
     */ 
    ".Lser_rx:\n" 
            "testp %2 wc\n"  
	"if_nc  jmp #.Lser_rx\n"   
  	        "rdpin $r31, %2\n"
	        "shr $r31, #24\n"
            "ret\n"

        : // outputs
        : "i"(DEBUG_BRK), "r"(_uart_tx_pin), "r"(_uart_rx_pin) // inputs
        : "r0", "r1", "r2", "r3", "r4", "r5", "r30", "r31" // clobber
    );

    /*
    volatile char stat[18];

    stat[0] = '~';
    stat[1] = 'g';

    getbrk_wcz(INT_CAST(stat[2]));
    getbrk_wc(INT_CAST(stat[6]));
    getbrk_wz(INT_CAST(stat[10]));
    INT_CAST(stat[14]) = INB; 

    if (*((int*)&stat[2]) >> 23 & 1) {
        brk(DEBUG_BRK);
        cogret;
    }

    // 1. dump current data
    for (int i = 0; i < sizeof(stat); i++) {
        tx_char(stat[i]);
    }

    while (1) {
        // 2. process commands
        volatile char rx[6];

        rx_char(rx[0]); // read one byte. If not $, come back and try again
        if (rx[0] != '$') {
            continue;
        }

        // we've got a command, read the remaining 5 bytes
        for (int i = 1; i < 6; i++) {
            rx_char(rx[i]);
        }

        switch (rx[1]) {
            case 'b':
                brk(INT_CAST(rx[2]));
                cogret;
                break;

            case 'e':
                brk(0);
                cogret;
                break;

            case 'r': {
                break;
            }

            case 'h': {
                stat[1] = 'h';
                stat[2] = *(char*)INT_CAST(rx[2]);
                for (int i = 0; i < 3; i++) {
                    tx_char(stat[i]);
                }
                break;
            }
            
            default:
                tx_char('e');
                for (int i = 0; i < 6; i++){
                    tx_char(rx[i]);
                }
                
                break;
        }
    }
    */
}