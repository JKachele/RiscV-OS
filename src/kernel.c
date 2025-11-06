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
extern char __kernel_base[];
extern void kernel_entry(void);
extern void switch_context(u32 *prev_sp, u32 *next_sp);

extern char _binary_shell_bin_start[];
extern char _binary_shell_bin_size[];
extern void user_entry(void);

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

void mapPage(u32 *table1, u32 vaddr, paddr_t paddr, u32 flags) {
        if (!is_aligned(vaddr, PAGE_SIZE))
                PANIC("unaligned vaddr %x", vaddr);

        if (!is_aligned(paddr, PAGE_SIZE))
                PANIC("unaligned paddr %x", paddr);

        u32 vpn1 = (vaddr >> 22) & 0x3ff;       // Get bits [31:22]
        if ((table1[vpn1] & PAGE_V) == 0) {
                // Create 1st level page table if doesn't exist
                u32 ptPaddr = alloc_pages(1);
                table1[vpn1] = ((ptPaddr / PAGE_SIZE) << 10) | PAGE_V;
        }

        // Set 2nd level page table entry
        u32 vpn0 = (vaddr >> 12) & 0x3ff;       // Get bits [21:12]
        u32 *table0 = (u32*)((table1[vpn1] >> 10) * PAGE_SIZE);
        table0[vpn0] = ((paddr / PAGE_SIZE) << 10) | flags | PAGE_V;
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

        asm volatile(
                "ecall"
                : "=r"(a0), "=r"(a1)
                : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4),
                        "r"(a5), "r"(a6), "r"(a7)
                : "memory"
        );

        return (struct sbiret) {.error = a0, .value = a1 };
}

long getchar(void) {
        struct sbiret ret = sbi_call(0, 0, 0, 0, 0, 0, 0, 2);
        return ret.error;
}

void putchar(char c) {
        sbi_call(c, 0, 0, 0, 0, 0, 0, 1);
}

struct process* createProcess(const void *image, size_t imageSize) {
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
        *--sp = 0;               // s11
        *--sp = 0;               // s10
        *--sp = 0;               // s9
        *--sp = 0;               // s8
        *--sp = 0;               // s7
        *--sp = 0;               // s6
        *--sp = 0;               // s5
        *--sp = 0;               // s4
        *--sp = 0;               // s3
        *--sp = 0;               // s2
        *--sp = 0;               // s1
        *--sp = 0;               // s0
        *--sp = (u32)user_entry; // ra

        u32 *pageTable = (u32*)alloc_pages(1);

        // Map kernel pages
        for (paddr_t paddr = (paddr_t)__kernel_base;
                        paddr < (paddr_t)__free_ram_end; paddr += PAGE_SIZE) {
                mapPage(pageTable, paddr, paddr, PAGE_R | PAGE_W | PAGE_X);
        }

        // Map user pages
        for (u32 off = 0; off < imageSize; off += PAGE_SIZE) {
                paddr_t page = alloc_pages(1);

                // Handle case where data is smaller than a page
                size_t remaining = imageSize - off;
                size_t copySize = PAGE_SIZE <= remaining ? PAGE_SIZE : remaining;

                // Fill and map page
                memcpy((void*)page, image + off, copySize);
                mapPage(pageTable, USER_BASE + off, page,
                                PAGE_U | PAGE_R | PAGE_W | PAGE_X);
        }

        // Initalize fields
        proc->pid = i + 1;
        proc->state = PROC_RUNNABLE;
        proc->sp = (u32)sp;
        proc->pageTable = pageTable;
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

        struct process *prev = current_proc;
        current_proc = next;

        // Save stack pointer of current process
        asm volatile(
                "sfence.vma\n"
                "csrw satp, %[satp]\n"
                "sfence.vma\n"
                "csrw sscratch, %[sscratch]\n"
                :
                : [satp] "r" (SATP_SV32 | ((u32)next->pageTable / PAGE_SIZE)),
                  [sscratch] "r" ((u32)&next->stack[sizeof(next->stack)])
        );

        switch_context(&prev->sp, &next->sp);
}

void handleSyscall(struct trap_frame *f) {
        switch (f->a3) {
        case SYS_PUTCHAR:
                putchar(f->a0);
                break;
        case SYS_GETCHAR:
                while (1) {
                        long ch = getchar();
                        if (ch >= 0) {
                                f->a0 = ch;
                                break;
                        }
                        yield();
                }
                break;
        case SYS_EXIT:
                printf("process %d exited\n", current_proc->pid);
                current_proc->state = PROC_EXITED;
                yield();
                PANIC("unreachable");
        default:
                PANIC("Unhandled syscall a3=%x\n", f->a3);
        }
}

void handleTrap(struct trap_frame *f) {
        u32 scause = READ_CSR(scause);
        u32 stval = READ_CSR(stval);
        u32 userPC = READ_CSR(sepc);

        if (scause == SCAUSE_ECALL) {
                handleSyscall(f);
                userPC +=4;
        } else {
                PANIC("Unexpected trap scause=%x, stval=%x, sepc=%x\n",
                                scause, stval, userPC);
        }

        WRITE_CSR(sepc, userPC);
}

void kernel_main(void) {
        memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);
        WRITE_CSR(stvec, (u32) kernel_entry);
        printf("\n\nhello, %s\n", "world");

        // Create idle process
        idle_proc = createProcess(NULL, 0);
        idle_proc->pid = 0;
        current_proc = idle_proc;

        createProcess(_binary_shell_bin_start, (size_t)_binary_shell_bin_size);

        yield();
        PANIC("Switched to idle process!");
}

