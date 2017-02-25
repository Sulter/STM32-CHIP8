#ifndef RNG_H
#define RNG_H

/**
 *These functions are not yet part of libopencm3, but might be in future versions
 */

#include <stdint.h>
#include <stdbool.h>

#include <libopencm3/stm32/memorymap.h>
#include <libopencm3/cm3/common.h>
#include <libopencm3/stm32/f4/rng.h>

void rng_enable(void);
void rng_disable(void);
bool rng_get_random(uint32_t *rand_nr);
uint32_t rng_get_random_blocking(void);

#endif
