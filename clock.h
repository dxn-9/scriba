
#ifndef _CLOCK_H
#define _CLOCK_H

typedef struct
{
    int time, delta_time;
} Clock;

void update_clock();
extern Clock appClock;

#endif // _CLOCK_H