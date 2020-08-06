	.text
	.file	"test3.cpp"
	.globl	main                    '' -- Begin function main
	.type	main,@function
main:                                   '' @main
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
.LBB0_2:                                '' %if.end
			mov r0, sp
			sub r0, #8
			rdlong r15, r0
			sub sp, #8
			ret
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
                                        '' -- End function
	.ident	"clang version 11.0.0 (https://github.com/ne75/llvm-propeller2.git d1c3cdbf8cc823d3a4eaf073edc830b45da44b64)"
	.section	".note.GNU-stack","",@progbits
