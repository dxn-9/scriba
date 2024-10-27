#include "cursor.h"
#include "text.h"
#include "clock.h"
#include "cursor.h"
#include "utils.h"
#include <stdbool.h>

SDL_Color red = {255, 0, 0, 255};
SDL_Color blue = {0, 0, 255, 255};

// Gets the index of the character before the cursor
int get_buffer_index_prev(Cursor *cursor, TextBuffer *buffer)
{
    if (cursor->x == 0)
        return get_buffer_index(cursor, buffer) - 1;

    Cursor temp_cursor = {
        .x = cursor->x - 1,
        .y = cursor->y};
    return get_buffer_index(&temp_cursor, buffer);
}
int get_buffer_index(Cursor *cursor, TextBuffer *buffer)
{
    Line *line = get_line_at(buffer, cursor->y);
    const char *data = &((char *)buffer->text.data)[line->bytes_offset];
    const char **ptr = &data;
    int count = cursor->x;
    size_t bytes_until_end = strlen(*ptr); // TODO: This is probably not the efficent way to count the bytes.

    while (count > 0)
    {
        SDL_StepUTF8(ptr, NULL);
        count--;
    }

    return line->bytes_offset + bytes_until_end - strlen(*ptr);
}
void cursor_move_up(Cursor *cursor, TextBuffer *buffer, Selection *selection)
{
    cursor->y = MAX(0, cursor->y - 1);
    cursor->x = MIN(cursor->x, get_line_length(buffer, cursor->y));

    if (selection != NULL)
    {
        selection_update(selection, buffer, cursor);
    }
}
void cursor_move_down(Cursor *cursor, TextBuffer *buffer, Selection *selection)
{
    cursor->y = MIN(cursor->y + 1, buffer->lines.length - 1);
    cursor->x = MIN(get_line_length(buffer, cursor->y), cursor->x);
    if (selection != NULL)
    {
        selection_update(selection, buffer, cursor);
    }
}
void cursor_move_left(Cursor *cursor, TextBuffer *buffer, Selection *selection)
{
    cursor->x = MAX(0, cursor->x - 1);
    if (selection != NULL)
    {
        selection_update(selection, buffer, cursor);
    }
}
void cursor_move_start_line(Cursor *cursor, TextBuffer *buffer, Selection *selection)
{
    cursor->x = 0;

    if (selection != NULL)
    {
        selection_update(selection, buffer, cursor);
    }
}

void cursor_move_end_line(Cursor *cursor, TextBuffer *buffer, Selection *selection)
{
    cursor->x = get_line_length(buffer, cursor->y);
    if (selection != NULL)
    {
        selection_update(selection, buffer, cursor);
    }
}
void cursor_move_right(Cursor *cursor, TextBuffer *buffer, Selection *selection)
{

    cursor->x = MIN(get_line_length(buffer, cursor->y), cursor->x + 1);
    if (selection != NULL)
    {
        selection_update(selection, buffer, cursor);
    }
}
void render_cursor(SDL_Renderer *renderer, Cursor *cursor, SDL_FRect offset)
{
    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);

    bool is_odd = (appClock.time / RECTANGLE_BLINK) % 2 == 0;
    SDL_Color color = is_odd ? red : blue;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    int chars_out_of_view_x = -offset.x / cursor->w;
    int chars_out_of_view_y = -offset.y / cursor->h;
    SDL_FRect rect = {(cursor->x - chars_out_of_view_x) * cursor->w, (cursor->y - chars_out_of_view_y) * cursor->h, cursor->w, cursor->h};
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
