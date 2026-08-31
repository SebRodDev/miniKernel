#ifndef IO_H
#define IO_H

#include <stdint.h>

/* Write a byte to an I/O port. */
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

/* Read a byte from an I/O port. */
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/* A tiny delay used after PIC/hardware reprogramming — writing to an unused
   port (0x80, conventionally used for POST diagnostic codes) takes long
   enough to give slow hardware time to catch up. This is a decades-old
   real-mode-era trick that's still the standard way to do this. */
static inline void io_wait(void) {
    outb(0x80, 0);
}

#endif