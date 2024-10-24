#include "input.h"

static EFI_SYSTEM_TABLE *SystemTable;
static uint8_t KEY_STATE[16];
static const CHAR16 KEY_MAP[16] = {L'x', L'1', L'2', L'3', L'q', L'w',
                                   L'e', L'a', L's', L'd', L'z', L'c',
                                   L'4', L'r', L'f', L'v'};

static CHAR16 keyToUnicode(uint8_t key) { return KEY_MAP[key]; }

static uint8_t unicodeToKey(CHAR16 unicode) {
  for (int i = 0; i < 16; i++)
    if (KEY_MAP[i] == unicode)
      return i;

  return -1;
}

void init_input(EFI_SYSTEM_TABLE *ST) {
  SystemTable = ST;

  for (int i = 0; i < 16; i++)
    KEY_STATE[i] = KEY_TICKS;
}

bool key_pressed(uint8_t key) { return KEY_STATE[key] < KEY_TICKS; }

void reset_released() {
  for (int i = 0; i < 16; i++)
    if (KEY_STATE[i] == KEY_TICKS)
      KEY_STATE[i]++;
}

bool any_released(uint8_t *key) {
  for (int i = 0; i < 16; i++) {
    if (KEY_STATE[i] == KEY_TICKS) {
      *key = i;
      return true;
    }
  }

  return false;
}

bool update_pressed() {
  EFI_STATUS status;
  EFI_INPUT_KEY inputKey;

  status = SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &inputKey);

  if (status == EFI_SUCCESS) {
    if (inputKey.ScanCode == 0x17)
      return true;

    uint8_t key = unicodeToKey(inputKey.UnicodeChar);

    if (key < 16)
      KEY_STATE[key] = 0;

    return update_pressed();
  }

  for (int i = 0; i < 16; i++)
    if (KEY_STATE[i] < KEY_TICKS)
      KEY_STATE[i]++;

  return false;
}