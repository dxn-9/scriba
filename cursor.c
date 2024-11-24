#include <stdbool.h>
#include <stdio.h>

#include "cursor.h"
#include "text.h"
#include "clock.h"
#include "cursor.h"
#include "utils.h"
#include "constants.h"

SDL_Color red = {255, 0, 0, 255};
SDL_Color blue = {0, 0, 255, 255};

void cursor_move_to_selection_start(Cursor *cursor, Selection *selection)
{
    cursor->x = selection->start_x;
    cursor->y = selection->start_y;
}
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
void cursor_update_view_x(Cursor *cursor, TextBuffer *buffer)
{
    Line *line = get_line_at(buffer, cursor->y);
    const char *buff = ((char *)buffer->text.data) + line->bytes_offset;
    const char **ptr = &buff;

    int new_lines_in_line = 0;
    for (int i = 0; i < cursor->x; ++i, SDL_StepUTF8(ptr, NULL))
    {
        if (*ptr[0] == '\t')
            new_lines_in_line++;
    }

    cursor->view_x = cursor->x + new_lines_in_line * (TABS_VIEW_SIZE - 1);
}
void cursor_set_x(Cursor *cursor, TextBuffer *buffer, int value)
{
    cursor->x = MIN(value, get_line_length(buffer, cursor->y));
    cursor_update_view_x(cursor, buffer);
}
void cursor_set_y(Cursor *cursor, TextBuffer *buffer, int value)
{
    SDL_assert(buffer->lines.length > value);
    cursor->y = value;
}
void cursor_move_up(Cursor *cursor, TextBuffer *buffer)
{
    cursor->y = MAX(0, cursor->y - 1);
    cursor_set_x(cursor, buffer, MIN(cursor->x, get_line_length(buffer, cursor->y)));
}
void cursor_move_down(Cursor *cursor, TextBuffer *buffer)
{
    cursor->y = MIN(cursor->y + 1, buffer->lines.length - 1);
    cursor_set_x(cursor, buffer, MIN(get_line_length(buffer, cursor->y), cursor->x));
}
void cursor_move_left(Cursor *cursor, TextBuffer *buffer)
{

    cursor_set_x(cursor, buffer, MAX(0, cursor->x - 1));
}
void cursor_move_start_line(Cursor *cursor, TextBuffer *buffer)
{
    cursor_set_x(cursor, buffer, 0);
}

void cursor_move_end_line(Cursor *cursor, TextBuffer *buffer)
{
    cursor_set_x(cursor, buffer, get_line_length(buffer, cursor->y));
}
void cursor_move_right(Cursor *cursor, TextBuffer *buffer)
{

    cursor_set_x(cursor, buffer, MIN(get_line_length(buffer, cursor->y), cursor->x + 1));
}

void render_cursor(SDL_Renderer *renderer, Cursor *cursor, SDL_FRect offset)
{
    bool is_odd = (app_clock.time / RECTANGLE_BLINK) % 2 == 0;
    SDL_Color color = is_odd ? red : blue;
    SDL_FRect rect = {(cursor->view_x * cursor->w) + offset.x, (cursor->y * cursor->h) + offset.y, CURSOR_VIEW_SIZE, cursor->h};
    render_fill_rectangle(renderer, color, rect);
}

void cursor_set_from_buffer_index(Cursor *cursor, TextBuffer *buffer, int index)
{
    SDL_assert(index < buffer->text.length);

    for (int i = 0; i < buffer->lines.length; ++i)
    {
        Line *line = get_line_at(buffer, i);
        if (line->bytes_offset + line->bytes >= index)
        {
            int x = 0;
            const char *data = ((char *)buffer->text.data) + line->bytes_offset;
            char *target = ((char *)buffer->text.data) + index;
            const char **ptr = &data;

            while (data != target)
            {
                SDL_StepUTF8(ptr, NULL);
                x++;
            }
            cursor->y = i;
            cursor->x = x;

            break;
        }
    }
    cursor_update_view_x(cursor, buffer);
}

void debug_cursor(Cursor *cursor)
{
    printf("Cursor: x:%i y:%i vX: %i\n", cursor->x, cursor->y, cursor->view_x);
}

Cursor copy_cursor(Cursor *cursor)
{
    Cursor c = {
        .x = cursor->x,
        .y = cursor->y,
        .w = cursor->w,
        .h = cursor->h,
        .view_x = cursor->view_x};

    return c;
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
