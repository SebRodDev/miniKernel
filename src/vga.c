#include "vga.h"
#include "io.h"

static const size_t VGA_WIDTH = 320;
static const size_t VGA_HEIGHT = 200;

static uint8_t* const VGA_MEMORY_BUFFER = (uint8_t*) 0xA0000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

typedef struct {
    uint8_t index;
    uint8_t value;
} reg_t;

// converting computer screen from a text-only to a canvas that i can draw things on
// since when we boot up we are in text-mode we instead enter mode 13h which was commonly used for games like doom
// we talk directly to the virtual graphics array so that we can start drawing on the screen

// once this function runs then we can access every pixel on our screen since it is just one byte so it can be easily indexed
// cause the entire screen becomes indexable
void vgaInit(void) {
    // 1. Misc Output Register (Direct write, no index)
    outb(0x3C2, 0x63);

    // 2. Sequencer Registers (Port 0x3C4)
    reg_t g_seq[] = {
        {0x00, 0x03}, {0x01, 0x01}, {0x02, 0x0F}, {0x03, 0x00}, {0x04, 0x0E}
    };
    for (int i = 0; i < 5; i++) {
        outb(0x3C4, g_seq[i].index);
        outb(0x3C4 + 1, g_seq[i].value);
    }

    // 3. CRT Controller Registers (Port 0x3D4)
    // Note: CRTC index 0x11 has write-protect bits in upper nibble, clear them first
    outb(0x3D4, 0x11);
    uint8_t prot = inb(0x3D4 + 1);
    outb(0x3D4 + 1, prot & 0x7F);

    reg_t g_crtc[] = {
        {0x00, 0x5F}, {0x01, 0x4F}, {0x02, 0x50}, {0x03, 0x82},
        {0x04, 0x54}, {0x05, 0x80}, {0x06, 0xBF}, {0x07, 0x1F},
        {0x08, 0x00}, {0x09, 0x41}, {0x0A, 0x00}, {0x0B, 0x00},
        {0x0C, 0x00}, {0x0D, 0x00}, {0x0E, 0x00}, {0x0F, 0x00},
        {0x10, 0x9C}, {0x11, 0x8E}, {0x12, 0x8F}, {0x13, 0x28},
        {0x14, 0x40}, {0x15, 0x96}, {0x16, 0xB9}, {0x17, 0xA3},
        {0x18, 0xFF}
    };
    for (int i = 0; i < 25; i++) {
        outb(0x3D4, g_crtc[i].index);
        outb(0x3D4 + 1, g_crtc[i].value);
    }

    // 4. Graphics Controller Registers (Port 0x3CE)
    reg_t g_gc[] = {
        {0x00, 0x00}, {0x01, 0x00}, {0x02, 0x00}, {0x03, 0x00},
        {0x04, 0x00}, {0x05, 0x40}, {0x06, 0x05}, {0x07, 0x0F},
        {0x08, 0xFF}
    };
    for (int i = 0; i < 9; i++) {
        outb(0x3CE, g_gc[i].index);
        outb(0x3CE + 1, g_gc[i].value);
    }

    // 5. Attribute Controller Registers (Port 0x3C0)
    reg_t g_ac[] = {
        {0x00, 0x00}, {0x01, 0x01}, {0x02, 0x02}, {0x03, 0x03},
        {0x04, 0x04}, {0x05, 0x05}, {0x06, 0x06}, {0x07, 0x07},
        {0x08, 0x08}, {0x09, 0x09}, {0x0A, 0x0A}, {0x0B, 0x0B},
        {0x0C, 0x0C}, {0x0D, 0x0D}, {0x0E, 0x0E}, {0x0F, 0x0F},
        {0x10, 0x41}, {0x11, 0x00}, {0x12, 0x0F}, {0x13, 0x00},
        {0x14, 0x00}
    };
    for (int i = 0; i < 21; i++) {
        inb(0x3DA); // Reset flip-flop before each write
        outb(0x3C0, g_ac[i].index);
        outb(0x3C0, g_ac[i].value);
    }

    // Re-enable video subsystem (Attribute Controller passthrough bit)
    inb(0x3DA);
    outb(0x3C0, 0x20);
}

void putPixel(int row, int column, enum vga_color color) {
   VGA_MEMORY_BUFFER[row * 320 + column] = color;
}

void clearScreen() {
   for (int row = 0; row < VGA_HEIGHT; row++) {
      for (int column = 0; column < VGA_WIDTH; column++) {
         putPixel(row, column, VGA_COLOR_BLACK);
      }
   }
}