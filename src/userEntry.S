#include "defines.h"
.global user_entry
.section .text

user_entry:
    li t0, USER_BASE
    csrw sepc, t0
    li t0, SSTATUS_SPIE
    csrw sstatus, t0
    sret
