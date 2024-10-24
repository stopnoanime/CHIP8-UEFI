#include "input.h"

static EFI_SYSTEM_TABLE *SystemTable;
static const CHAR16 KEY_MAP[16] = {L'x', L'1', L'2', L'3', L'q', L'w', L'e', L'a',
                                   L's', L'd', L'z', L'c', L'4', L'r', L'f', L'v'};

static CHAR16 keyToUnicode(uint8_t key) { return KEY_MAP[key]; }

static uint8_t unicodeToKey(CHAR16 unicode) {
  for (int i = 0; i < 16; i++)
    if (KEY_MAP[i] == unicode)
      return i;

  return -1;
}

void init_input(EFI_SYSTEM_TABLE *ST) { SystemTable = ST; }

bool key_pressed(uint8_t key) {
  EFI_STATUS status;
  EFI_INPUT_KEY inputKey;

  status = SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &inputKey);

  if (status != EFI_SUCCESS)
    return false;

  return inputKey.UnicodeChar == keyToUnicode(key);
}

bool any_pressed(uint8_t *key) {
  EFI_STATUS status;
  EFI_INPUT_KEY inputKey;

  status = SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &inputKey);

  if (status != EFI_SUCCESS)
    return false;

  uint8_t pressedKey = unicodeToKey(inputKey.UnicodeChar);

  if (pressedKey > 15) // invalid key
    return false;

  *key = pressedKey;
  return true;
}
