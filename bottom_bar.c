#include "bottom_bar.h"
#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#define BOTTOM_BAR_H 20

void render_selection_bar(SDL_Renderer *renderer, Context *context, int bar_y)
{
    SDL_assert(&context->selection != NULL);
    if (!context->selection.is_active)
        return;

    char selection_str[20];
    sprintf(selection_str, "c:%i", abs((int)context->selection.end_x - (int)context->selection.start_x));
    render_text(renderer, selection_str, (SDL_Color){0, 255, 0, 255}, 200, bar_y);
}
void render_cursor_position(SDL_Renderer *renderer, Context *context, int bar_y)
{
    SDL_assert(&context->cursor != NULL);

    char cursor_str[20];
    sprintf(cursor_str, "x:%i y:%i", context->cursor.x, context->cursor.y);
    render_text(renderer, cursor_str, (SDL_Color){0, 255, 0, 255}, 100, bar_y);
}

void render_fps_counter(SDL_Renderer *renderer, Context *context, int bar_y)
{
    char fps_str[40];

    sprintf(fps_str, "FPS:%0.f", context->fps);
    render_text(renderer, fps_str, (SDL_Color){0, 255, 0, 255}, 0, bar_y);
}

void render_file_name(SDL_Renderer *renderer, Context *context, int bar_y, int win_w)
{
    int x = win_w - (strlen(context->file_name) + 1) * context->cursor.w;
    render_text(renderer, context->file_name, (SDL_Color){0, 255, 0, 255}, x, bar_y);
}

void render_bottom_bar(SDL_Renderer *renderer, Context *context, int win_w, int win_h)
{
    SDL_Color color = {100, 100, 100, 255};
    int bar_y = win_h - BOTTOM_BAR_H;
    SDL_FRect rect = {0, bar_y, win_w, BOTTOM_BAR_H};
    bar_y -= 5;
    render_fill_rectangle(renderer, color, rect);
    render_fps_counter(renderer, context, bar_y);
    render_cursor_position(renderer, context, bar_y);
    render_selection_bar(renderer, context, bar_y);
    render_file_name(renderer, context, bar_y, win_w);
}