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
void cursor_move_up(Context *ctx)
{
    ctx->cursor.y = MAX(0, ctx->cursor.y - 1);
    ctx->cursor.x = MIN(ctx->cursor.x, get_line_length(&ctx->buffer, ctx->cursor.y));
    if (ctx->selection.is_active)
    {
        selection_update(ctx);
    }
}
void cursor_move_down(Context *ctx)
{
    ctx->cursor.y = MIN(ctx->cursor.y + 1, ctx->buffer.lines.length - 1);
    ctx->cursor.x = MIN(get_line_length(&ctx->buffer, ctx->cursor.y), ctx->cursor.x);
    if (ctx->selection.is_active)
    {
        selection_update(ctx);
    }
}
void cursor_move_left(Context *ctx)
{

    ctx->cursor.x = MAX(0, ctx->cursor.x - 1);
    if (ctx->selection.is_active)
    {
        selection_update(ctx);
    }
}
void cursor_move_start_line(Context *ctx)
{
    ctx->cursor.x = 0;

    if (ctx->selection.is_active)
    {
        selection_update(ctx);
    }
}

void cursor_move_end_line(Context *ctx)
{
    ctx->cursor.x = get_line_length(&ctx->buffer, ctx->cursor.y);
    if (ctx->selection.is_active)
    {
        selection_update(ctx);
    }
}
void cursor_move_right(Context *ctx)
{
    ctx->cursor.x = MIN(get_line_length(&ctx->buffer, ctx->cursor.y), ctx->cursor.x + 1);

    if (ctx->selection.is_active)
    {
        selection_update(ctx);
    }
}
void render_cursor(SDL_Renderer *renderer, Cursor *cursor, SDL_FRect offset)
{
    Uint8 r, g, b, a;
    bool is_odd = (app_clock.time / RECTANGLE_BLINK) % 2 == 0;
    SDL_Color color = is_odd ? red : blue;
    int chars_out_of_view_x = -offset.x / cursor->w;
    int chars_out_of_view_y = -offset.y / cursor->h;
    SDL_FRect rect = {(cursor->x - chars_out_of_view_x) * cursor->w, (cursor->y - chars_out_of_view_y) * cursor->h, cursor->w, cursor->h};
    render_fill_rectangle(renderer, color, rect);
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
