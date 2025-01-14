
#ifndef _APPLICATION_H
#define _APPLICATION_H
#include "utils.h"

typedef enum
{
    Insert,
    Command,
    List,
} Mode;

typedef struct
{
    uint64_t time, delta_time; // Time and delta time in milliseconds.
    double fps;
    char *cwd;
    Mode mode;
    TextBuffer command_buffer;
    Cursor command_buffer_cursor;
    Vector editors;
    int current_editor; // Index of the current editor
    int char_w, char_h; // Width and height of glyphs
    int win_w, win_h;
    SDL_Renderer *renderer;
    SDL_Window *window;

} Application;

void update_time();
extern Application application;

#endif // _APPLICATION_H