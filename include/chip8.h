#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

#define CHIP8_WIDTH 64
#define CHIP8_HEIGHT 32

void chip8_init(uint8_t *rom, uint16_t size);
void chip8_step(void);
uint8_t** chip8_get_screen_buffer(void);

#endif
