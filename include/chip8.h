#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

#define CHIP8_WIDTH 64
#define CHIP8_HEIGHT 32
#define CHIP8_MEMSIZE 4096

typedef uint8_t (*chip8_screen_t)[CHIP8_HEIGHT];

void chip8_init(const uint8_t *rom, uint16_t size);
void chip8_step(void);
chip8_screen_t chip8_get_screen_buffer(void);
void chip8_timer_ISR(void);

#endif
