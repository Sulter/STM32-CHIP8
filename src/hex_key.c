#include "hex_key.h"
#include <stdint.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#define GPIO_PORT GPIOA

#define GPIO_R_0 GPIO0
#define GPIO_R_1 GPIO1
#define GPIO_R_2 GPIO2
#define GPIO_R_3 GPIO3

#define GPIO_C_0 GPIO4
#define GPIO_C_1 GPIO5
#define GPIO_C_2 GPIO6
#define GPIO_C_3 GPIO7

typedef enum {
    key_1, key_2, key_3, key_a,
    key_4, key_5, key_6, key_b,
    key_7, key_8, key_9, ket_c,
    key_0, key_f, key_e, key_d
} hex_key_types;

static bool hex_keys[16] = {0};
static uint16_t hex_col[] = {GPIO_C_0, GPIO_C_1, GPIO_C_2, GPIO_C_3};
static uint16_t hex_row[] = {GPIO_R_0, GPIO_R_1, GPIO_R_2, GPIO_R_3};
static uint8_t current_c = 0;

bool hex_check_key(uint8_t key)
{
    return hex_keys[key];
}

void hex_key_cycle(void)
{
    //each row
    for(uint8_t i = 0; i < 4; i++) {
	uint8_t current_key = current_c * 4 + i;
	hex_keys[current_key] = gpio_get(GPIO_PORT, hex_row[i]);
    }

    gpio_clear(GPIO_PORT, hex_col[current_c]);

    current_c++;
    if(current_c >= 4) {
	current_c = 0;
    }

    gpio_set(GPIO_PORT, hex_col[current_c]);
}

void hex_key_init(void)
{
    rcc_periph_clock_enable(RCC_GPIOA);

    //inputs
    gpio_mode_setup(GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN,
		    GPIO_R_0 | GPIO_R_1 | GPIO_R_2 | GPIO_R_3);
    gpio_set_output_options(GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ,
			    GPIO_R_0 | GPIO_R_1 | GPIO_R_2 | GPIO_R_3);
    //output
    gpio_mode_setup(GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
		    GPIO_C_0 | GPIO_C_1 | GPIO_C_2 | GPIO_C_3);
    gpio_set_output_options(GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ,
			    GPIO_C_0 | GPIO_C_1 | GPIO_C_2 | GPIO_C_3);
}

