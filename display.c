#include "common.h"
#include "display.h"
#include "efi/protocol/efi-gop.h"

static int DISPLAY_SCALE;
static int DISPLAY_OFFSET_X;
static int DISPLAY_OFFSET_Y;

BOOLEAN CHIP8_FRAMEBUFFER[CHIP8_DISPLAY_X][CHIP8_DISPLAY_Y];

static EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;

static void display_pixel(int x, int y, BOOLEAN pixel)
{
   *((uint32_t*)(gop->Mode->FrameBufferBase + 4 * gop->Mode->Info->PixelsPerScanLine * y + 4 * x)) = pixel ? 0x33FF00 : 0;
}

void init_display(EFI_SYSTEM_TABLE* SystemTable) {
    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

    EFI_GUID GraphicsGUID = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    SystemTable->BootServices->LocateProtocol(&GraphicsGUID, NULL, (VOID**)&gop);

    int x_res = gop->Mode->Info->HorizontalResolution;
    int y_res = gop->Mode->Info->VerticalResolution;

    DISPLAY_SCALE = MIN(x_res / CHIP8_DISPLAY_X, y_res / CHIP8_DISPLAY_Y);
    DISPLAY_OFFSET_X = (x_res - (CHIP8_DISPLAY_X * DISPLAY_SCALE)) / 2;
    DISPLAY_OFFSET_Y = (y_res - (CHIP8_DISPLAY_Y * DISPLAY_SCALE)) / 2;
}

void draw_framebuffer() {
    for (int y = 0; y < CHIP8_DISPLAY_Y * DISPLAY_SCALE; y++)
        for (int x = 0; x < CHIP8_DISPLAY_X * DISPLAY_SCALE; x++)
            display_pixel(x + DISPLAY_OFFSET_X, y + DISPLAY_OFFSET_Y, CHIP8_FRAMEBUFFER[x/DISPLAY_SCALE][y/DISPLAY_SCALE]);
}