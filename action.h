#ifndef _ACTION_H
#define _ACTION_H
#include "SDL3/SDL.h"

typedef struct Editor Editor;

typedef enum
{
    None,
    Copy,
    Paste,
    Cut,
    SelectAll,
    MoveLeft,
    MoveRight,
    MoveDown,
    MoveUp,
    DebugPrint,
    InsertTab,
    InsertLine,
    DeleteChar,
    CommandMode,
    ListMode,
    InsertMode,
    SaveFile,
    DeleteFile,
    Quit
} Action;

void Dispatch(Editor *editor, SDL_Event event, Action action);
void (*ActionPtr)(Editor *);
Action get_action(SDL_Event event);

#endif // _ACTION_H