#include "SDL.h"
#include "SDL_render.h"
#include "SDL_ttf.h"
#include "text.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

SDL_Window *win = NULL;
SDL_Renderer *renderer;
TTF_Font *font;
SDL_Surface *text_surface;
SDL_Texture *text_tex;

bool loop()
{

    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_Event e;

    while (SDL_PollEvent(&e) != 0)
    {

        switch (e.type)
        {
        case SDL_TEXTINPUT:
            text_add(e.text.text);
            break;
        case SDL_QUIT:
            return false;
        case SDL_KEYDOWN:
            if (e.key.keysym.sym == SDLK_ESCAPE)
            {
                return false;
            }
        }
    }
    if (text.length > 0)
    {

        text_surface = TTF_RenderText_Blended(font, text.text, (SDL_Color){0, 0, 0});

        if (text_surface == NULL)
        {
            printf("Failed to render text: %s", TTF_GetError());
            return false;
        }
        text_tex = SDL_CreateTextureFromSurface(renderer, text_surface);
        int texW, texH;
        SDL_QueryTexture(text_tex, NULL, NULL, &texW, &texH);
        SDL_Rect dest = {0, 0, texW, texH};
        SDL_RenderCopy(renderer, text_tex, NULL, &dest);
    }

    SDL_RenderPresent(renderer);

    return true;
}

bool init()
{
    text_init();

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

    font = TTF_OpenFont("ttf/JetbrainsMono-Regular.ttf", 24);

    if (font == NULL)
    {
        printf("Failed to load font: %s", TTF_GetError());
        return false;
    }

    // Start sending SDL_TextInput events
    SDL_StartTextInput();

    return true;
}

void quit()
{
    SDL_StopTextInput();
    TTF_CloseFont(font);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

int main(int argc, char **argv)
{

    if (!init())
    {

        return 1;
    }

    while (loop())
    {
        SDL_Delay(10);
    }

    quit();
    return 0;
}