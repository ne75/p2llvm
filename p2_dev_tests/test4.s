	.text
	.file	"test4.cpp"
	.section	.text.main,"ax",@progbits
	.globl	main
	.type	main,@function
main:
			add sp, #8
			mov r0, sp
			sub r0, #8
			wrlong #0, r0
			mov r0, sp
			sub r0, #4
			wrbyte #0, r0
			rdbyte r0, r0
			not r0, r0
			and r0, #1
			mov r1, sp
			sub r1, #3
			wrbyte r0, r1
			mov r15, #0
			sub sp, #8
			ret
.Lfunc_end0:
	.size	main, .Lfunc_end0-main

	.ident	"clang version 11.0.0 (https://github.com/ne75/llvm-propeller2.git 30af30c1e3459932c3229db64deb13befa6acc7f)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
