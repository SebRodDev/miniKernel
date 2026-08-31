/* linker.ld — lays out the kernel binary in memory */

ENTRY(_start)

SECTIONS
{
    /* Kernel loaded at 1MiB, the conventional spot that's safe on every x86 machine
       and avoids the messy low-memory region (BIOS data, video memory, etc). */
    . = 1M;

    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot)
        *(.text)
    }

    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    }

    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    }

    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }
}
