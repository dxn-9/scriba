#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "text.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

SDL_Window *win = NULL;
SDL_Renderer *renderer;
TTF_Font *font;

TextBuffer main_buffer;

char *initial_text = "Hello";

void render_text(char *str, int offsetY)
{
    // printf("RenderText::%s, len:%i\n", str, strlen(str));
    SDL_Surface *surface = TTF_RenderText_Blended(font, str, (SDL_Color){0, 0, 0});

    if (surface == NULL)
    {
        printf("Error creating surface in render_text: %s\n", SDL_GetError());
        return;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (texture == NULL)
    {
        printf("Error creating texture in render_text: %s\n", SDL_GetError());
        return;
    }

    int texW, texH;
    if (SDL_QueryTexture(texture, NULL, NULL, &texW, &texH) < 0)
    {
        printf("QueryTexture Failed %s", SDL_GetError());
        return;
    }

    if (SDL_RenderCopy(renderer, texture, NULL, &(SDL_Rect){0, offsetY, texW, texH}) < 0)
    {
        printf("RenderCopy failed: %s", SDL_GetError());
        return;
    }

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

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
            text_append(&main_buffer, e.text.text);
            break;
        case SDL_QUIT:
            return false;
        case SDL_KEYDOWN:
            if (e.key.keysym.sym == SDLK_RETURN)
            {
                text_append(&main_buffer, "\n");
                break;
            }
            if (e.key.keysym.sym == SDLK_ESCAPE)
            {
                return false;
            }
        }
    }
    if (main_buffer.length > 0)
    {

        char display_buffer[main_buffer.length + 1];
        strcpy(display_buffer, main_buffer.text);
        char *token = strtok(display_buffer, "\n");
        int token_count = 0;
        while (token != NULL)
        {
            render_text(token, token_count * 20);
            token = strtok(NULL, "\n");
            token_count++;
        }
    }

    SDL_RenderPresent(renderer);

    return true;
}

bool init()
{
    main_buffer = text_new(initial_text);

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

    font = TTF_OpenFont("ttf/JetBrainsMono-Regular.ttf", 24);

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
    TTF_CloseFont(font);
    SDL_StopTextInput();

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
