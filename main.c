#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "text.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "clock.h"
#include "cursor.h"

#define HORIZONTAL_VIEW_OFFSET 2
#define VERTICAL_VIEW_OFFSET 2

SDL_Window *win = NULL;
SDL_Renderer *renderer;

typedef struct
{
    Cursor cursor;
    TextBuffer main_buffer;
    Selection selection;
} Context;

bool loop(Context *context)
{
    TextBuffer *main_buffer = &context->main_buffer;
    Cursor *cursor = &context->cursor;
    Selection *selection = &context->selection;

    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_Event e;

    while (SDL_PollEvent(&e) != 0)
    {
        printf("Event! %i - %i \n", e.type, e.key.down);
        switch (e.type)
        {
        case SDL_EVENT_TEXT_INPUT:
            text_add(main_buffer, cursor, e.text.text);
            cursor_move_right(cursor, main_buffer, selection);
            break;
        case SDL_EVENT_QUIT:
            return false;
        case SDL_EVENT_KEY_DOWN:
            switch (e.key.key)
            {
            case SDLK_LEFT:
                cursor_move_left(cursor, main_buffer, selection);
                break;
            case SDLK_UP:
                cursor_move_up(cursor, main_buffer, selection);
                break;
            case SDLK_RIGHT:
                cursor_move_right(cursor, main_buffer, selection);
                break;
            case SDLK_R:
                printf("TextBuffer: \n");
                debug_vec(&main_buffer->text);
                printf("LinesBuffer: \n");
                debug_vec(&main_buffer->lines);
                break;
            case SDLK_DOWN:
                cursor_move_down(cursor, main_buffer, selection);
                break;
            case SDLK_RETURN:
                text_newline(main_buffer, cursor);
                break;
            case SDLK_LSHIFT:
                context->selection = selection_new(main_buffer, cursor);
                break;
            case SDLK_BACKSPACE:
                text_remove_char(main_buffer, cursor);
                break;
            case SDLK_ESCAPE:
                return false;
            }
            break;
        case SDL_EVENT_KEY_UP:
            switch (e.key.key)
            {
            case SDLK_LSHIFT:
                context->selection = selection_end(main_buffer, cursor);
                break;
            }
        }
    }
    int win_w, win_h;
    SDL_GetWindowSizeInPixels(win, &win_w, &win_h);

    int cursor_nums_x = win_w / context->cursor.w;
    int cursor_nums_y = win_h / context->cursor.h;
    // This calculates how much the view should shift to the left and down. If the cursor is within the
    // bounds of the normal view it will be 0, or else it will be a multiple of cursor->w (how many chars are out of the view). Same applies for y coords.
    SDL_FRect offset = {
        .x = MIN(0, (cursor_nums_x - (context->cursor.x + HORIZONTAL_VIEW_OFFSET)) * context->cursor.w),
        .y = MIN(0, (cursor_nums_y - (context->cursor.y + VERTICAL_VIEW_OFFSET)) * context->cursor.h),
    };

    render_selection(renderer, &context->selection, main_buffer, (SDL_FRect){});
    render_text(renderer, main_buffer, offset);
    render_cursor(renderer, cursor, offset);

    SDL_RenderPresent(renderer);

    return true;
}

bool init()
{

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    win = SDL_CreateWindow("Scriba", 500, 300, 0);
    if (win == NULL)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(win, NULL);
    if (renderer == NULL)
    {
        printf("Failed to create renderer %s", SDL_GetError());
        return false;
    }
    if (!TTF_Init())
    {
        printf("Could not initialize TTF: %s", SDL_GetError());
        return false;
    }

    if (!init_text())
    {
        printf("Could not initialize text: %s", SDL_GetError());
        return false;
    }

    // Start sending SDL_TextInput events
    SDL_StartTextInput(win);
    appClock.time = SDL_GetTicks();

    return true;
}

void quit(Context *context)
{
    clean_text(&context->main_buffer);
    SDL_StopTextInput(win);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

int main(int argc, char **argv)
{
    Context context;

    if (!init())
    {

        return 1;
    }
    Cursor cursor = new_cursor(0, 0, char_w_, char_h_);
    context.cursor = cursor;
    context.main_buffer = text_new(&cursor, initial_text);
    while (loop(&context))
    {
        int now = SDL_GetTicks();
        appClock.delta_time = now - appClock.time;
        appClock.time = now;
    }

    quit(&context);
    return 0;
}
