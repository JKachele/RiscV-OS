/*************************************************
 *File----------kernel.c
 *Project-------RiscV
 *Author--------Justin Kachele
 *Created-------Wednesday Oct 29, 2025 10:57:03 UTC
*License-------GNU GPL-3.0
 ************************************************/
#include "kernel.h"
#include "common.h"

extern char __bss[];
extern char __bss_end[];
extern char __free_ram[];
extern char __free_ram_end[];
extern void kernel_entry(void);
extern void switch_context(u32 *prev_sp, u32 *next_sp);

struct process procs[PROCS_MAX];
struct process *current_proc;
struct process *idle_proc;

paddr_t alloc_pages(u32 n) {
        static paddr_t next_paddr = (paddr_t)__free_ram;
        paddr_t paddr = next_paddr;
        next_paddr += n * PAGE_SIZE;

        if (next_paddr > (paddr_t)__free_ram_end)
                PANIC("out of memory");

        memset((void*) paddr, 0, n * PAGE_SIZE);
        return paddr;
}

struct sbiret sbi_call(long arg0, long arg1, long arg2, long arg3, long arg4,
                long arg5, long fid, long eid) {
        register long a0 asm("a0") = arg0;
        register long a1 asm("a1") = arg1;
        register long a2 asm("a2") = arg2;
        register long a3 asm("a3") = arg3;
        register long a4 asm("a4") = arg4;
        register long a5 asm("a5") = arg5;
        register long a6 asm("a6") = fid;
        register long a7 asm("a7") = eid;

        asm(
                "ecall"
                : "=r"(a0), "=r"(a1)
                : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4),
                        "r"(a5), "r"(a6), "r"(a7)
                : "memory"
        );

        return (struct sbiret) {.error = a0, .value = a1 };
}

void putchar(char c) {
        sbi_call(c, 0, 0, 0, 0, 0, 0, 1);
}

struct process* createProcess(u32 pc) {
        struct process *proc = NULL;
        int i;
        for (i = 0; i < PROCS_MAX; i++) {
                if (procs[i].state == PROC_UNUSED) {
                        proc = &procs[i];
                        break;
                }
        }

        if (!proc)
                PANIC("No available process slots");

        // Stack callee-saved registers
        // Registers restored in context switch
        u32 *sp = (u32*)&proc->stack[sizeof(proc->stack)];
        *--sp = 0;      // s11
        *--sp = 0;      // s10
        *--sp = 0;      // s9
        *--sp = 0;      // s8
        *--sp = 0;      // s7
        *--sp = 0;      // s6
        *--sp = 0;      // s5
        *--sp = 0;      // s4
        *--sp = 0;      // s3
        *--sp = 0;      // s2
        *--sp = 0;      // s1
        *--sp = 0;      // s0
        *--sp = (u32)pc;

        // Initalize fields
        proc->pid = i + 1;
        proc->state = PROC_RUNNABLE;
        proc->sp = (u32)sp;
        return proc;
}

// Give CPU to another process
void yield(void) {
        // Search for runnable process
        struct process *next = idle_proc;
        for (int i = 0; i < PROCS_MAX; i++) {
                int proc_index = (current_proc->pid + i) % PROCS_MAX;
                struct process *proc = &procs[proc_index];
                if (proc->state == PROC_RUNNABLE && proc->pid > 0) {
                        next = proc;break;
                }
        }

        // If no other process is runnable, continue with current process
        if (next == current_proc)
                return;

        // Save stack pointer of current process
        asm(
                "csrw sscratch, %[sscratch]\n"
                :
                : [sscratch] "r" ((u32)&next->stack[sizeof(next->stack)])
        );

        // Context switch
        struct process *prev = current_proc;
        current_proc = next;
        switch_context(&prev->sp, &next->sp);
}

void handle_trap(struct trap_frame *f) {
        u32 scause = READ_CSR(scause);
        u32 stval = READ_CSR(stval);
        u32 sepc = READ_CSR(sepc);

        PANIC("Unexpected trap scause=%x, stval=%x, sepc=%x\n",
                        scause, stval, sepc);
}

void delay(void) {
        for (int i = 0; i < 30000000; i++)
                asm("nop");
}

struct process *proc_a;
struct process *proc_b;

void proc_a_entry(void) {
        printf("starting Process A\n");
        while (1) {
                putchar('A');
                yield();
                delay();
        }
}

void proc_b_entry(void) {
        printf("starting Process B\n");
        while (1) {
                putchar('B');
                yield();
                delay();
        }
}

void kernel_main(void) {
        memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);
        WRITE_CSR(stvec, (u32) kernel_entry);
        printf("\n\nhello, %s\n", "world");

        // Create idle process
        idle_proc = createProcess((u32)NULL);
        idle_proc->pid = 0;
        current_proc = idle_proc;

        proc_a = createProcess((u32)proc_a_entry);
        proc_b = createProcess((u32)proc_b_entry);

        yield();
        PANIC("BOOTED!");
}

