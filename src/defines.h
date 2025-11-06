/*************************************************
 *File----------defines.h
 *Project-------RiscV-OS
 *Author--------Justin Kachele
 *Created-------Thursday Nov 06, 2025 21:57:56 UTC
 *License-------GNU GPL-3.0
 ************************************************/
#ifndef DEFINES_H
#define DEFINES_H

#define PROCS_MAX 8
#define PROC_UNUSED   0
#define PROC_RUNNABLE 1
#define PROC_EXITED   2
#define SATP_SV32 (1u << 31)
#define SSTATUS_SPIE (1 << 5)
#define SCAUSE_ECALL 8
#define PAGE_V (1 << 0)
#define PAGE_R (1 << 1)
#define PAGE_W (1 << 2)
#define PAGE_X (1 << 3)
#define PAGE_U (1 << 4)
#define USER_BASE 0x1000000

#endif

