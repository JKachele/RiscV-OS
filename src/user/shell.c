/*************************************************
 *File----------shell.c
 *Project-------RiscV-OS
 *Author--------Justin Kachele
 *Created-------Thursday Nov 06, 2025 17:31:30 UTC
 *License-------GNU GPL-3.0
 ************************************************/

#include "user.h"

void main(void) {
        while (1) {
prompt:
                printf("> ");
                char cmdline[128];
                for (int i = 0;; i++) {
                        char ch = getchar();
                        putchar(ch);
                        if (i == sizeof(cmdline) - 1) {
                                printf("Command too long\n");
                                goto prompt;
                        } else if (ch == '\r') {
                                printf("\n");
                                cmdline[i] = '\0';
                                break;
                        } else {
                                cmdline[i] = ch;
                        }
                }

                if (strcmp(cmdline, "hello") == 0)
                        printf("Hello world from the shell!\n");
                else if (strcmp(cmdline, "exit") == 0)
                        exit();
                else
                        printf("Unknown command: %s\n", cmdline);
        }
}


