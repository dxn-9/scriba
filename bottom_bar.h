#ifndef _BOTTOM_BAR_H
#define _BOTTOM_BAR_H
#include <SDL3/SDL.h>
#include "cursor.h"

#define BOTTOM_BAR_H 24
#define BOTTOM_BAR_PADDING_X 10
#define TEMP_BOTTOM_BAR_COMMAND_X 300 // this should be calculated

void render_bottom_bar(SDL_Renderer *renderer, Context *context);
void render_list_editors(SDL_Renderer *renderer);
#endif