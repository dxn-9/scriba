
#include "cursor.h"
#include "text.h"
#include "clock.h"
#include "cursor.h"
#include "utils.h"
#include <stdbool.h>

SDL_Color red = {255, 0, 0, 255};
SDL_Color blue = {0, 0, 255, 255};

int get_buffer_index(Cursor *cursor, TextBuffer *buffer)
{
    return get_line_at(buffer, cursor->y)->start + cursor->x;
}
void cursor_move_up(Cursor *cursor, TextBuffer *buffer)
{
    cursor->y = MAX(0, cursor->y - 1);
    cursor->x = MIN(cursor->x, get_line_length(buffer, cursor->y));
}
void cursor_move_down(Cursor *cursor, TextBuffer *buffer)
{
    cursor->y = MIN(cursor->y + 1, buffer->lines.length - 1);
    cursor->x = MIN(get_line_length(buffer, cursor->y), cursor->x);
}
void cursor_move_left(Cursor *cursor, TextBuffer *buffer)
{
    cursor->x = MAX(0, cursor->x - 1);
}
void cursor_move_start_line(Cursor *cursor, TextBuffer *buffer)
{
    cursor->x = 0;
}

void cursor_move_end_line(Cursor *cursor, TextBuffer *buffer)
{
    cursor->x = get_line_length(buffer, cursor->y);
}
void cursor_move_right(Cursor *cursor, TextBuffer *buffer)
{
    cursor->x = MIN(get_line_length(buffer, cursor->y), cursor->x + 1);
}
void render_cursor(SDL_Renderer *renderer, Cursor *cursor)
{
    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);

    bool is_odd = (appClock.time / RECTANGLE_BLINK) % 2 == 0;
    SDL_Color color = is_odd ? red : blue;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    // printf("CursorX: %i\n", cursor->x);
    SDL_FRect rect = {cursor->x * cursor->w, cursor->y * cursor->h, cursor->w, cursor->h};
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

Cursor new_cursor(int x, int y, int char_w, int char_h)
{
    Cursor cursor = {
        .x = x,
        .y = y,
        .w = char_w,
        .h = char_h};
    return cursor;
}
