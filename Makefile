######################################################################
# @author      : Justin Kachele (justin@kachele.com)
# @file        : Makefile
# @created     : Wednesday Oct 29, 2025 11:43:52 UTC
######################################################################
CC = riscv64-unknown-elf-gcc
AS = riscv64-unknown-elf-as
LD = riscv64-unknown-elf-ld
CFLAGS = -march=rv32g -mabi=ilp32 -nostdlib -Wno-builtin-declaration-mismatch
ASFLAGS = -march=rv32g -mabi=ilp32
LDFLAGS = --no-dynamic-linker -m elf32lriscv -static -nostdlib -s

SBI = ../opensbi/build/platform/generic/firmware/fw_dynamic.bin
LDSCRIPT = kernel.ld

SRC = $(wildcard src/*.c)
ASM = $(wildcard src/*.S) $(SRC:.c=.S)
OBJ = $(ASM:.S=.o)

BIN = bin

.phony: clean run

all: dirs kernel

kernel: $(OBJ) dirs
	$(LD) -T $(LDSCRIPT) -o $(BIN)/$@ $(OBJ) $(LDFLAGS)
	rm $(OBJ)

%.o: %.S
	$(AS) -o $@ -c $< $(ASFLAGS)

%.S: %.c
	$(CC) -o $@ -S $^ $(CFLAGS)

dirs:
	mkdir -p ./$(BIN)

run: all
	qemu-system-riscv32 -machine virt -bios default -kernel $(BIN)/kernel -nographic -serial mon:stdio --no-reboot

clean:
	rm -f $(OBJ)
	rm -rf $(BIN)
