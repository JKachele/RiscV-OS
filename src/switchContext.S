.global switch_context
.section .text

switch_context:
    ADDI sp, sp, -13 * 4
    SW ra, 0 * 4(sp)
    SW s0, 1 * 4(sp)
    SW s1, 2 * 4(sp)
    SW s2, 3 * 4(sp)
    SW s3, 4 * 4(sp)
    SW s4, 5 * 4(sp)
    SW s5, 6 * 4(sp)
    SW s6, 7 * 4(sp)
    SW s7, 8 * 4(sp)
    SW s8, 9 * 4(sp)
    SW s9, 10 * 4(sp)
    SW s10, 11 * 4(sp)
    SW s11, 12 * 4(sp)

    SW sp, (a0)
    lw sp, (a1)

    LW ra, 0 * 4(sp)
    LW s0, 1 * 4(sp)
    LW s1, 2 * 4(sp)
    LW s2, 3 * 4(sp)
    LW s3, 4 * 4(sp)
    LW s4, 5 * 4(sp)
    LW s5, 6 * 4(sp)
    LW s6, 7 * 4(sp)
    LW s7, 8 * 4(sp)
    LW s8, 9 * 4(sp)
    LW s9, 10 * 4(sp)
    LW s10, 11 * 4(sp)
    LW s11, 12 * 4(sp)
    ADDI sp, sp, 13 * 4
    RET
