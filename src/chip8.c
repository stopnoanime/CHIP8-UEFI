#include "chip8.h"
#include "display.h"
#include "font.h"
#include "input.h"

static uint8_t RAM[RAM_SIZE];
static uint8_t REGS[16];

static uint16_t PC;
static uint16_t I;

static uint16_t STACK[STACK_SIZE];
static uint8_t STACK_POINTER;

static uint8_t DELAY_TIMER;
static uint8_t SOUND_TIMER;

static void push(uint16_t val) { STACK[STACK_POINTER++] = val; }

static uint16_t pop(uint16_t val) { return STACK[--STACK_POINTER]; }

static void add(uint8_t X, uint8_t Y) {
  uint16_t res = REGS[X] + REGS[Y];
  REGS[X] = res;
  REGS[0xF] = res > 0xFF;
}

static void sub(uint8_t X, uint8_t A, uint8_t B) {
  int16_t res = REGS[A] - REGS[B];
  REGS[X] = res;
  REGS[0xF] = res >= 0;
}

// random ;)
static uint8_t random() {
  static uint32_t seed = 1;

  seed ^= seed << 13;
  seed ^= seed >> 17;
  seed ^= seed << 5;

  return seed;
}

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
  uint8_t tmp;
  PC += 2;

  static bool in_FX0A = false;
  switch (ins >> 12) {
  case 0x0:
    if (ins == 0x00E0)
      clear_framebuffer();
    else if (ins == 0x00EE)
      PC = pop(ins);
    break;

  case 0x1:
    PC = NNN(ins);
    break;

  case 0x2:
    push(PC);
    PC = NNN(ins);
    break;

  case 0x3:
    if (REGS[X(ins)] == NN(ins))
      PC += 2;
    break;

  case 0x4:
    if (REGS[X(ins)] != NN(ins))
      PC += 2;
    break;

  case 0x5:
    if (REGS[X(ins)] == REGS[Y(ins)])
      PC += 2;
    break;

  case 0x6:
    REGS[X(ins)] = NN(ins);
    break;

  case 0x7:
    REGS[X(ins)] += NN(ins);
    break;

  case 0x8:
    switch (N(ins)) {
    case 0x0:
      REGS[X(ins)] = REGS[Y(ins)];
      break;
    case 0x1:
      REGS[X(ins)] |= REGS[Y(ins)];
      break;
    case 0x2:
      REGS[X(ins)] &= REGS[Y(ins)];
      break;
    case 0x3:
      REGS[X(ins)] ^= REGS[Y(ins)];
      break;
    case 0x4:
      add(X(ins), Y(ins));
      break;
    case 0x5:
      sub(X(ins), X(ins), Y(ins));
      break;
    case 0x6:
      tmp = REGS[X(ins)] & 1;
      REGS[X(ins)] >>= 1;
      REGS[0xF] = tmp;
      break;
    case 0x7:
      sub(X(ins), Y(ins), X(ins));
      break;
    case 0xE:
      tmp = REGS[X(ins)] >> 7;
      REGS[X(ins)] <<= 1;
      REGS[0xF] = tmp;
      break;
    }
    break;

  case 0x9:
    if (REGS[X(ins)] != REGS[Y(ins)])
      PC += 2;
    break;

  case 0xA:
    I = NNN(ins);
    break;

  case 0xB:
    PC = NNN(ins) + REGS[0];
    break;

  case 0xC:
    REGS[X(ins)] = random() & NN(ins);
    break;

  case 0xD:
    REGS[0xF] = display_sprite(REGS[X(ins)], REGS[Y(ins)], N(ins), &RAM[I]);
    break;

  case 0xE:
    if (NN(ins) == 0x9E) {
      if (key_pressed(REGS[X(ins)]))
        PC += 2;
    } else if (NN(ins) == 0xA1) {
      if (!key_pressed(REGS[X(ins)]))
        PC += 2;
    }
    break;

  case 0xF:
    switch (NN(ins)) {
    case 0x07:
      REGS[X(ins)] = DELAY_TIMER;
      break;
    case 0x15:
      DELAY_TIMER = REGS[X(ins)];
      break;
    case 0x18:
      SOUND_TIMER = REGS[X(ins)];
      break;

    case 0x1E:
      I += REGS[X(ins)];
      break;

    case 0x0A:
      if (!in_FX0A) {
        in_FX0A = true;
        reset_released();
      }

      if (any_released(&REGS[X(ins)]))
        in_FX0A = false;
      else
        PC -= 2;
      break;

    case 0x29:
      I = (REGS[X(ins)] & 0xF) * 5;
      break;

    case 0x33:
      RAM[I] = REGS[X(ins)] / 100;
      RAM[I + 1] = (REGS[X(ins)] / 10) % 10;
      RAM[I + 2] = REGS[X(ins)] % 10;
      break;

    case 0x55:
      for (int i = 0; i <= X(ins); i++)
        RAM[I + i] = REGS[i];
      break;

    case 0x65:
      for (int i = 0; i <= X(ins); i++)
        REGS[i] = RAM[I + i];
      break;
    }
    break;
  }
}

void chip8_timer_60HZ() {
  if (DELAY_TIMER > 0)
    DELAY_TIMER--;
  if (SOUND_TIMER > 0)
    SOUND_TIMER--;
}
