
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "text.h"
#include <stdbool.h>

int char_w_, char_h_;
TTF_Font *font;
char *initial_text = "Hello";
TextBuffer main_buffer;

TextBuffer text_new(char *initialStr)
{
    TextBuffer buffer;
    int len = strlen(initialStr);
    int capacity = MAX(len, 16);
    buffer.text = malloc(sizeof(char) * capacity);
    memset(buffer.text, 0, sizeof(char) * capacity);
    if (initialStr != NULL)
    {
        strcat(buffer.text, initialStr);
    }
    buffer.capacity = capacity;
    buffer.length = len;
    return buffer;
}

void text_append(TextBuffer *buffer, char *str)
{

    size_t len = strlen(str);
    // printf("text append! BufferLen: %i BufferCapacity: %i, strLen: %i, content: %s\n", buffer->length, buffer->capacity, len, buffer->text);
    if (len + buffer->length + 1 >= buffer->capacity)
    {
        // printf("Making buffer bigger! %i - current: %i\n", len, buffer->length);
        size_t s = sizeof(char) * (buffer->capacity * 2);
        char *t = malloc(s);
        memset(t, 0, s); // Clear the memory so that strcat works correctly
        memcpy(t, buffer->text, buffer->length * sizeof(char));
        free(buffer->text);
        buffer->text = t;
        buffer->capacity = s;
    }
    buffer->length += len;
    strcat(buffer->text, str);
}

void render_text(SDL_Renderer *renderer)
{
    if (main_buffer.length > 0)
    {
        // FIXME: this is bad for performance. we're basically creating a new texture every frame
        // for each line
        char display_buffer[main_buffer.length + 1];
        strcpy(display_buffer, main_buffer.text);
        char *token = strtok(display_buffer, "\n");
        int token_count = 0;
        while (token != NULL)
        {
            // printf("RenderText::%s, len:%i\n", str, strlen(str));
            SDL_Surface *surface = TTF_RenderUTF8_Blended(font, token, (SDL_Color){0, 0, 0});

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

            int texW, texH;
            if (SDL_QueryTexture(texture, NULL, NULL, &texW, &texH) < 0)
            {
                printf("QueryTexture Failed %s", SDL_GetError());
                return;
            }

            if (SDL_RenderCopy(renderer, texture, NULL, &(SDL_Rect){0, token_count * texH, texW, texH}) < 0)
            {
                printf("RenderCopy failed: %s", SDL_GetError());
                return;
            }

            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
            token = strtok(NULL, "\n");
            token_count++;
        }
    }
}

void clean_text()
{
    free(main_buffer.text);
    TTF_CloseFont(font);
}
bool init_text()
{
    main_buffer = text_new(initial_text);
    font = TTF_OpenFont("ttf/JetBrainsMono-Regular.ttf", FONT_SIZE);

    SDL_Surface *glyph = TTF_RenderGlyph_Blended(font, '?', (SDL_Color){0});
    char_w_ = glyph->w;
    char_h_ = glyph->h;
    SDL_FreeSurface(glyph);

    if (font == NULL)
    {
        printf("Failed to load font: %s", TTF_GetError());
        return false;
    }
    return true;
}