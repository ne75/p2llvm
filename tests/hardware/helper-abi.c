// Exercise each helper with live values in every callee-saved register.
// Snapshots are taken in assembly before C can reload or repair a register.
volatile unsigned helper_before[30], helper_after[30];
volatile unsigned helper_sp_before, helper_sp_after;
const unsigned helper_canaries[30] = {
    0x6a000000, 0x6a000101, 0x6a000202, 0x6a000303, 0x6a000404,
    0x6a000505, 0x6a000606, 0x6a000707, 0x6a000808, 0x6a000909,
    0x6a000a0a, 0x6a000b0b, 0x6a000c0c, 0x6a000d0d, 0x6a000e0e,
    0x6a000f0f, 0x6a001010, 0x6a001111, 0x6a001212, 0x6a001313,
    0x6a001414, 0x6a001515, 0x6a001616, 0x6a001717, 0x6a001818,
    0x6a001919, 0x6a001a1a, 0x6a001b1b, 0x6a001c1c, 0x6a001d1d
};
#if defined(P2_DIVSI)
#define CALLEE "__divsi3"
#define ARGUMENTS "setq #1\n"
__attribute__((naked)) int helper_probe(int a, int b)
#elif defined(P2_MULDI)
#define CALLEE "__muldi3"
#define ARGUMENTS "setq #3\n"
__attribute__((naked)) unsigned long long helper_probe(unsigned long long a, unsigned long long b)
#elif defined(P2_UDIVMODDI)
#define CALLEE "__udivmoddi4"
#define ARGUMENTS "setq #3\n"
__attribute__((naked)) unsigned long long helper_probe(unsigned long long a, unsigned long long b, unsigned long long *r)
#else
#error Select a runtime helper
#endif
{
    __asm__ volatile(
        "setq #29\nwrlong r0, ptra++\n" // Save the probe caller's R0..R29.
        "mov pa, ##helper_canaries\nsetq #29\nrdlong r0, pa\n"
        "mov pa, ptra\nsub pa, #120\n" // Restore actual argument words.
        ARGUMENTS "rdlong r0, pa\n"
#ifdef P2_UDIVMODDI
        // The fifth word is stack-passed: original PTRA-8, now PTRA-128.
        "mov pa, ptra\nsub pa, #128\nrdlong pa, pa\nwrlong pa, ptra++\n"
#endif
        "mov pa, ##helper_before\nsetq #29\nwrlong r0, pa\n"
        "wrlong ptra, ##helper_sp_before\n"
        "calla #\\" CALLEE "\n"
        "wrlong ptra, ##helper_sp_after\n"
        "mov pa, ##helper_after\nsetq #29\nwrlong r0, pa\n"
#ifdef P2_UDIVMODDI
        "sub ptra, #4\n" // Remove the forwarded remainder pointer.
#endif
        "setq #29\nrdlong r0, --ptra\nreta\n");
}

unsigned helper_bad_registers(void) {
    unsigned bad = 0;
    for (unsigned i = 0; i < 30; ++i)
        if (helper_before[i] != helper_after[i]) bad |= 1u << i;
    if (helper_sp_before != helper_sp_after) bad |= 0x80000000u;
    return bad;
}
