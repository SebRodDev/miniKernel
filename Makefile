# Toy kernel Makefile
# Requires the i686-elf cross-toolchain (see README.md for install instructions).
CC := i686-elf-gcc
LD := i686-elf-gcc
AS := nasm
CFLAGS := -std=gnu11 -ffreestanding -O2 -Wall -Wextra -Iinclude
LDFLAGS := -ffreestanding -O2 -nostdlib -lgcc -T boot/linker.ld
ASM_SRC := boot/boot.s
C_SRC := $(wildcard src/*.c)
ASM_OBJ := $(ASM_SRC:.s=.o)
C_OBJ := $(C_SRC:.c=.o)
OBJ := $(ASM_OBJ) $(C_OBJ)
KERNEL := kernel.bin
ISO := kernel.iso

.PHONY: all clean run iso

all: $(KERNEL)

%.o: %.s
	$(AS) -f elf32 $< -o $@
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
$(KERNEL): $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $(OBJ)
# Boot the raw kernel binary directly in QEMU — fastest inner loop for development.
run: $(KERNEL)
	qemu-system-i386 -kernel $(KERNEL)
# Build a bootable ISO with GRUB — closer to "real" bootable media, useful later
# once you want to test on real hardware or need GRUB-specific features.
iso: $(KERNEL)
	mkdir -p isodir/boot/grub
	cp $(KERNEL) isodir/boot/kernel.bin
	cp boot/grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) isodir
clean:
	rm -f $(OBJ) $(KERNEL) $(ISO)
	rm -rf isodir