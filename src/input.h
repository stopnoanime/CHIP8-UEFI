#ifndef CHIP8_INPUT
#define CHIP8_INPUT

#include <efi-st.h>
#include <stdbool.h>
#include <stdint.h>

#define KEY_TICKS 17

void input_init(EFI_SYSTEM_TABLE *ST);

/*
  Needs to be called every tick to update key states.
  Returns true if ESC was pressed.
 */
bool update_key_states();

/* True if given key is currently pressed. */
bool key_pressed(uint8_t key);

/* Mark all keys as not released, needed for any_key_released to work properly. */
void reset_key_released();

/* True if any key was released since last reset_key_released. */
bool any_key_released(uint8_t *key);

#endif
