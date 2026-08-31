#include "vga.h"

/* ebx from boot.s — pointer to the multiboot info struct.
   We're not parsing it yet (that's stage 3, the physical memory manager),
   but it's already being passed through so we don't have to touch boot.s again later. */
void kernel_main(void* multiboot_info) {
    (void) multiboot_info;

    renderPong();
}
