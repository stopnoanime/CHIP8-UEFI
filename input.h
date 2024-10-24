#ifndef CHIP8_INPUT
#define CHIP8_INPUT

#include <stdbool.h>
#include <stdint.h>
#include "efi/efi-st.h"

void init_input(EFI_SYSTEM_TABLE *ST);
bool key_pressed(uint8_t key);
bool any_pressed(uint8_t *key);

#endif
