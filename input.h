#ifndef CHIP8_INPUT
#define CHIP8_INPUT

#include "efi/efi-st.h"
#include <stdbool.h>
#include <stdint.h>

#define KEY_TICKS 150

void init_input(EFI_SYSTEM_TABLE *ST);
bool key_pressed(uint8_t key);
void reset_released();
bool any_released(uint8_t *key);
bool update_pressed();

#endif
