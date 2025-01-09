

#ifndef _TEXT_H
#define _TEXT_H

#include "vector.h"
#include <stdbool.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "cursor.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) > (y)) ? (y) : (x))
#define DEFAULT_CAPACITY 16
#define FONT_SIZE 18
#define FONT_SIZE_SM 10

typedef struct
{
    int start;           // index of first character of the line
    int end;             // index of the character after the newline
    size_t bytes;        // bytes in the line (it could not equal the end-start because utf8 encoding)
    size_t bytes_offset; // bytes before start
} Line;

typedef struct TextBuffer
{
    Vector text;  // Contains the text bytes
    Vector lines; // Contains the indices of the line breaks.

} TextBuffer;

typedef struct Selection
{
    bool is_active;
    size_t buffer_start; // Positions in the buffer
    size_t buffer_end;   // Positions in the buffer
    size_t start_x;      // Screen space start x
    size_t start_y;      // Screen space start y
    size_t end_x;        // Screen space end x
    size_t end_y;        // Screen space end y

} Selection;

typedef struct Context Context;

extern int max_horizontal_characters;
extern TTF_Font *font;
extern const char *initial_text;
void clean_text(TextBuffer *buffer);

TextBuffer text_new(const char *initialStr);
void text_remove_char(TextBuffer *buffer, Cursor *cursor);
int text_add(TextBuffer *buffer, Cursor *cursor, const char *str); // Returns the last index in the buffer of the text that was inserted
int get_line_length(TextBuffer *buffer, int line);
int get_line_view_length(TextBuffer *buffer, int line); // Returns a number that accounts for tabs and white space.
void clean_text(TextBuffer *buffer);
bool init_text();

void handle_copy(Selection *selection, TextBuffer *buffer);
int handle_paste(Selection *selection, TextBuffer *buffer, Cursor *cursor); // Returns the last index in the buffer of the text that was pasted
void handle_cut(Selection *selection, TextBuffer *buffer);

void selection_cancel(Selection *selection);
void selection_start(Selection *selection, Cursor *cursor, TextBuffer *buffer);
void selection_delete(Selection *selection, TextBuffer *buffer);
void selection_update(Selection *selection, Cursor *cursor, TextBuffer *buffer);

void render_buffer(SDL_Renderer *renderer, TextBuffer *text, SDL_FRect view_offset);
void render_selection(SDL_Renderer *renderer, Selection *selection, TextBuffer *text, SDL_FRect view_offset);

#endif // _TEXT_H