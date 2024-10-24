#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cursor.h"
#include "text.h"
#include "utils.h"
int char_w_, char_h_;
TTF_Font *font;
// const char *initial_text = "ắ";
const char *initial_text = "ắ\nheắắắllo!ắ";

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
        printf("%s ptr \n", ptr);
    } while (SDL_StepUTF8(&ptr, NULL) != 0);
}

TextBuffer text_new(Cursor *cursor, const char *initial_str)
{
    TextBuffer buffer = {
        .text = vector_new(sizeof(char)),
        .lines = vector_new(sizeof(Line))};

    int pos = 0;
    int line_start = 0;
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
int get_line_length(TextBuffer *buffer, int line)
{
    Line *data = get_line_at(buffer, 0);

    if (line >= buffer->lines.length || line < 0)
    {
        printf("Warning: out of bound access attempt\n");
        return -1;
    }

    // Subtract 1 since we don't want to count newlines.
    return data[line].end - data[line].start - 1;
}
void text_newline(TextBuffer *buffer, Cursor *cursor)
{
    text_add(buffer, cursor, "\n");
    recompute_lines(buffer);
    cursor_move_down(cursor, buffer);
    cursor_move_start_line(cursor, buffer);
}

void text_add(TextBuffer *buffer, Cursor *cursor, const char *str)
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
}

// FIXME: this is bad for performance. we're basically creating a new texture every frame
// for each line. It should be cached.
void render_text(SDL_Renderer *renderer, TextBuffer *buffer)
{
    Vector text = buffer->text;

    if (text.length > 0)
    {
        for (int i = 0; i < buffer->lines.length; i++)
        {
            Line *line = get_line_at(buffer, i);
            if (line->bytes == 0)
                continue;

            // char text[size];
            // memcpy(text, ((char *)buffer->text.data) + line->start, size * buffer->text.element_size);
            SDL_Surface *surface = TTF_RenderText_Blended(font, (char *)buffer->text.data + line->bytes_offset, line->bytes, (SDL_Color){0, 0, 0});

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

            if (!SDL_RenderTexture(renderer, texture, NULL, &(SDL_FRect){0, i * texH, texW, texH}))
            {
                printf("RenderCopy failed: %s", SDL_GetError());
                exit(1);
            }

            SDL_DestroySurface(surface);
            SDL_DestroyTexture(texture);
        }
    }
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