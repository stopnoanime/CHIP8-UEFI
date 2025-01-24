#include "display.h"
#include "common.h"
#include <protocol/efi-gop.h>

static int DISPLAY_SCALE;
static int DISPLAY_OFFSET_X;
static int DISPLAY_OFFSET_Y;

static bool CHIP8_FRAMEBUFFER[CHIP8_DISPLAY_X][CHIP8_DISPLAY_Y];

static EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

void display_init(EFI_SYSTEM_TABLE *SystemTable) {
  SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

  EFI_GUID GraphicsGUID = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
  SystemTable->BootServices->LocateProtocol(&GraphicsGUID, NULL, (VOID **)&gop);

  int x_res = gop->Mode->Info->HorizontalResolution;
  int y_res = gop->Mode->Info->VerticalResolution;

  DISPLAY_SCALE = MIN(x_res / CHIP8_DISPLAY_X, y_res / CHIP8_DISPLAY_Y);
  DISPLAY_OFFSET_X = (x_res - (CHIP8_DISPLAY_X * DISPLAY_SCALE)) / 2;
  DISPLAY_OFFSET_Y = (y_res - (CHIP8_DISPLAY_Y * DISPLAY_SCALE)) / 2;
}

void display_clear() {
  for (int y = 0; y < CHIP8_DISPLAY_Y; y++)
    for (int x = 0; x < CHIP8_DISPLAY_X; x++)
      CHIP8_FRAMEBUFFER[x][y] = false;

  // Clear the real screen
  for (int y = 0; y < DISPLAY_SCALE * CHIP8_DISPLAY_Y; y++) 
    for (int x = 0; x < DISPLAY_SCALE * CHIP8_DISPLAY_X; x++)
      GOP_DRAW(DISPLAY_OFFSET_X + x, DISPLAY_OFFSET_Y + y, 0);
}

bool display_sprite(int x_start, int y_start, int tall, uint8_t *sprite) {
  x_start = x_start % CHIP8_DISPLAY_X;
  y_start = y_start % CHIP8_DISPLAY_Y;

  bool turned_off = false;

  for (int y = 0; y < tall && (y + y_start) < CHIP8_DISPLAY_Y; y++) {
    for (int x = 0; x < 8 && (x + x_start) < CHIP8_DISPLAY_X; x++) {
      bool *framebuffer_pixel = &CHIP8_FRAMEBUFFER[x + x_start][y + y_start];
      bool sprite_pixel = (sprite[y] >> (7 - x)) & 1;

      if (sprite_pixel && *framebuffer_pixel)
        turned_off = true;

      *framebuffer_pixel ^= sprite_pixel;

      // Displays the sprite pixel onto the real screen with scaling
      for (int pixel_y = 0; pixel_y < DISPLAY_SCALE; pixel_y++) { 
        for (int pixel_x = 0; pixel_x < DISPLAY_SCALE; pixel_x++) {
          uint32_t color = *framebuffer_pixel ? 0x33FF00 : 0;
          int screen_y = DISPLAY_OFFSET_Y + (y_start + y) * DISPLAY_SCALE + pixel_y;
          int screen_x = DISPLAY_OFFSET_X + (x_start + x) * DISPLAY_SCALE + pixel_x;

          GOP_DRAW(screen_x, screen_y, color);
        }
      }
    }
  }

  return turned_off;
}