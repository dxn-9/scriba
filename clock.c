#include <SDL3/SDL.h>
#include "clock.h"

Clock appClock;

void update_clock()
{
    int now = SDL_GetTicks();
    appClock.delta_time = now - appClock.time;
    appClock.time = now;
}