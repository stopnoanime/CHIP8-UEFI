#include "chip8.h"
#include "display.h"
#include "efi/efi-st.h"

int efi_main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE *SystemTable) {
  static const uint8_t rom[132] = {
      0x00, 0xe0, 0xa2, 0x2a, 0x60, 0x0c, 0x61, 0x08, 0xd0, 0x1f, 0x70, 0x09,
      0xa2, 0x39, 0xd0, 0x1f, 0xa2, 0x48, 0x70, 0x08, 0xd0, 0x1f, 0x70, 0x04,
      0xa2, 0x57, 0xd0, 0x1f, 0x70, 0x08, 0xa2, 0x66, 0xd0, 0x1f, 0x70, 0x08,
      0xa2, 0x75, 0xd0, 0x1f, 0x12, 0x28, 0xff, 0x00, 0xff, 0x00, 0x3c, 0x00,
      0x3c, 0x00, 0x3c, 0x00, 0x3c, 0x00, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff,
      0x00, 0x38, 0x00, 0x3f, 0x00, 0x3f, 0x00, 0x38, 0x00, 0xff, 0x00, 0xff,
      0x80, 0x00, 0xe0, 0x00, 0xe0, 0x00, 0x80, 0x00, 0x80, 0x00, 0xe0, 0x00,
      0xe0, 0x00, 0x80, 0xf8, 0x00, 0xfc, 0x00, 0x3e, 0x00, 0x3f, 0x00, 0x3b,
      0x00, 0x39, 0x00, 0xf8, 0x00, 0xf8, 0x03, 0x00, 0x07, 0x00, 0x0f, 0x00,
      0xbf, 0x00, 0xfb, 0x00, 0xf3, 0x00, 0xe3, 0x00, 0x43, 0xe0, 0x00, 0xe0,
      0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0xe0, 0x00, 0xe0};
  init_display(SystemTable);
  chip8_init(rom, 132);

  while (true)
    chip8_loop();

  return 0;
}
