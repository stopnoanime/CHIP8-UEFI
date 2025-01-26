#include "input.h"

static EFI_SYSTEM_TABLE *SystemTable;

/* Counts the number of ticks since last keypress for every key */
static uint8_t KEY_STATE[16];
static const CHAR16 KEY_MAP[16] = {
    u'x', u'1', u'2', u'3', u'q', u'w', u'e', u'a', u's', u'd', u'z', u'c', u'4', u'r', u'f', u'v',
};

static CHAR16 keyToUnicode(uint8_t key) { return KEY_MAP[key]; }

static uint8_t unicodeToKey(CHAR16 unicode) {
    for (int i = 0; i < 16; i++)
        if (KEY_MAP[i] == unicode)
            return i;

    return -1;
}

void input_init(EFI_SYSTEM_TABLE *ST) {
    SystemTable = ST;

    for (int i = 0; i < 16; i++)
        KEY_STATE[i] = KEY_TICKS; // Mark as released
}

bool key_pressed(uint8_t key) { return KEY_STATE[key] < KEY_TICKS; }

void reset_key_released() {
    for (int i = 0; i < 16; i++)
        if (KEY_STATE[i] == KEY_TICKS)
            KEY_STATE[i] = KEY_TICKS + 1;
}

bool any_key_released(uint8_t *key) {
    for (int i = 0; i < 16; i++) {
        if (KEY_STATE[i] == KEY_TICKS) {
            *key = i;
            return true;
        }
    }

    return false;
}

bool update_key_states() {
    EFI_STATUS status;
    EFI_INPUT_KEY inputKey;

    for (;;) {
        status = SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &inputKey);

        if (status != EFI_SUCCESS)
            break;

        if (inputKey.ScanCode == 0x17) // Escape
            return true;

        uint8_t key = unicodeToKey(inputKey.UnicodeChar);

        if (key < 16)
            KEY_STATE[key] = 0;
    }

    for (int i = 0; i < 16; i++)
        if (KEY_STATE[i] < KEY_TICKS)
            KEY_STATE[i]++;

    return false;
}