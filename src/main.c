#include <libopencm3/stm32/rcc.h>
#include "pcd8544.h"
#include "timer.h"

void setup(void);
void loop(void);
void setPixelTest(void);

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
}

void setPixelTest()
{
    static uint8_t x = 0;
    static uint8_t y = 0;
    TimerSleep(20);
    LcdSetPixel(x, y);
    x++;
    if(x > 83) {
	x=0;
	y++;
    }
    if(y > 47)
	y=0;

    LcdFlipBuffer();
}

void loop(void)
{
    setPixelTest();
}
