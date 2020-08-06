DAT
            org 0
            add sp, #4
            mov r0, sp
            sub r0, #4
            wrlong #0, r0
            mov r15, #0
            sub sp, #4
            ret

            org $1df

sp  long    0
r0  long    0
r1  long    0
r2  long    0
r3  long    0
r4  long    0
r5  long    0
r6  long    0
r7  long    0
r8  long    0
r9  long    0
r10 long    0
r11 long    0
r12 long    0
r13 long    0
r14 long    0
r15 long    0