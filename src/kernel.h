/*************************************************
 *File----------kernel.h
 *Project-------os1000
 *Author--------Justin Kachele
 *Created-------Tuesday Nov 04, 2025 13:03:23 UTC
 *License-------GNU GPL-3.0
 ************************************************/
#ifndef KERNEL_H
#define KERNEL_H

#include "common.h"

#define PROCS_MAX 8
#define PROC_UNUSED 0
#define PROC_RUNNABLE 1

struct process {
        int pid;        // Process ID
        int state;      // Process State: PROC_UNUSED or PROC_RUNNABLE
        vaddr_t sp;     // Stack pointer
        u8 stack[8192]; // Kernel stack
};

struct trap_frame {
         u32 ra;
         u32 gp;
         u32 tp;
         u32 t0;
         u32 t1;
         u32 t2;
         u32 t3;
         u32 t4;
         u32 t5;
         u32 t6;
         u32 a0;
         u32 a1;
         u32 a2;
         u32 a3;
         u32 a4;
         u32 a5;
         u32 a6;
         u32 a7;
         u32 s0;
         u32 s1;
         u32 s2;
         u32 s3;
         u32 s4;
         u32 s5;
         u32 s6;
         u32 s7;
         u32 s8;
         u32 s9;
         u32 s10;
         u32 s11;
         u32 sp;
} __attribute((packed));

struct sbiret {
    long error;
    long value;
};


#define PANIC(fmt, ...)                                                 \
        do {                                                            \
                printf("PANIC: %s:%d " fmt "\n",                        \
                                __FILE__, __LINE__, ##__VA_ARGS__);     \
                while(1) {}                                             \
        }while (0)

#define READ_CSR(reg)                                                   \
        ({                                                              \
                unsigned long __tmp;                                    \
                asm("csrr %0, " #reg : "=r"(__tmp));                    \
                __tmp;                                                  \
        })

#define WRITE_CSR(reg, value)                                           \
        do {                                                            \
                u32 __tmp = (value);                                    \
                asm("csrw " #reg ", %0" ::"r"(__tmp));                  \
        } while(0)

#endif

