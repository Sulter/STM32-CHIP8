#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdbool.h>

#define CHIP8_WIDTH 64
#define CHIP8_HEIGHT 32
#define CHIP8_MEMSIZE 4096
#define CHIP8_NUMBER_OF_KEYS 16

typedef uint8_t (*chip8_screen_t)[CHIP8_HEIGHT];
typedef enum {
    k_0, k_1, k_2, k_3,
    k_4, k_5, k_6, k_7,
    k_8, k_9, k_a, k_b,
    k_c, k_d, k_e, k_f
} chip8_key_types;


void chip8_init(const uint8_t *rom, uint16_t size);
void chip8_step(void);
chip8_screen_t chip8_get_screen_buffer(void);
void chip8_timer_ISR(void);
void chip8_set_key(chip8_key_types key, bool is_down);

#endif
