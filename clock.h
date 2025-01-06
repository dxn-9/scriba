
#ifndef _CLOCK_H
#define _CLOCK_H

typedef struct
{
    uint64_t time, delta_time; // Time and delta time in milliseconds.
} Clock;

void update_clock();
extern Clock app_clock;

#endif // _CLOCK_H