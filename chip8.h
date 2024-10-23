#ifndef CHIP8
#define CHIP8

#include <stdint.h>

#define RAM_SIZE 4096
#define STACK_SIZE 32

#define X(ins) ((ins >> 8) & 0xF)
#define Y(ins) ((ins >> 4) & 0xF)
#define N(ins) (ins & 0xF)
#define NN(ins) (ins & 0xFF)
#define NNN(ins) (ins & 0xFFF)

void chip8_init(const uint8_t *rom, uint16_t rom_size);
void chip8_loop();

#endif
