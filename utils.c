#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdlib.h>

#include "utils.h"
#include "text.h"

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
void render_text(SDL_Renderer *renderer, char *text, SDL_Color color, int x, int y)
{
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, 0, color);
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

    if (!properties)
    {
        printf("QueryTexture Failed %s", SDL_GetError());
        exit(1);
    }
    SDL_FRect dest_rectangle = {
        x, y, texW, texH};

    if (!SDL_RenderTexture(renderer, texture, NULL, &dest_rectangle))
    {
        printf("RenderCopy failed: %s", SDL_GetError());
        exit(1);
    }

    SDL_DestroySurface(surface);
    SDL_DestroyTexture(texture);
}

void render_fill_rectangle(SDL_Renderer *renderer, SDL_Color color, SDL_FRect rect)
{
    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

SDL_FRect selection_rect(float x, float y, float w, SDL_FRect *offset)
{
    SDL_FRect rect = {
        .x = x * char_w_ + offset->x,
        .y = y * char_h_ + offset->y,
        .w = w * char_w_,
        .h = char_h_};
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