#ifndef _ACTION_H
#define _ACTION_H
#include "SDL3/SDL.h"

typedef struct Context Context;

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
    InsertCommand,
    SaveFile,
    Quit
} Action;

void Dispatch(Context *context, SDL_Event event, Action action);
void (*ActionPtr)(Context *);
Action get_action(SDL_Event event);

#endif // _ACTION_H