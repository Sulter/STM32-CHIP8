#include <libopencm3/stm32/rcc.h>
#include "pcd8544.h"

void sleep(uint16_t);
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

void sleep(uint16_t ms)
{
    uint32_t sleeptime = 16800 * ms;
    for (uint32_t  i= 0; i < sleeptime; i++) {
	__asm__("nop");
    }
}

void setup(void)
{
    /*
      The main PLL will run at 168 MHz, APB1 at 42, APB2 at 84 MHz.
     */
    rcc_clock_setup_hse_3v3(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);

    LcdInitialise();
}

void setPixelTest()
{
    static uint8_t x = 0;
    static uint8_t y = 0;
    sleep(20);
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
