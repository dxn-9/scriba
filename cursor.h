#ifndef _CURSOR_H
#define _CURSOR_H

#include <SDL2/SDL.h>

#define RECTANGLE_BLINK 500 // ms

typedef struct TextBuffer TextBuffer; // forward declare since this also will be included in text.h

typedef struct Cursor
{
    int x, y;
    int w, h;
} Cursor;

void cursor_move_down(Cursor *cursor, TextBuffer *buffer);
void cursor_move_up(Cursor *cursor, TextBuffer *buffer);
void cursor_move_right(Cursor *cursor, TextBuffer *buffer);
void cursor_move_left(Cursor *cursor, TextBuffer *buffer);
void render_cursor(SDL_Renderer *renderer, Cursor *cursor);
Cursor new_cursor(int x, int y, int char_w, int char_h);
extern Cursor cursor;

#endif // _CURSOR_H