/*************************************************
 *File----------common.h
 *Project-------os1000
 *Author--------Justin Kachele
 *Created-------Tuesday Nov 04, 2025 13:33:54 UTC
 *License-------GNU GPL-3.0
 ************************************************/
#ifndef COMMON_H
#define COMMON_H

#define true  1
#define false 0
#define NULL ((void*)0)
#define va_list  __builtin_va_list
#define va_start __builtin_va_start
#define va_end   __builtin_va_end
#define va_arg   __builtin_va_arg

#define PAGE_SIZE 4096

typedef unsigned char uint8_t;
typedef uint8_t u8;
typedef unsigned short uint16_t;
typedef uint16_t u16;
typedef unsigned int uint32_t;
typedef uint32_t u32;
typedef unsigned long long uint64_t;
typedef uint64_t u64;
typedef uint32_t size_t;
typedef uint32_t paddr_t;
typedef uint32_t vaddr_t;


void *memset(void *buf, char c, size_t n);
void *memcpy(void *dst, const void *src, size_t n);
char *strcpy(char *dst, const char *src);
int   strcmp(const char *s1, const char *s2);
void  printf(const char *fmt, ...);

#endif

