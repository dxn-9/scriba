#include <SDL3/SDL.h>
#include "clock.h"

Clock app_clock;

void update_clock()
{
    int now = SDL_GetTicks();
    app_clock.delta_time = now - app_clock.time;
    app_clock.time = now;
}