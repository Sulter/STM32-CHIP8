#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/nvic.h>
#include <stddef.h>
#include "timer.h"

#define TOTAL_TIMERS 5

volatile uint32_t systemTimeCount;

Timer *timerList[TOTAL_TIMERS];


uint32_t TimerGetTotal(void)
{
    return systemTimeCount;
}

void TimerAdd(Timer *t, uint8_t timerNr)
{
    if(timerNr < TOTAL_TIMERS) {
	t->millisCount = 0;
	timerList[timerNr] = t;
    }
}

void TimerRemove(uint8_t timerNr)
{
    if(timerNr < TOTAL_TIMERS) {
	timerList[timerNr] = NULL;
    }
}

void TimerSleep(uint32_t ms)
{
    uint32_t wait = systemTimeCount + ms;
    while (wait > systemTimeCount);
}

//ISR
void sys_tick_handler(void)
{
    //handle timers
    for(uint8_t i = 0; i < TOTAL_TIMERS; i++) {
	Timer *t = timerList[i];
	if(t != NULL) {
	    if(t->millisCount > t->millisInterrupt) {
		t->timerCallback();
		t->millisCount = 0;
	    }
	    else {
		t->millisCount++;
	    }
	}
    }

    //increase the total counter
    systemTimeCount++;
}

void TimerInit(uint32_t cpuFrequency)
{
    //ensure null pointers
    for(uint8_t i = 0; i < TOTAL_TIMERS; i++) {
	timerList[i] = NULL;
    }

    //ms resolution
    systick_set_reload(cpuFrequency / 1000);
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
    systick_counter_enable();

    systick_interrupt_enable();
}
