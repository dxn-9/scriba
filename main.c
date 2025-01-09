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
#include "application.h"
#include "action.h"
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

bool loop()
{
    Context *context = &((Context *)application.editors.data)[application.current_editor];
    TextBuffer *buffer = &context->buffer;
    Cursor *cursor = &context->cursor;
    Selection *selection = &context->selection;

    SDL_SetRenderDrawColor(renderer, BG_COLOR.r, BG_COLOR.g, BG_COLOR.b, BG_COLOR.a);
    SDL_RenderClear(renderer);

    SDL_Event e;
    float scroll_offset_y = 0.0;
    float scroll_offset_x = 0.0;
    context->focus_cursor = false; // Reset the focus cursor flag

    while (SDL_PollEvent(&e) != 0)
    {
        // printf("Event! %i - %i \n", e.type, e.key.down);
        switch (e.type)
        {
        case SDL_EVENT_TEXT_INPUT:

            if (selection->is_active)
                clear_selection_text(selection, buffer, cursor);

            if (application.mode == Command)
            {
                text_add(&application.command_buffer, &application.command_buffer_cursor, e.text.text);
                cursor_move_right(&application.command_buffer_cursor, &application.command_buffer);
            }
            else
            {
                text_add(buffer, cursor, e.text.text);
                cursor_move_right(cursor, buffer);
                context->focus_cursor = true;
            }
            break;
        case SDL_EVENT_QUIT:
            return false;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        {
            Vector2I pos = get_cursor_pos_from_screen(e.button.x, e.button.y, last_view_offset);
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

                    Vector2I pos = get_cursor_pos_from_screen(e.button.x, e.button.y, last_view_offset);
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

    SDL_GetWindowSizeInPixels(win, &application.win_w, &application.win_h);

    SDL_FRect offset = get_view_offset(last_view_offset, cursor, context->focus_cursor, buffer->lines.length, max_horizontal_characters, scroll_offset_x, scroll_offset_y);
    last_view_offset = offset;

    render_selection(renderer, &context->selection, buffer, offset);
    render_buffer(renderer, buffer, offset);
    render_cursor(renderer, cursor, offset);
    render_bottom_bar(renderer, context);

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
    application.time = SDL_GetTicks();

    return true;
}

void quit(Context *context)
{
    clean_text(&application.command_buffer);
    clean_text(&context->buffer);
    vector_free(&application.editors);
    SDL_StopTextInput(win);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();
    TTF_CloseFont(font);
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

    char *filename = argv[1];

    Context context;
    read_or_create_file(filename, &context);

    if (!init())
    {
        return 1;
    }

    application.editors = vector_new(sizeof(Context));
    vector_push(&application.editors, &context);
    application.mode = Insert;

    size_t frame_counter = 0;
    last_view_offset.x = get_line_number_offset_text();

    while (loop())
    {
        uint64_t now = SDL_GetTicks();
        if (frame_counter % FPS_SAMPLE_SIZE == 0)
        {
            uint64_t tot_delta = 0;
            application.fps = 1000 / (now - application.time);
        }
        frame_counter++;
    }

    quit(&context);
    return 0;
}