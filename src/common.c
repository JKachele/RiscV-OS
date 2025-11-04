/*************************************************
 *File----------common.c
 *Project-------os1000
 *Author--------Justin Kachele
 *Created-------Tuesday Nov 04, 2025 13:33:49 UTC
 *License-------GNU GPL-3.0
 ************************************************/

#include "common.h"

void putchar(char c);

void* memset(void *buf, char c, size_t n) {
        u8 *p = (u8*)buf;
        while (n--)
                *p++ = c;
        return buf;
}

void *memcpy(void *dst, const void *src, size_t n) {
        u8 *d = (u8 *) dst;
        const u8 *s = (const u8 *) src;
        while (n--)
                *d++ = *s++;
        return dst;
}

char *strcpy(char *dst, const char *src) {
        char *d = dst;
        while (*src)
                *d++ = *src++;
        *d = '\0';
        return dst;
}

int strcmp(const char *s1, const char *s2) {
        while (*s1 && *s2) {
                if (*s1 != *s2)
                        break;
                s1++;
                s2++;
        }

        return *(unsigned char *)s1 - *(unsigned char *)s2;
}

void printf(const char *fmt, ...) {
        // Get arguments
        va_list vargs;
        va_start(vargs, fmt);

        // Format string
        while (*fmt) {
                if (*fmt != '%') {
                        putchar(*fmt);
                        fmt++;
                        continue;
                }

                fmt++;  // Skip '%' format specifier
                switch (*fmt) {
                        case '\0':      // '%' at end of format string
                                putchar('%');
                                goto end;
                        case 's': {     // Print a NULL-terminated string
                                const char *s = va_arg(vargs, const char *);
                                while (*s) {
                                        putchar(*s);
                                        s++;
                                }
                                break;
                        }
                        case 'd': {     // Print a integer in decimal
                                int value = va_arg(vargs, int);
                                unsigned magnitude = value;
                                if (value < 0) {
                                        putchar('-');
                                        magnitude = -magnitude;
                                }

                                unsigned divisor = 1;
                                while (magnitude / divisor > 9)
                                        divisor *= 10;

                                while (divisor > 0) {
                                        putchar('0' + magnitude / divisor);
                                        magnitude %= divisor;
                                        divisor /= 10;
                                }

                                break;
                        }
                        case 'x': {     // Print a integer in hexadecimal
                                unsigned value = va_arg(vargs, unsigned);
                                for (int i = 7; i >= 0; i--) {
                                        unsigned nibble = (value >> (i * 4)) & 0xf;
                                        putchar("0123456789abcdef"[nibble]);
                                }
                        }
                }
                fmt++;
        }
end:
        va_end(vargs);
}

