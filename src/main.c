#include "chip8.h"
#include "common.h"
#include "display.h"
#include "file.h"
#include "input.h"
#include <efi-st.h>

#define ROM_SIZE 0xE00
#define INTERVAL_60HZ 16666
#define CHIP8_TICK_RATE 8
#define MAX_FILE_COUNT 26

static CHAR16 files[MAX_FILE_COUNT][MAX_FILENAME_LEN];

void display_menu(EFI_SYSTEM_TABLE *SystemTable) {
    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
    display_clear();

    output(u"Press specified key to start ROM.\n\r\n\r");

    CHAR16 rom_prefix[4] = u"a: ";
    for (int i = 0; i < MAX_FILE_COUNT; i++) {
        if (!files[i][0])
            break;

        output(rom_prefix);
        output(files[i]);
        output(u"\n\r");

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

        int selected = Key.UnicodeChar - u'a';
        if (0 <= selected && selected < 26 && files[selected][0])
            return selected;
    }
}

void start_chip8(EFI_SYSTEM_TABLE *SystemTable, uint8_t *rom) {
    display_init(SystemTable);
    input_init(SystemTable);
    chip8_init(rom, ROM_SIZE);

    for (;;) {
        for (int i = 0; i < CHIP8_TICK_RATE; i++)
            chip8_loop();

        chip8_timer_60HZ();

        if (update_key_states())
            return;

        SystemTable->BootServices->Stall(INTERVAL_60HZ);
    }
}

int efi_main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    EFI_FILE_PROTOCOL *volume = open_volume(imageHandle, SystemTable);
    EFI_FILE_PROTOCOL *rom_dir = open_directory(volume, u"roms", SystemTable);
    read_file_names(rom_dir, files, MAX_FILE_COUNT);

    uint8_t rom[ROM_SIZE];

    for (;;) {
        display_menu(SystemTable);
        read_file(rom_dir, files[poll_key(SystemTable)], rom, ROM_SIZE);
        start_chip8(SystemTable, rom);
    }

    return 0;
}
