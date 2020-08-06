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
main                                   '' @main
'' %bb.0:                               '' %entry
			add sp, #8
			mov r0, sp
			sub r0, #8
			wrlong #0, r0
			mov r0, sp
			sub r0, #4
			wrlong #3, r0
			rdlong r0, r0
			cmp r0, #3	wcz
	if_nz	jmp #.LBB0_2
'' %bb.1:                               '' %if.then
			mov r0, sp
			sub r0, #4
			wrlong #1, r0
.LBB0_2                                '' %if.end
			mov r0, sp
			sub r0, #8
			rdlong r15, r0
			sub sp, #8
			ret

