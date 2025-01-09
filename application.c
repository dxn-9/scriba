#include <SDL3/SDL.h>
#include "application.h"

Application application;

void update_time()
{
    int now = SDL_GetTicks();
    application.delta_time = now - application.time;
    application.time = now;
}