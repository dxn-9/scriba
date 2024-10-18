
#ifndef _TEXT_H
#define _TEXT_H

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define DEFAULT_CAPACITY 16
#define FONT_SIZE 24

#include <stdbool.h>
#include <SDL2/SDL_ttf.h>

typedef struct
{
    char *text;
    int capacity;
    int length;

} TextBuffer;

extern TTF_Font *font;
extern char *initial_text;
extern TextBuffer main_buffer;
extern int char_w_;
extern int char_h_;

TextBuffer text_new(char *initialStr);
void text_append(TextBuffer *buffer, char *str);
void render_text(SDL_Renderer *renderer);
void clean_text();
bool init_text();

#endif // _TEXT_H