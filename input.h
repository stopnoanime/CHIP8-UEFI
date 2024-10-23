#ifndef CHIP8_INPUT
#define CHIP8_INPUT

#include <stdbool.h>
#include <stdint.h>

bool key_pressed(uint8_t key);
bool any_pressed(uint8_t *key);

#endif
