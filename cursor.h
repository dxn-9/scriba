#ifndef _CURSOR_H
#define _CURSOR_H

#include <SDL3/SDL.h>

#define RECTANGLE_BLINK 500 // ms

typedef struct TextBuffer TextBuffer; // forward declare since this also will be included in text.h
typedef struct Selection Selection;
typedef struct Editor Editor;

typedef struct Cursor
{
    int x, y;
    int view_x; // How much visually it's shifted to the right. This is useful for handling spaces and tabs.

} Cursor;

int get_buffer_index_prev(Cursor *cursor, TextBuffer *buffer);
int get_buffer_index(Cursor *cursor, TextBuffer *buffer);
void cursor_update_view_x(Cursor *cursor, TextBuffer *buffer);
void cursor_set_x(Cursor *cursor, TextBuffer *buffer, int value);
void cursor_set_y(Cursor *cursor, TextBuffer *buffer, int value);
void cursor_move_down(Cursor *cursor, TextBuffer *buffer);
void cursor_move_up(Cursor *cursor, TextBuffer *buffer);
void cursor_move_right(Cursor *cursor, TextBuffer *buffer);
void cursor_move_left(Cursor *cursor, TextBuffer *buffer);
void cursor_move_start_line(Cursor *cursor, TextBuffer *buffer);
void cursor_move_end_line(Cursor *cursor, TextBuffer *buffer);
void cursor_move_to_selection_start(Cursor *cursor, Selection *selection);
void render_cursor(Cursor *cursor, SDL_FRect offset);
void cursor_set_from_buffer_index(Cursor *cursor, TextBuffer *buffer, int buffer_index);
void debug_cursor(Cursor *cursor);
Cursor copy_cursor(Cursor *cursor);
Cursor new_cursor(int x, int y);

#endif // _CURSOR_H