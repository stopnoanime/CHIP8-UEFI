#ifndef CHIP8_DISPLAY
#define CHIP8_DISPLAY

#include "efi/efi-st.h"
#include <stdbool.h>

#define CHIP8_DISPLAY_X 64
#define CHIP8_DISPLAY_Y 32

extern bool CHIP8_FRAMEBUFFER[CHIP8_DISPLAY_X][CHIP8_DISPLAY_Y];

void init_display(EFI_SYSTEM_TABLE *SystemTable);

void clear_framebuffer();
void draw_framebuffer();
bool display_sprite(int x_start, int y_start, int tall, uint8_t *sprite);

#endif
