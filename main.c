#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "text.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "clock.h"
#include "cursor.h"

SDL_Window *win = NULL;
SDL_Renderer *renderer;

typedef struct
{
    Cursor cursor;
    TextBuffer main_buffer;
} Context;

bool loop(Context *context)
{
    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_Event e;

    while (SDL_PollEvent(&e) != 0)
    {
        switch (e.type)
        {
        case SDL_EVENT_TEXT_INPUT:
            text_append(&context->main_buffer, e.text.text);
            cursor_move_right(&context->cursor, &context->main_buffer);
            break;
        case SDL_EVENT_QUIT:
            return false;
        case SDL_EVENT_KEY_DOWN:
            switch (e.key.key)
            {
            case SDLK_LEFT:
                cursor_move_left(&context->cursor, &context->main_buffer);
                break;
            case SDLK_UP:
                cursor_move_up(&context->cursor, &context->main_buffer);
                break;
            case SDLK_RIGHT:
                cursor_move_right(&context->cursor, &context->main_buffer);
                break;
            case SDLK_DOWN:
                cursor_move_down(&context->cursor, &context->main_buffer);
                break;
            case SDLK_RETURN:
                text_newline(&context->main_buffer, &context->cursor);
                cursor_move_down(&context->cursor, &context->main_buffer);
                break;
            case SDLK_ESCAPE:
                printf("escape! \n");
                return false;
            }
        }
    }

    render_text(renderer, &context->main_buffer);
    render_cursor(renderer, &context->cursor);

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
    win = SDL_CreateWindow("Hello World", 500, 300, 0);
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
    context.main_buffer = text_new("Hello");

    while (loop(&context))
    {
        int now = SDL_GetTicks();
        appClock.delta_time = now - appClock.time;
        appClock.time = now;
    }

    quit(&context);
    return 0;
}
