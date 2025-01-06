#include "key_bindings.h"
#include "action.h"
#include <stdbool.h>

// It may seem overkill but  this will be necessary for setting up personalized keybindings

bool is_paste(SDL_Event event)
{

#ifdef __APPLE__
    return event.key.key == SDLK_V && event.key.mod & SDL_KMOD_GUI;
#else
    return event.key.key == SDLK_V && e.key.mod & SDL_KMOD_CTRL;
#endif
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
    if (is_quit(event))
        return Quit;
    return None;
}
