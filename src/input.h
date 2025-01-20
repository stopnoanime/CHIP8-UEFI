#ifndef CHIP8_INPUT
#define CHIP8_INPUT

#include <efi-st.h>
#include <stdbool.h>
#include <stdint.h>

#define KEY_TICKS 12

void init_input(EFI_SYSTEM_TABLE *ST);
bool key_pressed(uint8_t key);

/* Mark all keys as not released ,needed for any_released to work properly.*/
void reset_released();

/* True if any key was released since last reset_released */
bool any_released(uint8_t *key);

/*
  Needs to be called every tick to update key states.
  Returns true if ESC was detected
 */
bool update_pressed();

#endif
