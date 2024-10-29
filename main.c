#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bottom_bar.h"
#include "text.h"
#include "clock.h"
#include "cursor.h"
#include "utils.h"

#define HORIZONTAL_VIEW_OFFSET 2
#define VERTICAL_VIEW_OFFSET 2
#define FPS_SAMPLE_SIZE 60

SDL_Window *win = NULL;
SDL_Renderer *renderer;

void save(Context *ctx)
{
    SDL_IOStream *stream = SDL_IOFromFile(ctx->file_name, "w");
    size_t size = SDL_WriteIO(stream, ctx->buffer.text.data, ctx->buffer.text.length - 1);
    if (size < ctx->buffer.text.length - 1)
    {
        printf("SaveFailed::%s", SDL_GetError());
        SDL_CloseIO(stream);
        return;
    }
    bool result = SDL_CloseIO(stream);
    if (!result)
    {
        printf("ClosingFailed::%s", SDL_GetError());
    }
}
bool loop(Context *context)
{
    TextBuffer *buffer = &context->buffer;
    Cursor *cursor = &context->cursor;
    Selection *selection = &context->selection;

    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_Event e;

    while (SDL_PollEvent(&e) != 0)
    {
        // printf("Event! %i - %i \n", e.type, e.key.down);
        switch (e.type)
        {
        case SDL_EVENT_TEXT_INPUT:
            text_add(context, e.text.text);
            cursor_move_right(context);

            break;
        case SDL_EVENT_QUIT:
            return false;
        case SDL_EVENT_KEY_DOWN:
            switch (e.key.key)
            {
            case SDLK_LEFT:
                if (e.key.mod == SDL_KMOD_LSHIFT && !selection->is_active)
                    selection_start(context);
                cursor_move_left(context);
                if (!(e.key.mod == SDL_KMOD_LSHIFT) && selection->is_active)
                {
                    selection_cancel(context);
                }
                break;
            case SDLK_UP:
                if (e.key.mod == SDL_KMOD_LSHIFT && !selection->is_active)
                    selection_start(context);
                cursor_move_up(context);
                if (!(e.key.mod == SDL_KMOD_LSHIFT) && selection->is_active)
                {
                    selection_cancel(context);
                }
                break;
            case SDLK_RIGHT:
                if (e.key.mod == SDL_KMOD_LSHIFT && !selection->is_active)
                    selection_start(context);
                cursor_move_right(context);
                if (!(e.key.mod == SDL_KMOD_LSHIFT) && selection->is_active)
                {
                    selection_cancel(context);
                }
                break;
            case SDLK_DOWN:
                if (e.key.mod == SDL_KMOD_LSHIFT && !selection->is_active)
                    selection_start(context);
                cursor_move_down(context);
                if (!(e.key.mod == SDL_KMOD_LSHIFT) && selection->is_active)
                {
                    selection_cancel(context);
                }
                break;
            case SDLK_S:
                if (e.key.mod == SDL_KMOD_LGUI)
                {
                    printf("Saving!!\n");
                    save(context);
                }
                break;
            case SDLK_V:
                if (e.key.mod & SDL_KMOD_GUI || e.key.mod & SDL_KMOD_CTRL)
                {
                    handle_paste(context);
                }
                break;
            case SDLK_C:
                if (e.key.mod & SDL_KMOD_GUI || e.key.mod & SDL_KMOD_CTRL)
                {
                    handle_copy(context);
                }
                break;
            case SDLK_X:
                if (e.key.mod & SDL_KMOD_GUI || e.key.mod & SDL_KMOD_CTRL)
                {
                    handle_cut(context);
                }
                break;
            case SDLK_R:
                printf("TextBuffer: \n");
                debug_vec(&buffer->text);
                printf("LinesBuffer: \n");
                debug_vec(&buffer->lines);
                break;
            case SDLK_RETURN:
                text_newline(context);
                break;
            case SDLK_BACKSPACE:
                if (!(e.key.mod == SDL_KMOD_LSHIFT) && selection->is_active)
                {
                    selection_delete(context);
                }
                else
                {
                    text_remove_char(context);
                }
                break;
            case SDLK_ESCAPE:
                return false;
            }
            break;
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

    render_selection(renderer, &context->selection, buffer, offset);
    render_buffer(renderer, buffer, offset);
    render_cursor(renderer, cursor, offset);
    render_bottom_bar(renderer, win_w, win_h);

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
    app_clock.time = SDL_GetTicks();

    return true;
}

void quit(Context *context)
{
    clean_text(&context->buffer);
    SDL_StopTextInput(win);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

int main(int argc, char **argv)
{
    SDL_IOStream *stream = NULL;
    Sint64 file_size;
    if (argc != 2)
    {
        printf("Usage: scriba <pathname>\n");
        return 1;
    }

    char *file_name = argv[1];
    struct stat sb;
    if (stat(file_name, &sb) == -1)
    {
        if (errno == ENOENT)
        {
            stream = SDL_IOFromFile(file_name, "w+");
        }
        else
        {
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        stream = SDL_IOFromFile(file_name, "r+");
    }

    if (stream == NULL)
    {
        printf("SDL_IOFromFile::%s\n", SDL_GetError());
        return 1;
    }

    file_size = SDL_GetIOSize(stream);
    char *data = malloc(file_size);
    SDL_ReadIO(stream, data, file_size);
    SDL_CloseIO(stream);

    Context context;

    if (!init())
    {

        return 1;
    }
    context.file_name = file_name;
    context.cursor = new_cursor(0, 0, char_w_, char_h_);
    context.buffer = text_new(&context.cursor, data);
    context.selection = (Selection){.is_active = false};

    bottom_bar.cursor = &context.cursor;
    bottom_bar.selection = &context.selection;

    free(data);

    int fps_samples[FPS_SAMPLE_SIZE];
    memset(fps_samples, 0, sizeof(fps_samples));
    size_t frame_counter = 0;
    while (loop(&context))
    {
        int now = SDL_GetTicks();
        if (frame_counter % FPS_SAMPLE_SIZE == 0)
        {
            size_t tot_delta = 0;

            for (int i = 0; i < FPS_SAMPLE_SIZE; ++i)
            {
                tot_delta += fps_samples[i];
            }
            bottom_bar.fps = 1000.0 / ((float)tot_delta / (float)FPS_SAMPLE_SIZE);
        }

        app_clock.delta_time = now - app_clock.time;
        app_clock.time = now;

        fps_samples[frame_counter % FPS_SAMPLE_SIZE] = app_clock.delta_time;
        frame_counter++;
    }

    quit(&context);
    return 0;
}