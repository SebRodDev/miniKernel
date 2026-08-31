; boot.s — Multiboot entry point
; Sets up a stack, then jumps into our C kernel entry point.

MBALIGN     equ  1 << 0              ; align loaded modules on page boundaries
MEMINFO     equ  1 << 1              ; provide memory map
MBFLAGS     equ  MBALIGN | MEMINFO
MAGIC       equ  0x1BADB002          ; magic number for multiboot bootloader to find
CHECKSUM    equ -(MAGIC + MBFLAGS)   ; checksum required to prove we're multiboot

section .multiboot
align 4
    dd MAGIC
    dd MBFLAGS
    dd CHECKSUM

; Reserve a small stack. The bootloader doesn't set one up for us.
section .bss
align 16
stack_bottom:
    resb 16384      ; 16 KiB stack
stack_top:

section .text
global _start
extern kernel_main
_start:
    ; Set up the stack (grows downward on x86)
    mov esp, stack_top

    ; ebx = pointer to multiboot info struct (passed by GRUB), useful later
    ; for reading the memory map. Push it as an argument to kernel_main.
    push ebx
    call kernel_main

    ; If kernel_main ever returns, halt forever
    cli
.hang:
    hlt
    jmp .hang
