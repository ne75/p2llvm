' SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
' Derived from the LLVM compiler-rt helper modified for P2LLVM.
' Frozen pre-shift-fix __lshrdi3 at Oz (root 6782126, LLVM faebaf0abaaa).
' Preserve the old wide-division benchmark's dependency and save/return cost.
' Only its original nonzero normalization counts 1..32 are used here.
        .section lut,"ax",@progbits
        .globl udivmod_previous_shift
        .type udivmod_previous_shift,@function
udivmod_previous_shift:
        setq #3
        wrlong r0, ptra++
        cmp r2, #32 wc
if_nc   jmp #.Lold_shift_large
        mov r3, #32
        sub r3, r2
        mov r31, r1
        shr r31, r2
        mov r30, r0
        shr r30, r2
        shl r1, r3
        or r30, r1
        jmp #.Lold_shift_return
.Lold_shift_large:
        mov r3, r2
        sub r3, #32
        mov r31, #0
        mov r30, r1
        shr r30, r3
.Lold_shift_return:
        setq #3
        rdlong r0, --ptra
        reta
        .size udivmod_previous_shift, .-udivmod_previous_shift
