.text
.globl alignment_with_offset
.type alignment_with_offset,@function
alignment_with_offset:
    wrlong r0, ptra++
    wrlong r2, ptra++
    mov r2, r0
    mov r0, r1
    add ptra, r2
    calla #\aligned_locals
    sub ptra, r2
    rdlong r2, --ptra
    rdlong r0, --ptra
    reta
