

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

// enum SelectAction
// {
//     Nothing,
//     CancelSelection,
//     Delete,
//     Replace,
//     Move,

// } SelectAction;

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

extern TTF_Font *font;
extern const char *initial_text;
void clean_text(TextBuffer *buffer);
extern int char_w_;
extern int char_h_;

TextBuffer text_new(Cursor *cursor, const char *initialStr);
void text_remove_char(Context *ctx);
void text_newline(Context *ctx);
void text_add(Context *ctx, const char *str);
int get_line_length(TextBuffer *buffer, int line);
void clean_text(TextBuffer *buffer);
bool init_text();

void handle_copy(Context *ctx);
void handle_paste(Context *ctx);

void selection_cancel(Context *ctx);
void selection_start(Context *ctx);
void selection_delete(Context *ctx);
void selection_update(Context *ctx);

void render_buffer(SDL_Renderer *renderer, TextBuffer *text, SDL_FRect view_offset);
void render_selection(SDL_Renderer *renderer, Selection *selection, TextBuffer *text, SDL_FRect view_offset);

#endif // _TEXT_H