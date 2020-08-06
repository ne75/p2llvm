	.text
	.file	"test6.cpp"
	.globl	_Z5blinkv               '' -- Begin function _Z5blinkv
	.type	_Z5blinkv,@function
_Z5blinkv:                              '' @_Z5blinkv
'' %bb.0:                               '' %entry
	''APP
			outh #56
	''NO_APP
	''APP
			augd #39062
	''NO_APP
	''APP
			waitx #256
	''NO_APP
	''APP
			outl #56
	''NO_APP
	''APP
			augd #39062
	''NO_APP
	''APP
			waitx #256
	''NO_APP
			ret
.Lfunc_end0:
	.size	_Z5blinkv, .Lfunc_end0-_Z5blinkv
                                        '' -- End function
	.globl	main                    '' -- Begin function main
	.type	main,@function
main:                                   '' @main
'' %bb.0:                               '' %entry
			add sp, #4
			mov r0, sp
			sub r0, #4
			wrlong #0, r0
	''APP
			dirh #56
	''NO_APP
.LBB1_1:                                '' %while.body
                                        '' =>This Inner Loop Header: Depth=1
			add sp, #0
			call #_Z5blinkv
			add sp, #0
			jmp #.LBB1_1
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
                                        '' -- End function
	.ident	"clang version 11.0.0 (https://github.com/ne75/llvm-propeller2.git bec8e00fb7c08cc0cdaa1eefb16d144ebeb868a6)"
	.section	".note.GNU-stack","",@progbits
