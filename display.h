#ifndef CHIP8_DISPLAY
#define CHIP8_DISPLAY

#include "efi/efi-st.h"

#define CHIP8_DISPLAY_X 64
#define CHIP8_DISPLAY_Y 32

extern BOOLEAN CHIP8_FRAMEBUFFER[CHIP8_DISPLAY_X][CHIP8_DISPLAY_Y];

void init_display(EFI_SYSTEM_TABLE* SystemTable);
void draw_framebuffer();

#endif
