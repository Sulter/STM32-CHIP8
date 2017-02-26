#ifndef TIMER_H
#define TIMER_H
#include <stdint.h>

typedef struct {
    volatile uint32_t millis_count;
    volatile uint32_t millis_interrupt;
    void (*timer_callback)(void);
} timer_struct;

/*
  Think a bit about what could go wrong with TimerRemove. Or if the structure is lost (out of scope et.c)
  The current implementation with pointers is not very safe.
 */


void timer_init(uint32_t cpuFrequency);
void timer_sleep(uint32_t ms);
void timer_add(timer_struct *t, uint8_t timerNr);
void timer_remove(uint8_t timerNr);
uint32_t timer_get_total(void);

#endif
