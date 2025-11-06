/*************************************************
 *File----------user.h
 *Project-------RiscV-OS
 *Author--------Justin Kachele
 *Created-------Wednesday Nov 05, 2025 21:00:53 UTC
 *License-------GNU GPL-3.0
 ************************************************/
#ifndef USER_H
#define USER_H

#include "../common.h"

__attribute__((noreturn)) void exit(void);
void putchar(char c);
int getchar(void);

#endif

