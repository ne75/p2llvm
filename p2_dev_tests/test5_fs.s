DAT

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

            orgh $800

_Z3sumiiiii
			add sp, #20
			mov r4, sp
			sub r4, #20
			rdlong r5, r4
			mov r5, sp
			sub r5, #16
			wrlong r0, r5
			mov r0, sp
			sub r0, #12
			wrlong r1, r0
			mov r1, sp
			sub r1, #8
			wrlong r2, r1
			mov r2, sp
			sub r2, #4
			wrlong r3, r2
			rdlong r15, r5
			rdlong r0, r0
			add r15, r0
			rdlong r0, r1
			add r15, r0
			rdlong r0, r2
			add r15, r0
			rdlong r0, r4
			add r15, r0
			sub sp, #20
			ret
main
			add sp, #8
			mov r0, sp
			sub r0, #8
			wrlong #0, r0
			add sp, #4
			mov r0, sp
			sub r0, #4
			wrlong #5, r0
			mov r0, #1
			mov r1, #2
			mov r2, #3
			mov r3, #4
			call #\_Z3sumiiiii
			sub sp, #4
			mov r0, sp
			sub r0, #4
			wrlong r15, r0
			rdlong r15, r0
			sub sp, #8
			ret
