#include "overlay.h"
#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "application.h"
#include "constants.h"

void render_list_editors()
{
    if (application.mode == List)
    {
        int height_padding = 0;
        int height = application.editors.length * application.char_h + height_padding * 2;
        int width = 400;
        int x = (application.win_w - width) / 2, y = (application.win_h - height) / 2;
        SDL_FRect position = {
            .w = width,
            .h = height,
            .x = x,
            .y = y};

        render_fill_rectangle(BG_COLOR, position);
        render_rectangle(BG_ACCENT_COLOR, position);
        for (int i = 0, line_offset = height_padding; i < application.editors.length; i++, line_offset += application.char_h)
        {

            Editor *editor = &((Editor *)application.editors.data)[i];
            int size = strlen(editor->filename) + 5;
            char editor_name[size];
            sprintf(editor_name, "[%i] %s", i + 1, editor->filename);

            if (i == application.current_editor)
            {
                SDL_FRect line_position = {
                    .w = width,
                    .h = application.char_h,
                    .x = x,
                    .y = y + line_offset};
                render_fill_rectangle(BG_ACCENT_COLOR, line_position);
            }

            render_text(editor_name, TEXT_COLOR, x, y + line_offset);
        }
    }
}

void render_commad_buffer(int bar_y)
{
    // Length 1 means that theres only the \0 character.
    if (application.command_buffer.text.length > 1)
    {
        render_text(application.command_buffer.text.data, (SDL_Color){0, 255, 0, 255}, TEMP_BOTTOM_BAR_COMMAND_X, bar_y);
    }
}
void render_selection_bar(Editor *editor, int bar_y)
{
    SDL_assert(&editor->selection != NULL);
    if (!editor->selection.is_active)
        return;

    char selection_str[20];
    sprintf(selection_str, "c:%i", abs((int)editor->selection.end_x - (int)editor->selection.start_x));
    render_text(selection_str, (SDL_Color){0, 255, 0, 255}, 200, bar_y);
}
void render_cursor_position(Editor *editor, int bar_y)
{
    SDL_assert(&editor->cursor != NULL);

    char cursor_str[20];
    sprintf(cursor_str, "x:%i y:%i", editor->cursor.x, editor->cursor.y);
    render_text(cursor_str, (SDL_Color){0, 255, 0, 255}, 100, bar_y);
}

void render_fps_counter(Editor *editor, int bar_y)
{
    char fps_str[40];
    sprintf(fps_str, "FPS:%0.f", application.fps);
    render_text(fps_str, (SDL_Color){0, 255, 0, 255}, BOTTOM_BAR_PADDING_X, bar_y);
}

void render_filename(Editor *editor, int bar_y)
{
    int size = strlen(editor->filename) + 5;
    char filename_view[size];
    sprintf(filename_view, "[%i] %s", application.current_editor + 1, editor->filename);
    int x = application.win_w - (strlen(filename_view) + 1) * application.char_w;
    render_text(filename_view, (SDL_Color){0, 255, 0, 255}, x, bar_y);
}

void render_bottom_bar(Editor *editor)
{
    SDL_Color color = {100, 100, 100, 255};
    int bar_y = application.win_h - BOTTOM_BAR_H;
    SDL_FRect rect = {0, bar_y, application.win_w, BOTTOM_BAR_H};
    bar_y += (BOTTOM_BAR_H - application.char_h) / 2;
    render_fill_rectangle(color, rect);
    render_fps_counter(editor, bar_y);
    render_cursor_position(editor, bar_y);
    render_selection_bar(editor, bar_y);
    render_filename(editor, bar_y);
    render_commad_buffer(bar_y);
}