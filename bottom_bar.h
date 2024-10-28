#ifndef _BOTTOM_BAR_H
#define _BOTTOM_BAR_H
#include <SDL3/SDL.h>
#include "cursor.h"

typedef struct
{
    float fps;
    Cursor *cursor;
    Selection *selection;

} BottomBar;

extern BottomBar bottom_bar;

void render_bottom_bar(SDL_Renderer *renderer, int win_w, int win_h);
#endif