#include "display.h"
#include "common.h"
#include <protocol/efi-gop.h>

static int DISPLAY_SCALE;
static int DISPLAY_OFFSET_X;
static int DISPLAY_OFFSET_Y;

static bool CHIP8_FRAMEBUFFER[CHIP8_DISPLAY_X][CHIP8_DISPLAY_Y];

static EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

void init_display(EFI_SYSTEM_TABLE *SystemTable) {
  SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

  EFI_GUID GraphicsGUID = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
  SystemTable->BootServices->LocateProtocol(&GraphicsGUID, NULL, (VOID **)&gop);

  int x_res = gop->Mode->Info->HorizontalResolution;
  int y_res = gop->Mode->Info->VerticalResolution;

  DISPLAY_SCALE = MIN(x_res / CHIP8_DISPLAY_X, y_res / CHIP8_DISPLAY_Y);
  DISPLAY_OFFSET_X = (x_res - (CHIP8_DISPLAY_X * DISPLAY_SCALE)) / 2;
  DISPLAY_OFFSET_Y = (y_res - (CHIP8_DISPLAY_Y * DISPLAY_SCALE)) / 2;
}

void clear_framebuffer() {
  for (int y = 0; y < CHIP8_DISPLAY_Y; y++)
    for (int x = 0; x < CHIP8_DISPLAY_X; x++)
      CHIP8_FRAMEBUFFER[x][y] = false;
}

void draw_framebuffer() {
  for (int y = 0; y < CHIP8_DISPLAY_Y; y++) { 
    for (int x = 0; x < CHIP8_DISPLAY_X; x++) {
      int color = CHIP8_FRAMEBUFFER[x][y] ? 0x33FF00 : 0;
      gop->Blt(gop, (void*)&color, EfiBltVideoFill, 0, 0, DISPLAY_SCALE*x +DISPLAY_OFFSET_X , DISPLAY_SCALE*y + DISPLAY_OFFSET_Y, DISPLAY_SCALE, DISPLAY_SCALE, 0);
    }
  }
}

bool display_sprite(int x_start, int y_start, int tall, uint8_t *sprite) {
  x_start = x_start % CHIP8_DISPLAY_X;
  y_start = y_start % CHIP8_DISPLAY_Y;

  bool turned_off = false;

  for (int y = 0; y < tall && (y + y_start) < CHIP8_DISPLAY_Y; y++)
    for (int x = 0; x < 8 && (x + x_start) < CHIP8_DISPLAY_X; x++) {
      bool *framebuffer_pixel = &CHIP8_FRAMEBUFFER[x + x_start][y + y_start];
      bool sprite_pixel = (sprite[y] >> (7 - x)) & 1;

      if (sprite_pixel && *framebuffer_pixel)
        turned_off = true;

      *framebuffer_pixel ^= sprite_pixel;
    }

  return turned_off;
}