#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cursor.h"
#include "text.h"

int char_w_, char_h_;
TTF_Font *font;
char *initial_text = "Hello";

TextBuffer text_new(char *initial_str)
{
    TextBuffer buffer = {
        .text = vector_new(sizeof(char)),
        .lines = vector_new(sizeof(int))};

    text_append(&buffer, initial_str);

    return buffer;
}
void text_newline(TextBuffer *buffer, Cursor *cursor)
{
    vector_push(&buffer->lines, &cursor->x);
}

void text_append(TextBuffer *buffer, const char *str)
{
    size_t len = strlen(str);
    for (int i = 0; i < len; ++i)
    {
        vector_push(&buffer->text, &str[i]);
    }
}

// FIXME: this is bad for performance. we're basically creating a new texture every frame
// for each line. It should be cached.
void render_text(SDL_Renderer *renderer, TextBuffer *buffer)
{
    Vector text = buffer->text;
    Vector lines = buffer->lines;
    printf("RenderText::%zu\n", text.length);

    if (text.length > 0)
    {
        int left = 0;
        int right = 0;
        for (int i = 0; i <= buffer->lines.length; i++)
        {
            if (i == lines.length)
            {
                right = text.length;
            }
            else
            {
                right = ((int *)lines.data)[i];
            }

            int size = right - left;
            if (size == 0)
                continue;
            char text[size + 1];
            memcpy(text, &((char *)buffer->text.data)[left], size * buffer->text.element_size);
            text[size] = '\0';
            // printf("RenderText::%s, len:%i\n", str, strlen(str));
            printf("RenderText::Size %i\n", size);
            printf("RenderText::Text %s\n", text);

            SDL_Surface *surface = TTF_RenderText_Blended(font, text, strlen(text), (SDL_Color){0, 0, 0});

            if (surface == NULL)
            {
                printf("Error creating surface in render_text: %s\n", SDL_GetError());
                return;
            }
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (texture == NULL)
            {
                printf("Error creating texture in render_text: %s\n", SDL_GetError());
                return;
            }

            SDL_PropertiesID properties = SDL_GetTextureProperties(texture);
            int texW = SDL_GetNumberProperty(properties, SDL_PROP_TEXTURE_WIDTH_NUMBER, 0);
            int texH = SDL_GetNumberProperty(properties, SDL_PROP_TEXTURE_HEIGHT_NUMBER, 0);
            printf("TextureProperties: %i %i\n", texW, texH);

            if (!properties)
            {
                printf("QueryTexture Failed %s", SDL_GetError());
                return;
            }

            if (!SDL_RenderTexture(renderer, texture, NULL, &(SDL_FRect){0, i * texH, texW, texH}))
            {
                printf("RenderCopy failed: %s", SDL_GetError());
                return;
            }

            SDL_DestroySurface(surface);
            SDL_DestroyTexture(texture);
            left = right;
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