######################################################################
# @author      : Justin Kachele (justin@kachele.com)
# @file        : Makefile
# @created     : Wednesday Oct 29, 2025 11:43:52 UTC
######################################################################
CC = riscv64-unknown-elf-gcc
LD = riscv64-unknown-elf-ld
OBJCOPY = riscv64-unknown-elf-objcopy
CFLAGS = -march=rv32g -mabi=ilp32 -nostdlib -Wno-builtin-declaration-mismatch
LDFLAGS = --no-dynamic-linker -m elf32lriscv -static -nostdlib -s
OBFLAGS_BIN = --set-section-flags .bss=alloc,contents -O binary
OBFLAGS_O = -Ibinary -Oelf32-littleriscv

QEMU = qemu-system-riscv32 
QFLAGS  = -machine virt -bios default -nographic -serial mon:stdio --no-reboot
QFLAGS += -d unimp,guest_errors,int,cpu_reset -D qemu.log

SRC_U = $(wildcard src/user/*.c) src/common.c
ASM_U = $(wildcard src/user/*.S)
OBJ_U = $(ASM_U:.S=.o) $(SRC_U:.c=.o)

LDSCRIPT_U = src/user/user.ld

SRC_K = $(wildcard src/*.c)
ASM_K = $(wildcard src/*.S)
OBJ_K = $(ASM_K:.S=.o) $(SRC_K:.c=.o)
LDSCRIPT_K = src/kernel.ld

BIN = bin

.phony: clean run

all: dirs kernel

kernel: $(OBJ_K) shell_o dirs
	$(LD) -T $(LDSCRIPT_K) -o $(BIN)/$@ $(OBJ_K) $(BIN)/shell_o $(LDFLAGS)
	rm -f $(OBJ_K)
	rm -f $(OBJ_U)

shell_o: shell
	$(OBJCOPY) $(OBFLAGS_BIN) $(BIN)/$< shell.bin
	$(OBJCOPY) $(OBFLAGS_O) shell.bin $(BIN)/$@
	rm shell.bin

shell: $(OBJ_U) dirs
	$(LD) -T $(LDSCRIPT_U) -o $(BIN)/$@ $(OBJ_U) $(LDFLAGS)

%.o: %.S
	$(CC) -o $@ -c $^ $(CFLAGS)

%.o: %.c
	$(CC) -o $@ -c $^ $(CFLAGS)

dirs:
	mkdir -p ./$(BIN)

run: all
	$(QEMU) $(QFLAGS) -kernel $(BIN)/kernel

clean:
	rm -f $(OBJ)
	rm -rf $(BIN)
