#include "action.h"
#include "utils.h"
#include "text.h"

void action_null(Editor *editor, SDL_Event event) { return; }
void action_save_file(Editor *editor, SDL_Event event)
{

    SDL_IOStream *stream = SDL_IOFromFile(editor->filename, "w");
    size_t size = SDL_WriteIO(stream, editor->buffer.text.data, editor->buffer.text.length - 1);
    if (size < editor->buffer.text.length - 1)
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
void action_delete_file(Editor *editor, SDL_Event event)
{
    if (!SDL_RemovePath(editor->filename))
    {
        printf("RemovePath Failed::%s\n", SDL_GetError());
    }
    vector_remove(&application.editors, application.current_editor);
}
void action_select_all(Editor *editor, SDL_Event event)
{
    TextBuffer *buffer = &editor->buffer;

    Cursor start_cursor = new_cursor(0, 0);
    int last_line_index = buffer->lines.length - 1;

    SDL_assert(last_line_index >= 0);

    Line *last_line = get_line_at(buffer, last_line_index);

    Cursor end_cursor = new_cursor((last_line->end - last_line->start) - 1, last_line_index);
    cursor_update_view_x(&end_cursor, buffer);
    selection_start(&editor->selection, &start_cursor, buffer);
    selection_update(&editor->selection, &end_cursor, buffer);
}
void action_copy(Editor *editor, SDL_Event event)
{

    order_selection(&editor->selection);
    handle_copy(&editor->selection, &editor->buffer);
    selection_cancel(&editor->selection);
    editor->focus_cursor = true;
}
void action_paste(Editor *editor, SDL_Event event)
{
    int buffer_index = handle_paste(&editor->selection, &editor->buffer, &editor->cursor);
    cursor_set_from_buffer_index(&editor->cursor, &editor->buffer, buffer_index);
    editor->focus_cursor = true;
}
void action_cut(Editor *editor, SDL_Event event)
{

    order_selection(&editor->selection);
    handle_cut(&editor->selection, &editor->buffer);
    editor->focus_cursor = true;
}
void action_move_left(Editor *editor, SDL_Event event)
{
    if (application.mode == Command)
    {
        cursor_move_left(&application.command_buffer_cursor, &application.command_buffer);
        return;
    }
    if (application.mode == List)
        return;

    if (event.key.mod == SDL_KMOD_LSHIFT && !editor->selection.is_active)
        selection_start(&editor->selection, &editor->cursor, &editor->buffer);
    cursor_move_left(&editor->cursor, &editor->buffer);
    selection_update(&editor->selection, &editor->cursor, &editor->buffer);

    if (!(event.key.mod == SDL_KMOD_LSHIFT) && editor->selection.is_active)
    {
        selection_cancel(&editor->selection);
    }

    editor->focus_cursor = true;
}

void action_move_up(Editor *editor, SDL_Event event)
{
    if (application.mode == Command)
        return;
    if (application.mode == List)
    {
        application.current_editor = MAX(application.current_editor - 1, 0);
        return;
    }
    if (event.key.mod == SDL_KMOD_LSHIFT && !editor->selection.is_active)
        selection_start(&editor->selection, &editor->cursor, &editor->buffer);
    cursor_move_up(&editor->cursor, &editor->buffer);
    selection_update(&editor->selection, &editor->cursor, &editor->buffer);
    if (!(event.key.mod == SDL_KMOD_LSHIFT) && editor->selection.is_active)
    {
        selection_cancel(&editor->selection);
    }
    editor->focus_cursor = true;
}
void action_move_right(Editor *editor, SDL_Event event)
{
    if (application.mode == Command)
    {
        cursor_move_right(&application.command_buffer_cursor, &application.command_buffer);
        return;
    }
    if (application.mode == List)
        return;

    if (event.key.mod == SDL_KMOD_LSHIFT && !editor->selection.is_active)
        selection_start(&editor->selection, &editor->cursor, &editor->buffer);
    cursor_move_right(&editor->cursor, &editor->buffer);
    selection_update(&editor->selection, &editor->cursor, &editor->buffer);
    if (!(event.key.mod == SDL_KMOD_LSHIFT) && editor->selection.is_active)
    {
        selection_cancel(&editor->selection);
    }
    editor->focus_cursor = true;
}
void action_move_down(Editor *editor, SDL_Event event)
{
    if (application.mode == Command)
        return;
    if (application.mode == List)
    {
        application.current_editor = MIN(application.current_editor + 1, application.editors.length - 1);
        return;
    }
    if (event.key.mod == SDL_KMOD_LSHIFT && !editor->selection.is_active)
        selection_start(&editor->selection, &editor->cursor, &editor->buffer);
    cursor_move_down(&editor->cursor, &editor->buffer);
    selection_update(&editor->selection, &editor->cursor, &editor->buffer);
    if (!(event.key.mod == SDL_KMOD_LSHIFT) && editor->selection.is_active)
    {
        selection_cancel(&editor->selection);
    }
    editor->focus_cursor = true;
}
void action_debug_print(Editor *editor, SDL_Event event)
{

    printf("TextBuffer: \n");
    debug_vec(&editor->buffer.text);
    printf("LinesBuffer: \n");
    debug_vec(&editor->buffer.lines);
    printf("CommandBuffer: \n");
    debug_vec(&application.command_buffer.text);
}
void action_command_mode(Editor *editor, SDL_Event event)
{
    clean_text(&application.command_buffer);
    application.command_buffer_cursor = new_cursor(0, 0);
    application.command_buffer = text_new("");
    application.mode = Command;
}
void action_list_mode(Editor *editor, SDL_Event event)
{
    application.mode = List;
    SDL_StopTextInput(application.window);
}
void action_insert_mode(Editor *editor, SDL_Event event)
{
    action_command_mode(editor, event); // Clean up the command buffer
    SDL_StartTextInput(application.window);
    application.mode = Insert;
}

void action_insert_tab(Editor *editor, SDL_Event event)
{
    if (application.mode == Command)
        return;

    if (editor->selection.is_active)
        clear_selection_text(&editor->selection, &editor->buffer, &editor->cursor);
    text_add(&editor->buffer, &editor->cursor, "\t");
    cursor_move_right(&editor->cursor, &editor->buffer);
    selection_update(&editor->selection, &editor->cursor, &editor->buffer);
    editor->focus_cursor = true;
}

void action_insert_line(Editor *editor, SDL_Event event)
{
    switch (application.mode)
    {
    case Command:
    {

        char command[100] = {0};
        strcpy(command, application.command_buffer.text.data);

        // Edit command - open or create a new editor
        if (command[0] == 'e' && command[1] == ' ')
        {
            Editor new_editor;
            read_or_create_file(command + 2, &new_editor);
            vector_push(&application.editors, &new_editor);
            application.current_editor += 1;
            action_insert_mode(editor, event);
            return;
        }
        // Switch command - switch to an open editor by index
        if (command[0] == 's' && command[1] == ' ')
        {
            char *num = &command[2];
            int editor_index = SDL_atoi(num);
            if (editor_index > 0 && editor_index <= application.editors.length)
            {
                application.current_editor = editor_index - 1;
                return;
            }
        }
        if (SDL_strcmp(command, "l") == 0)
        {
            application.mode = List;
            return;
        }

        clean_text(&application.command_buffer);
        char invalid_cmd[100];
        sprintf(invalid_cmd, "Invalid command: %s", command);
        application.command_buffer = text_new(invalid_cmd);
        cursor_move_start_line(&application.command_buffer_cursor, &application.command_buffer);
    }
    break;
    case List:
    {
        application.mode = Insert;
    }
    break;
    case Insert:
    {
        if (editor->selection.is_active)
            clear_selection_text(&editor->selection, &editor->buffer, &editor->cursor);
        text_add(&editor->buffer, &editor->cursor, "\n");
        cursor_move_down(&editor->cursor, &editor->buffer);
        editor->focus_cursor = true;
    }
    break;
    }
}
void action_delete_char(Editor *editor, SDL_Event event)
{
    if (application.mode == Command)
    {
        text_remove_char(&application.command_buffer, &application.command_buffer_cursor);
        return;
    }
    if (editor->selection.is_active)
    {
        clear_selection_text(&editor->selection, &editor->buffer, &editor->cursor);
    }
    else
    {
        text_remove_char(&editor->buffer, &editor->cursor);
    }
    editor->focus_cursor = true;
}

typedef void (*ActionFunction)(Editor *, SDL_Event);
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
    &action_command_mode,
    &action_list_mode,
    &action_insert_mode,
    &action_save_file,
    &action_delete_file};

inline void Dispatch(Editor *editor, SDL_Event event, Action action)
{
    actionFunctions[action](editor, event);
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

bool is_command_mode(SDL_Event event)
{
    return event.key.key == SDLK_F && event.key.mod & SDL_KMOD_CTRL;
}
bool is_list_mode(SDL_Event event)
{
    return event.key.key == SDLK_L && event.key.mod & SDL_KMOD_CTRL;
}
bool is_insert_mode(SDL_Event event)
{
    return event.key.key == SDLK_ESCAPE && application.mode != Insert;
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
bool is_delete_file(SDL_Event event)
{
    return event.key.key == SDLK_X && application.mode == List;
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
    return event.key.key == SDLK_ESCAPE && application.mode == Insert;
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
    if (is_delete_file(event))
        return DeleteFile;
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
    if (is_command_mode(event))
        return CommandMode;
    if (is_list_mode(event))
        return ListMode;
    if (is_insert_mode(event))
        return InsertMode;
    if (is_quit(event))
        return Quit;
    return None;
}
