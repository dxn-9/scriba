#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
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
        case SDL_TEXTINPUT:
            text_append(&main_buffer, e.text.text);
            cursor_move_right(&context->cursor, &main_buffer);
            break;
        case SDL_QUIT:
            return false;
        case SDL_KEYDOWN:

            if (e.key.keysym.sym == SDLK_RETURN)
            {
                text_append(&main_buffer, "\n");
                cursor_move_down(&context->cursor, &main_buffer);
                break;
            }
            if (e.key.keysym.sym == SDLK_ESCAPE)
            {
                return false;
            }
        }
    }

    render_text(renderer);
    render_cursor(renderer, &context->cursor);

    SDL_RenderPresent(renderer);

    return true;
}

bool init()
{

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    win = SDL_CreateWindow("Hello World", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 300, 0);
    if (win == NULL)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        printf("Failed to create renderer %s", SDL_GetError());
        return false;
    }
    if (TTF_Init() == -1)
    {
        printf("Could not initialize TTF: %s", TTF_GetError());
        return false;
    }

    if (!init_text())
    {
        printf("Could not initialize text: %s", TTF_GetError());
        return false;
    }

    // Start sending SDL_TextInput events
    SDL_StartTextInput();
    clock.time = SDL_GetTicks();

    return true;
}

void quit()
{
    clean_text();
    SDL_StopTextInput();
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

    while (loop(&context))
    {
        int now = SDL_GetTicks();
        clock.delta_time = now - clock.time;
        clock.time = now;
    }

    quit();
    return 0;
}
