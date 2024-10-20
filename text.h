

#ifndef _TEXT_H
#define _TEXT_H

#include "vector.h"
#include <stdbool.h>
#include <SDL2/SDL_ttf.h>
#include "cursor.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define DEFAULT_CAPACITY 16
#define FONT_SIZE 24

typedef struct TextBuffer
{
    Vector text;  // Contains the text buffer
    Vector lines; // Contains the indices of the line breaks.

} TextBuffer;

extern TTF_Font *font;
extern char *initial_text;
void clean_text(TextBuffer *buffer);
extern int char_w_;
extern int char_h_;

TextBuffer text_new(char *initialStr);
void text_newline(TextBuffer *buffer, Cursor *cursor);
void text_append(TextBuffer *buffer, char *str);
void render_text(SDL_Renderer *renderer, TextBuffer *text);
void clean_text(TextBuffer *buffer);
bool init_text();

#endif // _TEXT_H