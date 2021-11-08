#include <propeller.h>

/**
 * BRK takes %aaaaaaaaaaaaaaaaeeee_LKJIHGFEDCBA
 * 
 * a is an address, e is event code, the rest are options as seen below
 * 
 * K: 1 = enable interrupt on breakpoint address match
 * J: 1 = enable interrupt on event %eeee
 * I: 1 = enable interrupt on asynchronous breakpoint (via COGBRK on another cog)
 * H: 1 = enable interrupt on INT3 ISR entry
 * G: 1 = enable interrupt on INT2 ISR entry
 * F: 1 = enable interrupt on INT1 ISR entry
 * E: 1 = enable interrupt on BRK instruction
 * D: 1 = enable interrupt on INT3 ISR code (single step)
 * C: 1 = enable interrupt on INT2 ISR code (single step)
 * B: 1 = enable interrupt on INT1 ISR code (single step)
 * A: 1 = enable interrupt on non-ISR code (single step)
*/

#define DEBUG_SINGLE        (1 << 0)
#define DEBUG_INT1_SNGLE    (1 << 1)
#define DEBUG_INT2_SNGLE    (1 << 2)
#define DEBUG_INT3_SNGLE    (1 << 3)
#define DEBUG_BRK           (1 << 4)
#define DEBUG_INT1_ENTRY    (1 << 5)
#define DEBUG_INT2_ENTRY    (1 << 6)
#define DEBUG_INT3_ENTRY    (1 << 7)
#define DEBUG_ASYNC         (1 << 8)
#define DEBUG_EVENT         (1 << 9)
#define DEBUG_ADDR          (1 << 10)

#define DEBUG_CODE_INIT     0xff    // Initial brk to enter debugging mode