#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/nvic.h>
#include <stddef.h>
#include "timer.h"

#define TOTAL_TIMERS 5

volatile uint32_t system_ms_count;

static timer_struct *timer_list[TOTAL_TIMERS];


uint32_t timer_get_total(void)
{
    return system_ms_count;
}

void timer_add(timer_struct *t, uint8_t timerNr)
{
    if(timerNr < TOTAL_TIMERS) {
	t->millis_count = 0;
	timer_list[timerNr] = t;
    }
}

void timer_remove(uint8_t timerNr)
{
    if(timerNr < TOTAL_TIMERS) {
	timer_list[timerNr] = NULL;
    }
}

void timer_sleep(uint32_t ms)
{
    uint32_t wait = system_ms_count + ms;
    while (wait > system_ms_count);
}

//ISR
void sys_tick_handler(void)
{
    //handle timers
    for(uint8_t i = 0; i < TOTAL_TIMERS; i++) {
	timer_struct *t = timer_list[i];
	if(t != NULL) {
	    if(t->millis_count > t->millis_interrupt) {
		t->timer_callback();
		t->millis_count = 0;
	    }
	    else {
		t->millis_count++;
	    }
	}
    }

    //increase the total counter
    system_ms_count++;
}

void timer_init(uint32_t cpuFrequency)
{
    //ensure null pointers
    for(uint8_t i = 0; i < TOTAL_TIMERS; i++) {
	timer_list[i] = NULL;
    }

    //ms resolution
    systick_set_reload(cpuFrequency / 1000);
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
    systick_counter_enable();

    systick_interrupt_enable();
}
