	.text
	.file	"test5.cpp"
	.section	.text._Z3sumiiiii,"ax",@progbits
	.globl	_Z3sumiiiii
	.type	_Z3sumiiiii,@function
_Z3sumiiiii:
			add sp, #20
			mov r4, sp
			sub r4, #20
			rdlong r5, r4
			mov r6, sp
			sub r6, #16
			wrlong r0, r6
			mov r0, sp
			sub r0, #12
			wrlong r1, r0
			mov r1, sp
			sub r1, #8
			wrlong r2, r1
			mov r2, sp
			sub r2, #4
			wrlong r3, r2
			rdlong r3, r6
			rdlong r0, r0
			add r3, r0
			rdlong r0, r1
			add r3, r0
			rdlong r0, r2
			add r3, r0
			rdlong r0, r4
			add r3, r0
			mov r15, r3
			sub sp, #20
			ret
.Lfunc_end0:
	.size	_Z3sumiiiii, .Lfunc_end0-_Z3sumiiiii

	.section	.text.main,"ax",@progbits
	.globl	main
	.type	main,@function
main:
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
			call #_Z3sumiiiii
			sub sp, #4
			mov r0, sp
			sub r0, #4
			wrlong r15, r0
			rdlong r15, r0
			sub sp, #8
			ret
.Lfunc_end1:
	.size	main, .Lfunc_end1-main

	.ident	"clang version 11.0.0 (https://github.com/ne75/llvm-propeller2.git bec8e00fb7c08cc0cdaa1eefb16d144ebeb868a6)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym _Z3sumiiiii
