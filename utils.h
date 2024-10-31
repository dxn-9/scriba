
#include "text.h"
#include "SDL3/SDL.h"

#ifndef _UTILS_H
#define _UTILS_H

typedef struct Context
{
    char *file_name;
    Cursor cursor;
    TextBuffer buffer;
    Selection selection;

} Context;

SDL_FRect calculate_view_offset(SDL_FRect previous_offset, int win_w, int win_h, Cursor *cursor);
// Renders a null terminated string at the given position
void render_text(SDL_Renderer *renderer, char *text, SDL_Color color, int x, int y);
void render_fill_rectangle(SDL_Renderer *renderer, SDL_Color color, SDL_FRect rect);
// Order the selection such that the start is always smaller than the end.
void order_selection(Selection *selection);
SDL_FRect selection_rect(float x, float y, float w, SDL_FRect *offset);
Line *get_line_at(TextBuffer *buffer, int y);
size_t utf8_char_bytes_at(TextBuffer *buffer, size_t position);

#endif