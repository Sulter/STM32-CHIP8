#include <libopencm3/stm32/rcc.h>
#include "rng.h"

void rng_disable(void)
{
    RNG_CR &= ~RNG_CR_RNGEN;
}

/** Enable the Random Number Generator peripheral.
 */
void rng_enable(void)
{
    rcc_periph_clock_enable(RCC_RNG);
    RNG_CR |= RNG_CR_RNGEN;
}

/** Randomizes a number (non-blocking).
 * Can fail if a clock error or seed error is detected. Consult the Reference
 * Manual, but "try again", potentially after resetting the peripheral
 * @param pointer to a uint32_t that will be randomized.
 * @returns true on success, pointer is only written to on success
 * @sa rng_get_random_blocking
 */
bool rng_get_random(uint32_t *rand_nr)
{
    /* data ready */
    if (!(RNG_SR & RNG_SR_DRDY)) {
	return false;
    }

    /* Check for errors */
    if (RNG_SR & (RNG_SR_CECS | RNG_SR_SECS)) {
	return false;
    }

    *rand_nr = RNG_DR;

    return true;
}


/**
 * Get a random number and block until it works.
 * Unless you have a clock problem, this should always return "promptly"
 * If you have a clock problem, you will wait here forever!
 * @returns a random 32bit number
 */
uint32_t rng_get_random_blocking(void)
{
    uint32_t rv;
    bool done;
    do {
	if (RNG_SR & RNG_SR_SECS) {
	    rng_disable();
	    rng_enable();
	}
	done = rng_get_random(&rv);
    } while (!done);

    return rv;
}
