/*************************************************
 *File----------user.c
 *Project-------RiscV-OS
 *Author--------Justin Kachele
 *Created-------Wednesday Nov 05, 2025 21:00:43 UTC
 *License-------GNU GPL-3.0
 ************************************************/

#include "user.h"

extern void start(void);

int syscall(int sysno, int arg0, int arg1, int arg2) {
        register long a0 asm("a0") = arg0;
        register long a1 asm("a1") = arg1;
        register long a2 asm("a2") = arg2;
        register long a3 asm("a3") = sysno;

        asm volatile(
                "ecall"
                : "=r"(a0)
                : "r"(a0), "r"(a1), "r"(a2), "r"(a3)
                : "memory"
        );
        return a0;
}

__attribute__((noreturn))
void exit(void) {
        syscall(SYS_EXIT, 0, 0, 0);
        for (;;); // Just in case :)
}

int getchar(void) {
        return syscall(SYS_GETCHAR, 0, 0, 0);
}

void putchar(char c) {
        syscall(SYS_PUTCHAR, c, 0, 0);
}

