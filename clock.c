#include <SDL2/SDL.h>
#include "clock.h"

Clock clock;

void update_clock()
{
    int now = SDL_GetTicks();
    clock.delta_time = now - clock.time;
    clock.time = now;
}