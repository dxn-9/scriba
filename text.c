#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cursor.h"
#include "text.h"
#include "constants.h"
#include "utils.h"
int char_w_, char_h_;
TTF_Font *font;
TTF_Font *sm_font;
// const char *initial_text = "ắ";
const char *initial_text = "ắ\nheắắe\neee\neeeee\nee\neee";

void recompute_lines(TextBuffer *buffer)
{
    char *data = buffer->text.data;
    if (buffer->lines.data != NULL)
    {
        vector_free(&buffer->lines);
    }
    buffer->lines = vector_new(sizeof(Line));
    const char *ptr = data;
    int line_start = 0;
    int i = 0;
    int tot_bytes = strlen(ptr);

    // using the lord's loop. lets go.
    do
    {
        if (ptr[0] == '\n' || ptr[0] == '\0')
        {
            int bytes_offset = 0;

            // Check if its not the first element
            if (line_start != 0)
            {
                // If it's not, the last inserted line will have the byte offset (the number of bytes before this line)
                Line *last_line = &((Line *)buffer->lines.data)[buffer->lines.length - 1];
                bytes_offset = last_line->bytes_offset + last_line->bytes + 1; // +1 for new line or \0
            }
            int offset = 0;
            // We don't want to count the newline in the bytes
            if (ptr[0] == '\n')
                offset = 1;

            size_t bytes = tot_bytes - strlen(ptr);
            tot_bytes = strlen(ptr) - offset;

            Line line = {
                .start = line_start,
                .end = i + 1, // line n is [firstchar..firstchar of n+1)
                .bytes = bytes,
                .bytes_offset = bytes_offset};

            line_start = i + 1;
            vector_push(&buffer->lines, &line);
        }
        i++;
    } while (SDL_StepUTF8(&ptr, NULL) != 0);
}

TextBuffer text_new(Cursor *cursor, const char *initial_str)
{

    TextBuffer buffer = {
        .text = vector_new(sizeof(char)),
        .lines = vector_new(sizeof(Line))};

    int pos = 0;
    while (initial_str[pos] != '\0')
    {
        vector_push(&buffer.text, &initial_str[pos]);
        pos++;
    }
    vector_push(&buffer.text, "\0");
    recompute_lines(&buffer);
    debug_vec(&buffer.text);
    debug_vec(&buffer.lines);
    return buffer;
}
void text_remove_char(TextBuffer *buffer, Cursor *cursor)
{
    bool is_newline = false;
    int index = get_buffer_index_prev(cursor, buffer);
    char *text = (char *)buffer->text.data;

    if (index < 0)
        return;

    if (text[index] == '\n')
        is_newline = true;

    if (is_newline)
    {
        cursor_move_up(cursor, buffer);
        cursor_move_end_line(cursor, buffer);
        vector_remove(&buffer->text, index);
    }
    else
    {
        size_t char_size = utf8_char_bytes_at(buffer, index);
        printf("charsize: %zi String: %c\n", char_size, text[index]);
        Range range = {
            .start = index,
            .end = index + char_size,
        };
        vector_remove_range(&buffer->text, range);
        cursor_move_left(cursor, buffer);
    }

    recompute_lines(buffer);
}
int get_line_view_length(TextBuffer *buffer, int line)
{
    SDL_assert(line < buffer->lines.length || line >= 0);

    Line *data = get_line_at(buffer, line);
    int white_space = calculate_view_whitespace((char *)buffer->text.data + data->bytes_offset, data->bytes);
    int line_length = get_line_length(buffer, line);
    int tabs_count = white_space / TABS_VIEW_SIZE; // TODO: if there are other whitespace sources, this doesnt work.

    return line_length - tabs_count + white_space;
}

int get_line_length(TextBuffer *buffer, int line)
{
    SDL_assert(line < buffer->lines.length || line >= 0);

    Line *data = get_line_at(buffer, line);

    // Subtract 1 since we don't want to count newlines.
    return data->end - data->start - 1;
}

int text_add(TextBuffer *buffer, Cursor *cursor, const char *str)
{
    size_t len = strlen(str);
    int buffer_index = get_buffer_index(cursor, buffer);
    for (int i = 0; i < len; ++i)
    {
        vector_add(&buffer->text, buffer_index + i, &str[i]);
    }
    recompute_lines(buffer);
    debug_vec(&buffer->text);
    debug_vec(&buffer->lines);
    return buffer_index + len;
}

// Replaces special characters (such as \t) to other characters.
// Returns a \0 terminated string.
// The pointer returned should be freed.
char *get_text_to_render(char *text, int size)
{
    int tabs = 0;
    for (int i = 0; i < size; ++i)
    {
        if (text[i] == '\t')
            tabs++;
    }

    size_t text_size = size + tabs * (TABS_VIEW_SIZE - 1) + 1;
    char *result = calloc(text_size, sizeof(char));

    int inserted_tabs = 0;

    for (int i = 0; i < size; ++i)
    {
        int index = i + inserted_tabs * (TABS_VIEW_SIZE - 1);
        if (text[i] == '\t')
        {
            // Insert visually 4 spaces instead of 1 tab
            for (int j = 0; j < TABS_VIEW_SIZE; j++)
                result[index + j] = ' ';

            inserted_tabs++;
        }
        else
        {
            result[index] = text[i];
        }
    }

    return result;
}

// FIXME: this is bad for performance. we're basically creating a new texture every frame
// for each line. It should be cached.
void render_buffer(SDL_Renderer *renderer, TextBuffer *buffer, SDL_FRect view_offset)
{
    Vector text = buffer->text;

    if (text.length > 0)
    {
        for (int i = 0; i < buffer->lines.length; i++)
        {
            Line *line = get_line_at(buffer, i);
            if (line->bytes == 0)
                continue;

            char *text_render = get_text_to_render((char *)buffer->text.data + line->bytes_offset, line->bytes);
            SDL_Surface *surface = TTF_RenderText_Blended(font, text_render, 0, (SDL_Color){0, 0, 0});
            free(text_render);

            if (surface == NULL)
            {
                printf("Error creating surface in render_text: %s\n", SDL_GetError());
                // printf("Text info: %s %i", text, size);
                exit(1);
            }
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (texture == NULL)
            {
                printf("Error creating texture in render_text: %s\n", SDL_GetError());
                exit(1);
            }

            SDL_PropertiesID properties = SDL_GetTextureProperties(texture);
            int texW = SDL_GetNumberProperty(properties, SDL_PROP_TEXTURE_WIDTH_NUMBER, 0);
            int texH = SDL_GetNumberProperty(properties, SDL_PROP_TEXTURE_HEIGHT_NUMBER, 0);
            // printf("TextureProperties: %i %i\n", texW, texH);

            if (!properties)
            {
                printf("QueryTexture Failed %s", SDL_GetError());
                exit(1);
            }
            SDL_FRect dest_rectangle = {
                view_offset.x, i * texH + view_offset.y, texW, texH};

            if (!SDL_RenderTexture(renderer, texture, NULL, &dest_rectangle))
            {
                printf("RenderCopy failed: %s", SDL_GetError());
                exit(1);
            }

            SDL_DestroySurface(surface);
            SDL_DestroyTexture(texture);
        }
    }
}

void render_selection(SDL_Renderer *renderer, Selection *selection, TextBuffer *buffer, SDL_FRect view_offset)
{
    if (!selection->is_active)
        return;
    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
    SDL_Color color = {
        .r = 211, .g = 164, .b = 227};
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    Vector rects = vector_new(sizeof(SDL_FRect));

    int y_diff = selection->end_y - selection->start_y;
    SDL_FRect rect;
    if (y_diff == 0)
    {
        rect = selection_rect(selection->start_x, selection->start_y,
                              ((float)selection->end_x - selection->start_x), &view_offset);
        vector_push(&rects, &rect);
    }
    else if (y_diff < 0)
    {
        rect = selection_rect(0, selection->start_y, selection->start_x, &view_offset);
        vector_push(&rects, &rect);
        for (int i = selection->end_y + 1; i < selection->start_y; ++i)
        {
            rect = selection_rect(0, i, MAX(get_line_view_length(buffer, i), 1), &view_offset);
            vector_push(&rects, &rect);
        }

        rect = selection_rect(selection->end_x, selection->end_y,
                              get_line_view_length(buffer, selection->end_y) - selection->end_x,
                              &view_offset);
        vector_push(&rects, &rect);
    }
    else
    {
        // y_diff is positive >
        rect = selection_rect(selection->start_x, selection->start_y,
                              MAX(get_line_view_length(buffer, selection->start_y) - selection->start_x,
                                  1),
                              &view_offset);
        vector_push(&rects, &rect);

        for (int i = selection->start_y + 1; i < selection->end_y; ++i)
        {
            rect = selection_rect(0, i, MAX(get_line_view_length(buffer, i), 1),
                                  &view_offset);
            vector_push(&rects, &rect);
        }
        rect = selection_rect(0, selection->end_y, selection->end_x, &view_offset);
        vector_push(&rects, &rect);
    }

    for (int i = 0; i < rects.length; ++i)
    {
        SDL_FRect *rect = &((SDL_FRect *)rects.data)[i];
        SDL_RenderFillRect(renderer, rect);
    }
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    vector_free(&rects);
}

void clean_text(TextBuffer *buffer)
{
    vector_free(&buffer->lines);
    vector_free(&buffer->text);
    TTF_CloseFont(font);
}
bool init_text()
{
    font = TTF_OpenFont("ttf/JetBrainsMono-Regular.ttf", FONT_SIZE);
    SDL_Surface *glyph = TTF_RenderGlyph_Blended(font, '?', (SDL_Color){0});
    char_w_ = glyph->w;
    char_h_ = glyph->h;
    SDL_DestroySurface(glyph);

    if (font == NULL)
    {
        printf("Failed to load font: %s", SDL_GetError());
        return false;
    }
    return true;
}

int handle_paste(Selection *selection, TextBuffer *buffer, Cursor *cursor)
{
    char *text = SDL_GetClipboardText();
    if (strcmp(text, "") == 0)
    {
        printf("ClipboardPaste::%s\n", SDL_GetError());
        free(text);
        return 0;
    }

    if (selection->is_active)
    {
        selection_delete(selection, buffer);
    }

    int index = text_add(buffer, cursor, text);
    free(text);
    return index;
}
void handle_cut(Selection *selection, TextBuffer *buffer)
{
    if (!selection->is_active)
        return;

    handle_copy(selection, buffer);
    selection_delete(selection, buffer);
}
void handle_copy(Selection *selection, TextBuffer *buffer)
{
    if (!selection->is_active)
        return;

    int selection_size = selection->buffer_end - selection->buffer_start;
    char *text = calloc(selection_size + 1, sizeof(char));
    memcpy(text, (char *)buffer->text.data + selection->buffer_start, selection_size);

    if (!SDL_SetClipboardText(text))
    {
        printf("ClipboardCopy::%s\n", SDL_GetError());
    }
    free(text);
}

void selection_start(Selection *selection, Cursor *cursor, TextBuffer *buffer)
{
    size_t buffer_start = get_buffer_index(cursor, buffer);
    printf("StartingSelection: %i\n", cursor->y);

    selection->buffer_start = buffer_start;
    selection->buffer_end = buffer_start;
    selection->start_x = cursor->x;
    selection->start_y = cursor->y;
    selection->end_x = cursor->x;
    selection->end_y = cursor->y;
    selection->is_active = true;
}

void selection_cancel(Selection *selection)
{
    selection->is_active = false;
}

void selection_delete(Selection *selection, TextBuffer *buffer)
{

    order_selection(selection);
    size_t start = selection->buffer_start;
    size_t end = selection->buffer_end;
    Range range = {.start = start, .end = end};
    vector_remove_range(&buffer->text, range);
    recompute_lines(buffer);
    selection_cancel(selection);
}

void selection_update(Selection *selection, Cursor *cursor, TextBuffer *buffer)
{
    if (selection->is_active)
    {
        size_t index = get_buffer_index(cursor, buffer);
        selection->end_x = cursor->view_x;
        selection->end_y = cursor->y;
        selection->buffer_end = index;
    }
}
