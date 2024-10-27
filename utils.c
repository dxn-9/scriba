#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"

#include "utils.h"
#include "text.h"

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