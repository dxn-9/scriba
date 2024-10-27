
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

SDL_FRect selection_rect(float x, float y, float w, SDL_FRect *offset);
Line *get_line_at(TextBuffer *buffer, int y);
size_t utf8_char_bytes_at(TextBuffer *buffer, size_t position);

#endif