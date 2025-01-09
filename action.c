#include "action.h"
#include "utils.h"
#include "text.h"

void action_null(Context *context, SDL_Event event) { return; }
void action_save_file(Context *context, SDL_Event event)
{

    SDL_IOStream *stream = SDL_IOFromFile(context->filename, "w");
    size_t size = SDL_WriteIO(stream, context->buffer.text.data, context->buffer.text.length - 1);
    if (size < context->buffer.text.length - 1)
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
void action_select_all(Context *context, SDL_Event event)
{
    TextBuffer *buffer = &context->buffer;

    Cursor start_cursor = new_cursor(0, 0);
    int last_line_index = buffer->lines.length - 1;

    SDL_assert(last_line_index >= 0);

    Line *last_line = get_line_at(buffer, last_line_index);

    Cursor end_cursor = new_cursor((last_line->end - last_line->start) - 1, last_line_index);
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
    printf("CommandBuffer: \n");
    debug_vec(&application.command_buffer.text);
}
void action_insert_command(Context *context, SDL_Event event)
{
    clean_text(&application.command_buffer);
    application.command_buffer_cursor = new_cursor(0, 0);
    application.command_buffer = text_new("");
    application.mode = Command;
}

void action_insert_tab(Context *context, SDL_Event event)
{
    if (application.mode == Command)
        return;

    if (context->selection.is_active)
        clear_selection_text(&context->selection, &context->buffer, &context->cursor);
    text_add(&context->buffer, &context->cursor, "\t");
    cursor_move_right(&context->cursor, &context->buffer);
    selection_update(&context->selection, &context->cursor, &context->buffer);
    context->focus_cursor = true;
}

void action_insert_line(Context *context, SDL_Event event)
{
    if (application.mode == Command)
    {
        char command[100] = {0};
        strcpy(command, application.command_buffer.text.data);

        if (command[0] == 'e' && command[1] == ' ')
        {
            Context new_editor;
            read_or_create_file(command + 2, &new_editor);
            vector_push(&application.editors, &new_editor);
            application.current_editor += 1;
            application.mode = Insert;
        }
        else
        {

            clean_text(&application.command_buffer);
            char invalid_cmd[100];
            sprintf(invalid_cmd, "Invalid command: %s", command);
            application.command_buffer = text_new(invalid_cmd);
        }
    }
    else if (application.mode == Insert)
    {

        if (context->selection.is_active)
            clear_selection_text(&context->selection, &context->buffer, &context->cursor);
        text_add(&context->buffer, &context->cursor, "\n");
        cursor_move_down(&context->cursor, &context->buffer);
        context->focus_cursor = true;
    }
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
    &action_delete_char,
    &action_insert_command,
    &action_save_file};

inline void Dispatch(Context *context, SDL_Event event, Action action)
{
    actionFunctions[action](context, event);
}

// ----- Keybindings for the actions ------

bool is_paste(SDL_Event event)
{

#ifdef __APPLE__
    return event.key.key == SDLK_V && event.key.mod & SDL_KMOD_GUI;
#else
    return event.key.key == SDLK_V && e.key.mod & SDL_KMOD_CTRL;
#endif
}

bool is_insert_command(SDL_Event event)
{
    return event.key.key == SDLK_F3;
}
bool is_cut(SDL_Event event)
{
#ifdef __APPLE__
    return event.key.key == SDLK_X && event.key.mod & SDL_KMOD_GUI;
#else
    return event.key.key == SDLK_X && e.key.mod & SDL_KMOD_CTRL;
#endif
}
bool is_copy(SDL_Event event)
{
#ifdef __APPLE__
    return event.key.key == SDLK_C && event.key.mod & SDL_KMOD_GUI;
#else
    return event.key.key == SDLK_C && e.key.mod & SDL_KMOD_CTRL;
#endif
}
bool is_save(SDL_Event event)
{
#ifdef __APPLE__
    return event.key.key == SDLK_S && event.key.mod & SDL_KMOD_GUI;
#else
    return event.key.key == SDLK_S && e.key.mod & SDL_KMOD_CTRL;
#endif
}

bool is_select_all(SDL_Event event)
{
#ifdef __APPLE__
    return event.key.key == SDLK_A && event.key.mod & SDL_KMOD_GUI;
#else
    return event.key.key == SDLK_A && e.key.mod & SDL_KMOD_CTRL;
#endif
}

bool is_move_left(SDL_Event event)
{
    return event.key.key == SDLK_LEFT;
}
bool is_move_right(SDL_Event event)
{
    return event.key.key == SDLK_RIGHT;
}
bool is_move_down(SDL_Event event)
{
    return event.key.key == SDLK_DOWN;
}
bool is_move_up(SDL_Event event)
{
    return event.key.key == SDLK_UP;
}

bool is_debug_print(SDL_Event event)
{
    return event.key.key == SDLK_R && event.key.mod & SDL_KMOD_CTRL;
}
bool is_quit(SDL_Event event)
{
    return event.key.key == SDLK_ESCAPE;
}
bool is_insert_tab(SDL_Event event)
{
    return event.key.key == SDLK_TAB;
}
bool is_insert_line(SDL_Event event)
{
    return event.key.key == SDLK_RETURN;
}

bool is_delete_char(SDL_Event event)
{
    return event.key.key == SDLK_BACKSPACE;
}
Action get_action(SDL_Event event)
{
    if (is_copy(event))
        return Copy;
    if (is_save(event))
        return SaveFile;
    if (is_paste(event))
        return Paste;
    if (is_cut(event))
        return Cut;
    if (is_select_all(event))
        return SelectAll;
    if (is_move_left(event))
        return MoveLeft;
    if (is_move_right(event))
        return MoveRight;
    if (is_move_down(event))
        return MoveDown;
    if (is_move_up(event))
        return MoveUp;
    if (is_debug_print(event))
        return DebugPrint;
    if (is_insert_tab(event))
        return InsertTab;
    if (is_insert_line(event))
        return InsertLine;
    if (is_delete_char(event))
        return DeleteChar;
    if (is_insert_command(event))
    {

        printf("Inset Command!\n");
        return InsertCommand;
    }
    if (is_quit(event))
        return Quit;
    return None;
}
