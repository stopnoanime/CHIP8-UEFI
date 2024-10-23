#include "chip8.h"
#include "display.h"
#include "font.h"

uint8_t RAM[RAM_SIZE];
uint8_t REGS[16];

uint16_t PC;
uint16_t I;

uint16_t STACK[STACK_SIZE];
uint8_t STACK_POINTER;

uint8_t DELAY_TIMER;
uint8_t SOUND_TIMER;

void chip8_init(const uint8_t *rom, uint16_t rom_size) {
  PC = 0x200;
  I = 0;
  STACK_POINTER = 0;
  DELAY_TIMER = 0;
  SOUND_TIMER = 0;

  for (int i = 0; i < FONT_SIZE; i++)
    RAM[i] = FONT_INIT[i];

  for (int i = 0; i < rom_size; i++)
    RAM[i + 0x200] = rom[i];

  clear_framebuffer();
}

void chip8_loop() {
  uint16_t ins = (RAM[PC] << 8) | RAM[PC + 1];
  PC += 2;

  switch (ins >> 12) {
  case 0x0:
    switch (ins) {
    case 0x00E0: // Clear screen
      clear_framebuffer();
      break;
    }
    break;

  case 0x1:
    PC = NNN(ins);
    break;

  case 0x6:
    REGS[X(ins)] = NN(ins);
    break;

  case 0x7:
    REGS[X(ins)] += NN(ins);
    break;

  case 0xA:
    I = NNN(ins);
    break;

  case 0xD:
    REGS[0xF] = display_sprite(REGS[X(ins)], REGS[Y(ins)], N(ins), &RAM[I]);
    draw_framebuffer();
    break;
  }
}