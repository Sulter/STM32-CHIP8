#include <libopencm3/stm32/rcc.h>
#include <stdio.h>
#include "pcd8544.h"
#include "timer.h"
#include "usb_cdc.h"

void setup(void);
void loop(void);
void setPixelTest(void);

Timer pixelTestTimer;

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

    LcdInitialise();
    TimerInit(168000000);
    usb_init();

    pixelTestTimer.millisInterrupt = 20;
    pixelTestTimer.timerCallback = setPixelTest;
    TimerAdd(&pixelTestTimer, 1);
}

void setPixelTest()
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
