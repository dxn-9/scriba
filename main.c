#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "key_bindings.h"
#include "bottom_bar.h"
#include "text.h"
#include "clock.h"
#include "cursor.h"
#include "utils.h"
#include "constants.h"

/**
 * TODO:
 *  - Multiple files open
 */

SDL_Window *win = NULL;
SDL_Renderer *renderer;
SDL_FRect last_view_offset = {0};
bool is_mouse_down = false;

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

    SDL_SetRenderDrawColor(renderer, BG_COLOR.r, BG_COLOR.g, BG_COLOR.b, BG_COLOR.a);
    SDL_RenderClear(renderer);

    SDL_Event e;
    float scroll_offset_y = 0.0;
    float scroll_offset_x = 0.0;
    // bool should_focus_cursor = false; // Flag to "recenter" the view if an action was performed
    context->focus_cursor = false; // Reset the focus cursor flag

    while (SDL_PollEvent(&e) != 0)
    {
        // printf("Event! %i - %i \n", e.type, e.key.down);
        switch (e.type)
        {
        case SDL_EVENT_TEXT_INPUT:

            if (selection->is_active)
                clear_selection_text(selection, buffer, cursor);

            text_add(buffer, cursor, e.text.text);
            cursor_move_right(cursor, buffer);
            context->focus_cursor = true;
            break;
        case SDL_EVENT_QUIT:
            return false;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        {
            Vector2I pos = get_cursor_pos_from_screen(e.button.x, e.button.y, last_view_offset, cursor->w, cursor->h);
            // Make sure to clamp the values between legal values.
            pos.y = MIN(MAX(0, pos.y), buffer->lines.length - 1);
            pos.x = MIN(MAX(0, pos.x), get_line_length(buffer, pos.y));
            is_mouse_down = true;
            cursor_set_y(cursor, buffer, pos.y);
            cursor_set_x(cursor, buffer, pos.x);

            if (selection->is_active)
            {
                selection_cancel(selection);
            }
            context->focus_cursor = true;
        }
        break;

        case SDL_EVENT_MOUSE_MOTION:
            if (is_mouse_down)
            {
                if (selection->is_active)
                {

                    Vector2I pos = get_cursor_pos_from_screen(e.button.x, e.button.y, last_view_offset, cursor->w, cursor->h);
                    // Make sure to clamp the values between legal values.
                    pos.y = MIN(MAX(0, pos.y), buffer->lines.length - 1);
                    pos.x = MIN(MAX(0, pos.x), get_line_length(buffer, pos.y));
                    cursor_set_y(cursor, buffer, pos.y);
                    cursor_set_x(cursor, buffer, pos.x);
                    selection_update(selection, cursor, buffer);
                }
                else
                {
                    selection_start(selection, cursor, buffer);
                }
                context->focus_cursor = true;
            }
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            is_mouse_down = false;
            break;
        case SDL_EVENT_MOUSE_WHEEL:
        {
            scroll_offset_y = e.wheel.y * SCROLL_MULT;
            scroll_offset_x = e.wheel.x * SCROLL_MULT;
        }
        break;
        case SDL_EVENT_KEY_DOWN:
        {
            Action action = get_action(e);
            if (action == Quit)
                return false;
            Dispatch(context, e, action);
        }
        break;
        }
    }

    int win_w, win_h;
    SDL_GetWindowSizeInPixels(win, &win_w, &win_h);

    SDL_FRect offset = get_view_offset(last_view_offset, win_w, win_h, cursor, context->focus_cursor, buffer->lines.length, max_horizontal_characters, scroll_offset_x, scroll_offset_y);
    last_view_offset = offset;

    render_selection(renderer, &context->selection, buffer, offset);
    render_buffer(renderer, buffer, offset, cursor->w, cursor->h, win_h);
    render_cursor(renderer, cursor, offset);
    render_bottom_bar(renderer, context, win_w, win_h);

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

    SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS;

    win = SDL_CreateWindow("Scriba", DEFAULT_WIDTH, DEFAULT_HEIGHT, flags);
    if (win == NULL)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(win, NULL);
    if (renderer == NULL)
    {
        printf("Failed to create renderer %s\n", SDL_GetError());
        return false;
    }
    if (!SDL_SetRenderVSync(renderer, 1))
    {

        printf("Failed to set renderer vsync %s\n", SDL_GetError());
        return false;
    }
    if (!TTF_Init())

    {
        printf("Could not initialize TTF: %s\n", SDL_GetError());
        return false;
    }

    if (!init_text())
    {
        printf("Could not initialize text: %s\n", SDL_GetError());
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

int main(int argc, char *argv[])
{
    SDL_IOStream *stream = NULL;
    Sint64 file_size;
    if (argc != 2)
    {
        printf("Usage: scriba <filename>\n");
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

    free(data);

    uint64_t fps_samples[FPS_SAMPLE_SIZE];
    memset(fps_samples, 0, sizeof(fps_samples));
    size_t frame_counter = 0;
    last_view_offset.x = get_line_number_offset_text(char_w_);

    while (loop(&context))
    {
        uint64_t now = SDL_GetTicks();
        if (frame_counter % FPS_SAMPLE_SIZE == 0)
        {
            uint64_t tot_delta = 0;

            for (int i = 0; i < FPS_SAMPLE_SIZE; ++i)
            {
                tot_delta += fps_samples[i];
            }
            context.fps = 1000 / (now - app_clock.time);
        }

        app_clock.delta_time = now - app_clock.time;
        app_clock.time = now;

        fps_samples[(frame_counter % FPS_SAMPLE_SIZE)] = app_clock.delta_time;
        frame_counter++;
    }

    quit(&context);
    return 0;
}