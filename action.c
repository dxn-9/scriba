#include "action.h"
#include "utils.h"
#include "text.h"

void action_null(Context *context, SDL_Event event) { return; }
void action_select_all(Context *context, SDL_Event event)
{
    Cursor *cursor = &context->cursor;
    TextBuffer *buffer = &context->buffer;

    Cursor start_cursor = new_cursor(0, 0, cursor->w, cursor->h);
    int last_line_index = buffer->lines.length - 1;

    SDL_assert(last_line_index >= 0);

    Line *last_line = get_line_at(buffer, last_line_index);

    Cursor end_cursor = new_cursor((last_line->end - last_line->start) - 1, last_line_index, cursor->w, cursor->h);
    cursor_update_view_x(&end_cursor, buffer);
    selection_start(&context->selection, &start_cursor, buffer);
    selection_update(&context->selection, &end_cursor, buffer);
}
void action_copy(Context *context, SDL_Event event)
{

    order_selection(&context->selection);
    handle_copy(&context->selection, &context->buffer);
    selection_cancel(&context->selection);
    context->focus_cursor = true;
}
void action_paste(Context *context, SDL_Event event)
{
    int buffer_index = handle_paste(&context->selection, &context->buffer, &context->cursor);
    cursor_set_from_buffer_index(&context->cursor, &context->buffer, buffer_index);
    context->focus_cursor = true;
}
void action_cut(Context *context, SDL_Event event)
{

    order_selection(&context->selection);
    handle_cut(&context->selection, &context->buffer);
    context->focus_cursor = true;
}
void action_move_left(Context *context, SDL_Event event)
{

    if (event.key.mod == SDL_KMOD_LSHIFT && !context->selection.is_active)
        selection_start(&context->selection, &context->cursor, &context->buffer);
    cursor_move_left(&context->cursor, &context->buffer);
    selection_update(&context->selection, &context->cursor, &context->buffer);

    if (!(event.key.mod == SDL_KMOD_LSHIFT) && context->selection.is_active)
    {
        selection_cancel(&context->selection);
    }

    context->focus_cursor = true;
}

void action_move_up(Context *context, SDL_Event event)
{
    if (event.key.mod == SDL_KMOD_LSHIFT && !context->selection.is_active)
        selection_start(&context->selection, &context->cursor, &context->buffer);
    cursor_move_up(&context->cursor, &context->buffer);
    selection_update(&context->selection, &context->cursor, &context->buffer);
    if (!(event.key.mod == SDL_KMOD_LSHIFT) && context->selection.is_active)
    {
        selection_cancel(&context->selection);
    }
    context->focus_cursor = true;
}
void action_move_right(Context *context, SDL_Event event)
{
    if (event.key.mod == SDL_KMOD_LSHIFT && !context->selection.is_active)
        selection_start(&context->selection, &context->cursor, &context->buffer);
    cursor_move_right(&context->cursor, &context->buffer);
    selection_update(&context->selection, &context->cursor, &context->buffer);
    if (!(event.key.mod == SDL_KMOD_LSHIFT) && context->selection.is_active)
    {
        selection_cancel(&context->selection);
    }
    context->focus_cursor = true;
}
void action_move_down(Context *context, SDL_Event event)
{
    if (event.key.mod == SDL_KMOD_LSHIFT && !context->selection.is_active)
        selection_start(&context->selection, &context->cursor, &context->buffer);
    cursor_move_down(&context->cursor, &context->buffer);
    selection_update(&context->selection, &context->cursor, &context->buffer);
    if (!(event.key.mod == SDL_KMOD_LSHIFT) && context->selection.is_active)
    {
        selection_cancel(&context->selection);
    }
    context->focus_cursor = true;
}
void action_debug_print(Context *context, SDL_Event event)
{

    printf("TextBuffer: \n");
    debug_vec(&context->buffer.text);
    printf("LinesBuffer: \n");
    debug_vec(&context->buffer.lines);
}

void action_insert_tab(Context *context, SDL_Event event)
{

    if (context->selection.is_active)
        clear_selection_text(&context->selection, &context->buffer, &context->cursor);
    text_add(&context->buffer, &context->cursor, "\t");
    cursor_move_right(&context->cursor, &context->buffer);
    selection_update(&context->selection, &context->cursor, &context->buffer);
    context->focus_cursor = true;
}

void action_insert_line(Context *context, SDL_Event event)
{
    if (context->selection.is_active)
        clear_selection_text(&context->selection, &context->buffer, &context->cursor);
    text_add(&context->buffer, &context->cursor, "\n");
    cursor_move_down(&context->cursor, &context->buffer);
    context->focus_cursor = true;
}
void action_delete_char(Context *context, SDL_Event event)
{
    if (context->selection.is_active)
    {
        clear_selection_text(&context->selection, &context->buffer, &context->cursor);
    }
    else
    {
        text_remove_char(&context->buffer, &context->cursor);
    }
    context->focus_cursor = true;
}

typedef void (*ActionFunction)(Context *, SDL_Event);
ActionFunction actionFunctions[] = {
    &action_null,
    &action_copy,
    &action_paste,
    &action_cut,
    &action_select_all,
    &action_move_left,
    &action_move_right,
    &action_move_down,
    &action_move_up,
    &action_debug_print,
    &action_insert_tab,
    &action_insert_line,
    &action_delete_char};

inline void Dispatch(Context *context, SDL_Event event, Action action)
{
    actionFunctions[action](context, event);
}
