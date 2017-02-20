#ifndef TIMER_H
#define TIMER_H
#include <stdint.h>

typedef struct {
    volatile uint32_t millisCount;
    volatile uint32_t millisInterrupt;
    void (*timerCallback)(void);
} Timer;

/*
  Think a bit about what could go wrong with TimerRemove. Or if the structure is lost (out of scope et.c)
  The current implementation with pointers is not very safe.
 */


void TimerInit(uint32_t cpuFrequency);
void TimerSleep(uint32_t ms);
void TimerAdd(Timer *t, uint8_t timerNr);
void TimerRemove(uint8_t timerNr);
uint32_t TimerGetTotal(void);

#endif
