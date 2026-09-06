# RUN: %mc -filetype=obj %s -o %t.o
# RUN: %bin/ld.lld -T %root/tests/semantics/layout.ld %t.o -o %t.elf
# RUN: %python %tools/check_linked_semantics.py %t.elf
# The identical object is also linked into the hardware semantics firmware.
.text
.globl asm_address_add, asm_address_sub, asm_both_fields
.globl asm_return_aug, asm_negative_branch, asm_setnib, asm_testb_wz
.globl asm_call_sqrt, asm_call_lut, sqrt
.type asm_address_add,@function
asm_address_add:
    mov r31, ##probe_data+528
    rdbyte r31, r31
    reta
.type asm_address_sub,@function
asm_address_sub:
    mov r31, ##probe_data-1
    rdbyte r31, r31
    reta
.type asm_both_fields,@function
asm_both_fields:
    wrlong ##probe_data+528, ##probe_slot
    rdlong r31, ##probe_slot
    rdbyte r31, r31
    reta
.type asm_return_aug,@function
asm_return_aug:
    call #.Lreturn_aug
    reta
.Lreturn_aug:
    _ret_ mov r31, ##1000
.type asm_negative_branch,@function
asm_negative_branch:
    mov r31, ##-1
    add r31, #1
    tjz r31, #-2
    reta
.type asm_setnib,@function
asm_setnib:
    mov r31, ##0x12345678
    setnib r31, ##1000, #7
    reta
.type asm_testb_wz,@function
asm_testb_wz:
    mov r31, #1
    cmp r31, #0 wcz
    testb r31, #0 wz
    wrz r31
    reta
.type asm_call_sqrt,@function
asm_call_sqrt:
    calla #\sqrt
    reta
.type sqrt,@function
sqrt:
    mov r31, #77
    reta
.type asm_call_lut,@function
asm_call_lut:
    calla #\lut_probe
    reta
.section lut,"ax",@progbits
.globl lut_probe
.type lut_probe,@function
lut_probe:
    mov r31, #91
    reta
.data
.balign 512
    .space 511
    .byte 0x5a
.globl probe_data, probe_slot, probe_delta32, probe_delta64
probe_data:
    .space 528
    .byte 0x33
.balign 4
probe_slot:
    .long 0
.Ldelta_start:
    .space 8
.Ldelta_end:
probe_delta32:
    .long .Ldelta_end-.Ldelta_start
probe_delta64:
    .quad (.Ldelta_end-.Ldelta_start)*0x100000001
