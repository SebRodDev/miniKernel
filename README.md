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

## Roadmap (what to build next, in order)

1. **IDT + interrupt handling** — exception handlers first (divide-by-zero,
   page fault, general protection fault with real error decoding), then wire
   up the PIC (8259) and get hardware IRQs flowing (PIT timer on IRQ0,
   keyboard on IRQ1).
2. **Physical memory manager** — parse the multiboot memory map (that's what
   the `multiboot_info` pointer already being passed into `kernel_main` is
   for), build a bitmap or free-list allocator over usable RAM.
3. **Paging / virtual memory** — page directory + page tables, identity-map
   the kernel, build a `kmalloc`-style heap allocator on top of the physical
   allocator.
4. **Preemptive scheduler** — PIT interrupt drives context switches between
   multiple kernel-level "tasks" (this is the part that makes it
   Unix-*like* rather than just a bootloader with a print statement — real
   concurrency, saved/restored register state, a ready queue).
5. **Syscall interface** — `int 0x80` handler; implement `write` and `exit`
   first, since almost everything else builds on being able to output and
   terminate cleanly.
6. **(Stretch) userspace separation** — ring 3 processes, a small fixed
   syscall table, maybe a tiny initrd with 2-3 toy programs to actually
   demonstrate multiple processes running.

Each stage above is a genuinely testable, demoable milestone on its own —
don't feel like you need to reach step 6 for this to be a strong project.
Stopping after step 4 (a kernel that boots, manages memory, and actually
preempts between multiple tasks) is already a very strong "Systems Software"
credential by itself.
