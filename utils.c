#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "text.h"
#include "constants.h"
#include <sys/stat.h>
#include <errno.h>

bool read_or_create_file(char *filename, Editor *editor)
{

    SDL_IOStream *stream = NULL;
    struct stat sb;
    if (stat(filename, &sb) == -1)
    {
        if (errno == ENOENT)
        {
            stream = SDL_IOFromFile(filename, "w+");
        }
        else
        {
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        stream = SDL_IOFromFile(filename, "r+");
    }

    if (stream == NULL)
    {
        printf("SDL_IOFromFile::%s\n", SDL_GetError());
        return false;
    }

    // When need to copy the string, in case the char* filename gets freed
    char *owned_filename = calloc(strlen(filename) + 1, sizeof(char));
    strcpy(owned_filename, filename);
    Sint64 file_size = SDL_GetIOSize(stream);
    char *data = malloc(file_size);
    SDL_ReadIO(stream, data, file_size);
    SDL_CloseIO(stream);

    editor->filename = owned_filename;
    editor->cursor = new_cursor(0, 0);
    editor->buffer = text_new(data);
    editor->selection = (Selection){.is_active = false};
    free(data);
    return true;
}

Vector2I get_cursor_pos_from_screen(float x, float y, SDL_FRect last_view_offset)
{

    Vector2I vec = {.x = 0, .y = 0};
    float line_number_offset = get_line_number_offset_text();
    int line_scroll_y = SDL_fabs(last_view_offset.y / application.char_h);
    int line_scroll_x = SDL_fabs((last_view_offset.x - line_number_offset) / application.char_w);

    int pos_x = ((x - line_number_offset) / application.char_w) + line_scroll_x;
    vec.x = pos_x;
    vec.y = (y / application.char_h) + line_scroll_y;
    return vec;
}

int get_line_number_offset()
{
    return application.char_w * LINE_NUMBER_SPACE;
}
int get_line_number_offset_text()
{
    return get_line_number_offset() + TEXT_PADDING;
}

int get_view_whitespace(char *text, int size)
{
    int tabs_num = 0;

    for (int i = 0; i < size; ++i)
    {
        if (text[i] == '\t')
            tabs_num++;
    }

    return tabs_num * TABS_VIEW_SIZE;
}

SDL_FRect get_view_offset(SDL_FRect previous_offset, Cursor *cursor,
                          bool should_focus_cursor, int max_v_lines, int max_h_lines,
                          float scroll_x, float scroll_y)
{
    SDL_FRect offset = previous_offset;

    int screen_chars_x = (application.win_w - get_line_number_offset_text()) / application.char_w;
    int screen_chars_y = application.win_h / application.char_h;

    int offset_x = SDL_abs((previous_offset.x - get_line_number_offset_text()) / application.char_w);
    int offset_y = SDL_abs((previous_offset.y / application.char_h));

    int min_x = offset_x + HORIZONTAL_VIEW_OFFSET;
    int max_x = min_x + screen_chars_x - (2 * HORIZONTAL_VIEW_OFFSET);

    int min_y = offset_y + VERTICAL_VIEW_OFFSET;
    int max_y = min_y + screen_chars_y - (2 * VERTICAL_VIEW_OFFSET);

    if (should_focus_cursor)
    {
        if (cursor->view_x > max_x)
        {
            offset.x -= (cursor->view_x - max_x) * application.char_w;
        }
        if (MAX(cursor->view_x, HORIZONTAL_VIEW_OFFSET) < min_x)
        {
            offset.x += SDL_abs(cursor->view_x - min_x) * application.char_w;
        }
        if (cursor->y > max_y)
        {
            offset.y -= (cursor->y - max_y) * application.char_h;
        }
        if (MAX(cursor->y, VERTICAL_VIEW_OFFSET) < min_y)
        {
            offset.y += SDL_abs(cursor->y - min_y) * application.char_h;
        }
    }

    offset.y += scroll_y;

    offset.x = MIN(offset.x, get_line_number_offset_text());
    offset.y = MIN(offset.y, 0.0);
    offset.y = MAX(offset.y, -SDL_abs(((max_v_lines + VERTICAL_VIEW_OFFSET) - screen_chars_y) * application.char_h));

    return offset;
}

void debug_rect(SDL_FRect *rect)
{
    printf("Rect: [x:%f,y:%f,w:%f,h:%f]\n", rect->x, rect->y, rect->w, rect->h);
}

void order_selection(Selection *selection)
{
    if (selection->buffer_end < selection->buffer_start)
    {
        size_t t = selection->buffer_end;
        size_t t_x = selection->end_x;
        size_t t_y = selection->end_y;

        selection->buffer_end = selection->buffer_start;
        selection->end_x = selection->start_x;
        selection->end_y = selection->start_y;

        selection->buffer_start = t;
        selection->start_x = t_x;
        selection->start_y = t_y;
    }
}
void clear_selection_text(Selection *selection, TextBuffer *buffer, Cursor *cursor)
{
    order_selection(selection);
    int selection_start_x = selection->start_x;
    int selection_start_y = selection->start_y;
    selection_delete(selection, buffer);
    cursor_set_y(cursor, buffer, selection_start_y);
    cursor_set_x(cursor, buffer, selection_start_x);
}
void render_text(char *text, SDL_Color color, int x, int y)
{
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, 0, color);
    if (surface == NULL)
    {
        printf("Error creating surface in render_text: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(application.renderer, surface);
    if (texture == NULL)
    {
        printf("Error creating texture in render_text: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_PropertiesID properties = SDL_GetTextureProperties(texture);
    int texW = SDL_GetNumberProperty(properties, SDL_PROP_TEXTURE_WIDTH_NUMBER, 0);
    int texH = SDL_GetNumberProperty(properties, SDL_PROP_TEXTURE_HEIGHT_NUMBER, 0);

    if (!properties)
    {
        printf("QueryTexture Failed %s", SDL_GetError());
        exit(1);
    }
    SDL_FRect dest_rectangle = {
        x, y, texW, texH};

    if (!SDL_RenderTexture(application.renderer, texture, NULL, &dest_rectangle))
    {
        printf("RenderCopy failed: %s", SDL_GetError());
        exit(1);
    }

    SDL_DestroySurface(surface);
    SDL_DestroyTexture(texture);
}

void render_rectangle(SDL_Color color, SDL_FRect rect)
{
    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(application.renderer, &r, &g, &b, &a);
    SDL_SetRenderDrawColor(application.renderer, color.r, color.g, color.b, color.a);
    SDL_RenderRect(application.renderer, &rect);
    SDL_SetRenderDrawColor(application.renderer, r, g, b, a);
}
void render_fill_rectangle(SDL_Color color, SDL_FRect rect)
{
    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(application.renderer, &r, &g, &b, &a);
    SDL_SetRenderDrawColor(application.renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(application.renderer, &rect);
    SDL_SetRenderDrawColor(application.renderer, r, g, b, a);
}

SDL_FRect selection_rect(float x, float y, float w, SDL_FRect *offset)
{
    SDL_FRect rect = {
        .x = x * application.char_w + offset->x,
        .y = y * application.char_h + offset->y,
        .w = w * application.char_w,
        .h = application.char_h};
    return rect;
}
// TODO: I can probably do a bitmask to see the size of the next character. For now i'll do it the inefficent way.
size_t utf8_char_bytes_at(TextBuffer *buffer, size_t position)
{
    const char *data = &((char *)buffer->text.data)[position];
    const char **ptr = &data;
    size_t l = strlen(*ptr);
    SDL_StepUTF8(ptr, NULL);
    return l - strlen(*ptr);
}
Line *get_line_at(TextBuffer *buffer, int y)
{
    return ((Line *)buffer->lines.data + y);
}