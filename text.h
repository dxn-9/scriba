

#ifndef _TEXT_H
#define _TEXT_H

#include "vector.h"
#include <stdbool.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "cursor.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) > (y)) ? (y) : (x))
#define DEFAULT_CAPACITY 16
#define FONT_SIZE 24

typedef struct
{
    int start;           // index of first character of the line
    int end;             // index of the character after the newline
    size_t bytes;        // bytes in the line (it could not equal the end-start because utf8 encoding)
    size_t bytes_offset; // bytes before start
} Line;

typedef struct TextBuffer
{
    Vector text;    // Contains the text bytes
    Vector lines;   // Contains the indices of the line breaks.
    int characters; // Actual characters, (utf 8)

} TextBuffer;

extern TTF_Font *font;
extern const char *initial_text;
void clean_text(TextBuffer *buffer);
extern int char_w_;
extern int char_h_;

TextBuffer text_new(Cursor *cursor, const char *initialStr);
void text_remove_char(TextBuffer *buffer, Cursor *cursor);
void text_newline(TextBuffer *buffer, Cursor *cursor);
void text_add(TextBuffer *buffer, Cursor *cursor, const char *str);
int get_line_length(TextBuffer *buffer, int line);
void render_text(SDL_Renderer *renderer, TextBuffer *text);
void clean_text(TextBuffer *buffer);
bool init_text();

#endif // _TEXT_H