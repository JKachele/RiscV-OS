.global kernel_entry
.section .text

.align 2
kernel_entry:
    # Get kernel stack of running process
    CSRRW sp, sscratch, sp

    ADDI sp, sp, -4 * 31
    SW ra,  4 * 0(sp)
    SW gp,  4 * 1(sp)
    SW tp,  4 * 2(sp)
    SW t0,  4 * 3(sp)
    SW t1,  4 * 4(sp)
    SW t2,  4 * 5(sp)
    SW t3,  4 * 6(sp)
    SW t4,  4 * 7(sp)
    SW t5,  4 * 8(sp)
    SW t6,  4 * 9(sp)
    SW a0,  4 * 10(sp)
    SW a1,  4 * 11(sp)
    SW a2,  4 * 12(sp)
    SW a3,  4 * 13(sp)
    SW a4,  4 * 14(sp)
    SW a5,  4 * 15(sp)
    SW a6,  4 * 16(sp)
    SW a7,  4 * 17(sp)
    SW s0,  4 * 18(sp)
    SW s1,  4 * 19(sp)
    SW s2,  4 * 20(sp)
    SW s3,  4 * 21(sp)
    SW s4,  4 * 22(sp)
    SW s5,  4 * 23(sp)
    SW s6,  4 * 24(sp)
    SW s7,  4 * 25(sp)
    SW s8,  4 * 26(sp)
    SW s9,  4 * 27(sp)
    SW s10, 4 * 28(sp)
    SW s11, 4 * 29(sp)
    
    # Retrieve and save sp at time of execution
    CSRR a0, sscratch
    SW a0, 4 * 30(sp)

    # Reset kernel stack
    ADDI a0, sp, 4 * 31
    CSRW sscratch, a0
    
    MV a0, sp
    CALL handle_trap
    
    LW ra,  4 * 0(sp)
    LW gp,  4 * 1(sp)
    LW tp,  4 * 2(sp)
    LW t0,  4 * 3(sp)
    LW t1,  4 * 4(sp)
    LW t2,  4 * 5(sp)
    LW t3,  4 * 6(sp)
    LW t4,  4 * 7(sp)
    LW t5,  4 * 8(sp)
    LW t6,  4 * 9(sp)
    LW a0,  4 * 10(sp)
    LW a1,  4 * 11(sp)
    LW a2,  4 * 12(sp)
    LW a3,  4 * 13(sp)
    LW a4,  4 * 14(sp)
    LW a5,  4 * 15(sp)
    LW a6,  4 * 16(sp)
    LW a7,  4 * 17(sp)
    LW s0,  4 * 18(sp)
    LW s1,  4 * 19(sp)
    LW s2,  4 * 20(sp)
    LW s3,  4 * 21(sp)
    LW s4,  4 * 22(sp)
    LW s5,  4 * 23(sp)
    LW s6,  4 * 24(sp)
    LW s7,  4 * 25(sp)
    LW s8,  4 * 26(sp)
    LW s9,  4 * 27(sp)
    LW s10, 4 * 28(sp)
    LW s11, 4 * 29(sp)
    LW sp,  4 * 30(sp)
    SRET
