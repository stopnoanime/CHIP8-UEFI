#include "chip8.h"
#include "common.h"
#include "display.h"
#include "input.h"
#include <efi-st.h>
#include <protocol/efi-lip.h>
#include <protocol/efi-sfsp.h>

#define ROM_SIZE 0xE00
#define CHIP8_DELAY 1000

typedef union EFI_FILE_INFO_CONST {
  EFI_FILE_INFO data;
  CHAR16 _[64];
} EFI_FILE_INFO_CONST;

static CHAR16 files[26][16];

void copy_string(CHAR16 *dest, CHAR16 *src) {
  while ((*dest++ = *src++))
    ;
}

/** https://wiki.osdev.org/Loading_files_under_UEFI */
EFI_FILE_PROTOCOL *OpenVolume(EFI_HANDLE image, EFI_SYSTEM_TABLE *ST) {
  EFI_GUID lipGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
  EFI_LOADED_IMAGE_PROTOCOL *limg;

  EFI_GUID fsGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *sfs;

  EFI_FILE_PROTOCOL *Volume;

  ST->BootServices->HandleProtocol(image, &lipGuid, (VOID **)&limg);
  ST->BootServices->HandleProtocol(limg->DeviceHandle, &fsGuid, (VOID **)&sfs);
  sfs->OpenVolume(sfs, &Volume);

  return Volume;
}

EFI_FILE_PROTOCOL *open_roms_dir(EFI_HANDLE imageHandle,
                                 EFI_SYSTEM_TABLE *SystemTable) {
  EFI_FILE_PROTOCOL *Volume = OpenVolume(imageHandle, SystemTable);
  EFI_FILE_PROTOCOL *Roms;

  Volume->Open(Volume, &Roms, L"roms", EFI_FILE_MODE_READ, 0);

  return Roms;
}

void init_files(EFI_FILE_PROTOCOL *Roms) {
  EFI_FILE_INFO_CONST file_info;
  UINTN file_info_size;
  int index = 0;

  for (;;) {
    file_info_size = sizeof(file_info);
    Roms->Read(Roms, &file_info_size, &file_info);

    if (file_info_size == 0)
      break;

    if (file_info.data.Attribute & EFI_FILE_DIRECTORY)
      continue;

    copy_string(files[index], file_info.data.FileName);

    if (index == 26)
      break;

    index++;
  }

  for (; index < 26; index++)
    files[index][0] = 0;
}

void display_menu(EFI_SYSTEM_TABLE *SystemTable) {
  CHAR16 rom_prefix[4] = L"a: ";

  output(L"Press specified key to start ROM.\n\r\n\r");

  for (int i = 0; i < 26; i++) {
    if (!files[i][0])
      break;

    output(rom_prefix);
    output(files[i]);
    output(L"\n\r");

    rom_prefix[0]++;
  }
}

int poll_key(EFI_SYSTEM_TABLE *SystemTable) {
  UINTN Index;
  EFI_INPUT_KEY Key;
  EFI_STATUS Status;

  for (;;) {
    Status = SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key);

    if (Status != EFI_SUCCESS)
      continue;

    int selected = Key.UnicodeChar - L'a';
    if (0 <= selected && selected < 26 && files[selected][0])
      return selected;
  }
}

void load_rom(EFI_FILE_PROTOCOL *Roms, int rom_number, uint8_t *buff) {
  EFI_FILE_PROTOCOL *Rom;
  Roms->Open(Roms, &Rom, files[rom_number], EFI_FILE_MODE_READ, 0);

  UINTN buff_size = ROM_SIZE;
  Rom->Read(Rom, &buff_size, buff);
}

void start_chip8(EFI_SYSTEM_TABLE *SystemTable, uint8_t *rom) {
  chip8_init(rom, ROM_SIZE);
  init_display(SystemTable);
  init_input(SystemTable);

  int timer_counter = 0;
  while (true) {
    chip8_loop();

    timer_counter = (timer_counter + 1) % 10;
    if (timer_counter == 0)
      chip8_timer_60HZ();

    if (update_pressed())
      return;

    SystemTable->BootServices->Stall(CHIP8_DELAY);
  }
}

int efi_main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE *SystemTable) {

  EFI_FILE_PROTOCOL *Roms = open_roms_dir(imageHandle, SystemTable);
  init_files(Roms);

  for (;;) {
    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
    display_menu(SystemTable);
    int selected = poll_key(SystemTable);

    uint8_t rom[ROM_SIZE];
    load_rom(Roms, selected, rom);

    start_chip8(SystemTable, rom);
  }

  return 0;
}
