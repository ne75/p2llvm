# P2 inline assembly operands

Use extended assembly operands to tell the compiler which values an assembly
block reads and writes. The compiler then owns register allocation, callee-save
handling, stack arguments and the C return sequence.

The `r` constraint accepts 32-bit integers and pointers in a single register,
and 64-bit integers in an even/odd register pair. Use `%L0` and `%H0` to name the
low and high words of a 64-bit operand; named operands use `%L[value]` and
`%H[value]`. Word modifiers on a single-register operand are rejected.

```c
unsigned long long add_words(unsigned long long a, unsigned long long b) {
    __asm__("add %L0, %L1 wc\naddx %H0, %H1"
            : "+&r"(a) : "r"(b) : "cc");
    return a;
}
```

`+` declares an operand read and written. `&` prevents that output from sharing
registers with other inputs still needed after the first write. Declare scratch
registers and condition flags as clobbers. Use a `memory` clobber when the block
reads or writes memory not described by its operands.

When an ABI return register is required, bind a 32-bit value to `r31`, or a
64-bit value to the explicit pair `r30_r31`:

```c
unsigned long long read_counter(void) {
    register unsigned long long result __asm__("r30_r31");
    __asm__ volatile("getct %H0 wc\ngetct %L0" : "=r"(result));
    return result;
}
```

Other fixed pairs are named `r0_r1`, `r2_r3`, through `r28_r29`. Binding a 64-bit
operand to one register, such as `r30`, is rejected rather than discarding its
high word. Pair names are compiler operands; use the word modifiers inside
P2 instructions. Give local labels a `%=` suffix if an asm block can be cloned.
