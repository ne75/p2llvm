#include <propeller.h>
#include <debug.h>
#include <stdio.h>

/**
 * The debug app
 * 
 * written entirely in asm for space concerns and prevent the optimizer from doing weird stuff
 * 
 * hub address 0xfe400..0xfe41f will be used for the stat array
 * hub address 0xfe420..0xfe418 will be used for the 6 byte rx array
 * 
 */
__attribute__ ((cogmain, noreturn)) void __dbg_run() {
    asm(    
            // r0 will hold the address of stat.
            "augs #0x7f2\n"      
            "mov $r0, #0\n"    

            // r1 will hold the address of rx.     
            "augs #0x7f2\n"     
            "mov $r1, #0x20\n"

            // get coginit status bit
            "getbrk $r3 wcz\n"
            "shr $r3, #23\n"        
            "and $r3, #1\n"

            // if this is a coginit interrupt, set up for a brk interrupt and return 
            "tjz $r3, #.Lenter\n"
            "brk %0\n"
            "ret\n" 

    ".Lenter:"

    ".Llock:"
            "locktry %4 wc\n"
    "if_nc   jmp #.Llock\n"

            // set up by writing '~' and 'g' to our stat array
            "mov $r2, $r0\n"
            "wrbyte #0x7e, $r2\n"   
            "add $r2, #1\n"
            "wrbyte #0x67, $r2\n"

            // get our cogid
            "cogid $r6\n"
            "add $r2, #1\n"
            "wrbyte $r6, $r2\n"

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

    ".Lstat_dump:"
            // dump current data         
            "mov $r3, #19\n"  
            "call #.Ltx_bytes\n"

            // map ina/inb to pins and not interrupts regs
            "brk %3\n"

            // main loop. we stay here forever or return from the interrupt
    ".Lmain_loop:\n"

            // check if there's already data waiting. if there is, jump to process it
            "rdbyte $r2, $r1\n"
            "cmp $r2, #0x24 wz\n"
    "if_z   jmp #.Lprocess_cmd\n"

    ".Lrx_begin:"
            // get a byte, if we didn't get a '$', jump back to the start of the routine
            "call #.Lser_rx\n"
            "cmp $r31, #0x24 wz\n"      
    "if_nz  jmp #.Lrx_begin\n"        
            
            // read in 6 bytes
            "mov $r2, $r1\n"    
            "wrbyte #0x24, $r2\n" // write the $ first to mark that we have a command
            "add $r2, #1\n"

            "mov $r3, #6\n"
    ".Lrx_cmd:"
            "call #.Lser_rx\n"
            "wrbyte $r31, $r2\n"
            "add $r2, #1\n"
            "djnz $r3, #.Lrx_cmd\n"

    ".Lprocess_cmd:"
            // get the command byte
            "mov $r2, $r1\n"
            "add $r2, #1\n"
            "rdbyte $r3, $r2\n"
            "add $r2, #1\n"

            // get the cogid byte and check if the command is for us
            "rdbyte $r4, $r2\n"
            "add $r2, #1\n"
            "cmp $r4, $r6 wz\n"
    "if_z   jmp #.Lcmd_switch\n"
            // command is not for us, release the lock so another cog can begin processing,
            // then, go back to try to re-acquire the lock, but we need to skip sending the status, 
            // since that should only be sent on isr entry
            "brk #0\n"
            "lockrel %4\n"
            "jmp #.Llock\n"

    ".Lcmd_switch:"
            // switch statement
            "cmp $r3, #0x68 wz\n"   // h
    "if_z   jmp #.Lcase_h\n"
            "cmp $r3, #0x72 wz\n"   // r
    "if_z   jmp #.Lcase_r\n"
            "cmp $r3, #0x62 wz\n"   // b
    "if_z   jmp #.Lcase_b\n"

            "brk #0\n"          // default
            "jmp #.Lret\n"

    // h command case
    ".Lcase_h:"
            "mov $r4, $r0\n"
            "add $r4, #1\n"
            "wrbyte #0x68, $r4\n"
            "add $r4, #2\n"
            "rdlong $r5, $r2\n"   // r5 is the address we want to get 
            "rdbyte $r5, $r5\n"   // r5 is the value we wanted to get
            "wrbyte $r5, $r4\n"

            // transmite 4 bytes from stat         
            "mov $r3, #4\n" 
            "call #.Ltx_bytes\n"

            // clear the command
            "mov $r2, $r1\n"
            "wrbyte #0, $r2\n"
            "jmp #.Lmain_loop\n"

    // r command case
    ".Lcase_r:"
            "mov $r4, $r0\n"
            "add $r4, #1\n"
            "wrbyte #0x72, $r4\n"
            "add $r4, #2\n"
            "rdlong $r5, $r2\n"     // r5 is the register address we want
            "alts $r5, #0\n"        // next instruction's s field will be the value in r5, i.e the register we want to read
            "mov $r5, $0\n"         // r5 is the value we wanted to get
            "wrlong $r5, $r4\n"

            // transmit 7 bytes from stat         
            "mov $r3, #7\n" 
            "call #.Ltx_bytes\n"

            // clear the command
            "mov $r2, $r1\n"
            "wrbyte #0, $r2\n"
            "jmp #.Lmain_loop\n" 

    // b command case
    ".Lcase_b:"
            "rdlong $r5, $r2\n"   // r5 is the brk value 
            "brk $r5\n"
            // nothing to transmit, exit the debug isr

            // clear the command
            "mov $r2, $r1\n"
            "wrbyte #0, $r2\n"
            "jmp #.Lret\n" 

    ".Lret:"
            "lockrel %4\n"
            "ret\n" 

    /**
     * tx bytes from stat. r3 should contain the number of bytes to send. uses r2 and r30
     */     
    ".Ltx_bytes:"
            "mov $r2, $r0\n"
    ".Ltx_loop:"
            "rdbyte $r30, $r2\n"
            "call #.Lser_tx\n"
            "add $r2, #1\n"
            "djnz $r3, #.Ltx_loop\n"
            "ret\n" 

    /**
     * tx_ser subroutine. place byte to tx in r10
     */
    ".Lser_tx:" 
            "wypin $r30, %1\n"   
            "waitx #20\n"  
    ".Ltxflush:"
            "testp %1 wc\n"
    "if_nc	jmp	#.Ltxflush\n"
            "ret\n"

    /**
     * rx_ser subroutine. read byte, blocks until one is received. 
     * byte is stored in r31
     */ 
    ".Lser_rx:" 
            "testp %2 wc\n"  
	"if_nc  jmp #.Lser_rx\n"   
  	        "rdpin $r31, %2\n"
	        "shr $r31, #24\n"
            "ret\n"

        : // outputs
        : "i"(DEBUG_BRK), "r"(_uart_tx_pin), "r"(_uart_rx_pin), "r"(DEBUG_MAP_INx), "r"(_dbg_lock) // inputs
        : "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r30", "r31" // clobber
    );
}