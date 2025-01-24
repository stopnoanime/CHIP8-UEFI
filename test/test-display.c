// Compare performance of different framebuffer access methods
#include <efi-st.h>
#include <protocol/efi-gop.h>
#include <stdbool.h>

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define NULL ((void *)0)

#define DISPLAY_X 64
#define DISPLAY_Y 32

static int DISPLAY_SCALE;

static EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

void init_vars(EFI_SYSTEM_TABLE *SystemTable) {
  EFI_GUID GraphicsGUID = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
  SystemTable->BootServices->LocateProtocol(&GraphicsGUID, NULL, (VOID **)&gop);

  int x_res = gop->Mode->Info->HorizontalResolution;
  int y_res = gop->Mode->Info->VerticalResolution;
  DISPLAY_SCALE = MIN(x_res / DISPLAY_X, y_res / DISPLAY_Y);
}

void draw_framebuffer_each_pixel(uint32_t color) {
  for (int y = 0; y < DISPLAY_Y * DISPLAY_SCALE; y++) { 
    for (int x = 0; x < DISPLAY_X * DISPLAY_SCALE; x++) {
      uint32_t pixel = (x/DISPLAY_SCALE + y/DISPLAY_SCALE)%2 ? color : 0;
      *((uint32_t*)(gop->Mode->FrameBufferBase + 4 * gop->Mode->Info->PixelsPerScanLine * y + 4 * x)) = pixel;
    }
  }
}

void draw_blt_each_block(uint32_t color) {
  for (int y = 0; y < DISPLAY_Y; y++) { 
    for (int x = 0; x < DISPLAY_X; x++) {
      uint32_t pixel = (x + y)%2 ? color : 0;
      gop->Blt(gop, (void*)&pixel, EfiBltVideoFill, 0, 0, DISPLAY_SCALE*x, DISPLAY_SCALE*y, DISPLAY_SCALE, DISPLAY_SCALE, 0);
    }
  }
}

void display_uint64_t(uint64_t num, EFI_SYSTEM_TABLE *SystemTable) {
  CHAR16 buf[25];
  int i = 0;

  do {
    buf[i++] = (num % 10) + u'0';
    num /= 10;
  } while (num > 0);

  buf[i] = 0;

  i--;
  for (int j = 0; j < i; i++, j--) {
    CHAR16 temp = buf[i];
    buf[i] = buf[j];
    buf[j] = temp;
  }

  SystemTable->ConOut->OutputString(SystemTable->ConOut, buf);
}

void benchmark_function(void (*f)(uint32_t color), EFI_SYSTEM_TABLE *SystemTable) {
  uint64_t start = __builtin_ia32_rdtsc();

  for (int i = 0; i < 200; i++)
    f(i%2 ? 0xFF: 0xFF00);

  uint64_t stop = __builtin_ia32_rdtsc();
  display_uint64_t(stop-start, SystemTable);
}

int efi_main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE *SystemTable) {
  init_vars(SystemTable);

  benchmark_function(draw_framebuffer_each_pixel, SystemTable);

  SystemTable->BootServices->Stall(5000000);

  benchmark_function(draw_blt_each_block, SystemTable);

  for(;;) {}
}
