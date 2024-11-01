#include "bottom_bar.h"
#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#define BOTTOM_BAR_H 20
BottomBar bottom_bar = {
    .cursor = NULL,
    .selection = NULL,
    .fps = 0};

void render_selection_bar(SDL_Renderer *renderer, int bar_y)
{
    if (bottom_bar.selection == NULL || !bottom_bar.selection->is_active)
        return;

    char selection_str[20];
    sprintf(selection_str, "c:%i", abs((int)bottom_bar.selection->end_x - (int)bottom_bar.selection->start_x));
    render_text(renderer, selection_str, (SDL_Color){0, 255, 0, 255}, 200, bar_y);
}
void render_cursor_position(SDL_Renderer *renderer, int bar_y)
{
    if (bottom_bar.cursor == NULL)
        return;

    char cursor_str[20];
    sprintf(cursor_str, "x:%i y:%i", bottom_bar.cursor->x, bottom_bar.cursor->y);
    render_text(renderer, cursor_str, (SDL_Color){0, 255, 0, 255}, 100, bar_y);
}

void render_fps_counter(SDL_Renderer *renderer, int bar_y)
{
    char fps_str[40];

    sprintf(fps_str, "FPS:%0.f", bottom_bar.fps);
    render_text(renderer, fps_str, (SDL_Color){0, 255, 0, 255}, 0, bar_y);
}

void render_bottom_bar(SDL_Renderer *renderer, int win_w, int win_h)
{
    SDL_Color color = {100, 100, 100, 255};
    int bar_y = win_h - BOTTOM_BAR_H;
    SDL_FRect rect = {0, bar_y, win_w, BOTTOM_BAR_H};
    render_fill_rectangle(renderer, color, rect);
    render_fps_counter(renderer, bar_y);
    render_cursor_position(renderer, bar_y);
    render_selection_bar(renderer, bar_y);
}