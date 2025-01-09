
#include "text.h"
#include "application.h"
#include "SDL3/SDL.h"

#ifndef _UTILS_H
#define _UTILS_H

typedef struct Vector2I
{
    int x, y;
} Vector2I;

typedef struct Context
{
    char *filename;
    Cursor cursor;
    TextBuffer buffer;
    Selection selection;
    bool focus_cursor;

} Context;

bool read_or_create_file(char *filename, Context *context);
Vector2I get_cursor_pos_from_screen(float x, float y, SDL_FRect last_view_offset);
int get_line_number_offset();
int get_line_number_offset_text();             // Adds a some padding for text area
int get_view_whitespace(char *text, int size); // Calculates how much whitespace there is in the given string of size
SDL_FRect get_view_offset(SDL_FRect previous_offset, Cursor *cursor, bool should_focus_cursor, int max_v_lines, int max_h_lines, float scroll_x, float scroll_y);
// Renders a null terminated string at the given position
void render_text(SDL_Renderer *renderer, char *text, SDL_Color color, int x, int y);
void render_fill_rectangle(SDL_Renderer *renderer, SDL_Color color, SDL_FRect rect);
// Order the selection such that the start is always smaller than the end.
void debug_rect(SDL_FRect *rect);
void order_selection(Selection *selection);
void clear_selection_text(Selection *selection, TextBuffer *buffer, Cursor *cursor);
SDL_FRect selection_rect(float x, float y, float w, SDL_FRect *offset);
Line *get_line_at(TextBuffer *buffer, int y);
size_t utf8_char_bytes_at(TextBuffer *buffer, size_t position);

#endif