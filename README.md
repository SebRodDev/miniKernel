# toy-kernel

A from-scratch, Unix-like toy kernel. Stage 1 (this skeleton) boots via
Multiboot/GRUB and prints to the screen via a hand-written VGA text-mode driver.

## Setup (macOS)

```bash
brew install nasm qemu
brew tap nativeos/i386-elf-toolchain
brew install i386-elf-binutils i386-elf-gcc
```

Verify:
```bash
i386-elf-gcc --version
qemu-system-i386 --version
```

## Build & run

```bash
make run
```

This assembles `boot/boot.s`, compiles the C sources, links everything at the
1MiB mark via `boot/linker.ld`, and boots the resulting `kernel.bin` directly
in QEMU (no GRUB/ISO needed for day-to-day development — `-kernel` tells QEMU
to act as a multiboot-aware bootloader itself).

You should see:
```
Stage 1: kernel booted successfully.
Multiboot handoff OK, GDT/stack are live.
Next: IDT + interrupt handling.
```

If you want a real bootable ISO (e.g. to test under a different emulator, or
eventually on real hardware via a USB stick), `make iso` will build one using
`grub-mkrescue` (requires `brew install i386-elf-grub xorriso` first).

## What's here (Stage 1)

- `boot/boot.s` — Multiboot header, stack setup, hands off to `kernel_main`
- `boot/linker.ld` — places the kernel at the conventional 1MiB mark
- `src/vga.c` / `include/vga.h` — direct framebuffer writes to `0xB8000`,
  scrolling, basic terminal semantics
- `src/kernel.c` — entry point, currently just proves boot + output work