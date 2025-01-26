#ifndef CHIP8_DISPLAY
#define CHIP8_DISPLAY

#include <efi-st.h>
#include <stdbool.h>

#define CHIP8_DISPLAY_X 64
#define CHIP8_DISPLAY_Y 32

#define GOP_DRAW(x, y, color)                                                                                          \
    (*((uint32_t *)(gop->Mode->FrameBufferBase + 4 * gop->Mode->Info->PixelsPerScanLine * (y) + 4 * (x))) = (color))

void display_init(EFI_SYSTEM_TABLE *SystemTable);

void display_clear();
bool display_sprite(int x_start, int y_start, int tall, uint8_t *sprite);

#endif
