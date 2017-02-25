#include <libopencm3/stm32/rcc.h>
#include <stdio.h>
#include "pcd8544.h"
#include "timer.h"
#include "usb_cdc.h"
#include "chip8.h"
#include "rng.h"

void setup(void);
void loop(void);
void set_pixel_test(void);
void chip8_loop(void);
void chip8_draw(void);

Timer chip8_loop_timer, chip8_timers_timer;
uint8_t chip8ROM[] = {0xC0, 0x1f, 0xA0, 0x32, 0xD1, 0x05};

int main(void)
{
    setup();

    for(;;) {
	loop();
    }

    return 0;
}

void setup(void)
{
    /*
      The main PLL will run at 168 MHz, APB1 at 42, APB2 at 84 MHz.
     */
    rcc_clock_setup_hse_3v3(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);

    //init individual drivers/modules
    rng_enable();
    LcdInitialise();
    TimerInit(168000000);
    usb_init();
    chip8_init(chip8ROM, 6);

    //init timers
    chip8_loop_timer.millisInterrupt = 1;
    chip8_loop_timer.timerCallback = chip8_loop;
    TimerAdd(&chip8_loop_timer, 1);

    chip8_timers_timer.millisInterrupt = 170; //~6Hz
    chip8_timers_timer.timerCallback = chip8_timer_ISR;
    TimerAdd(&chip8_timers_timer, 2);
}

void chip8_loop(void)
{
    chip8_step();

    chip8_draw();
}

void chip8_draw(void)
{
    chip8_screen_t chip8_screen_ptr;

    chip8_screen_ptr = chip8_get_screen_buffer();
    for(int x = 0; x < CHIP8_WIDTH; x++) {
	for(int y = 0; y < CHIP8_HEIGHT; y++) {
	    if(chip8_screen_ptr[x][y])
		LcdSetPixel(x, y);
	}
    }
    LcdFlipBuffer();
}

void set_pixel_test()
{
    static uint8_t x = 0;
    static uint8_t y = 0;

    LcdSetPixel(x, y);
    char buf[64];
    uint8_t len = usb_get(buf);
    if(len) {
	switch (buf[0]) {
	    case 'a':
		x--;
		break;
	    case 'w':
		y--;
		break;
	    case 's':
		y++;
		break;
	    case 'd':
		x++;
		printf("Moving in x++ direction\r\n");
		break;
	    default:
		break;
	}
    }

    LcdFlipBuffer();
}

void loop(void)
{
    usb_poll();
}
