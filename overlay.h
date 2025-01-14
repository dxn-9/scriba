#ifndef _BOTTOM_BAR_H
#define _BOTTOM_BAR_H
#include <SDL3/SDL.h>
#include "cursor.h"

#define BOTTOM_BAR_H 24
#define BOTTOM_BAR_PADDING_X 10
#define TEMP_BOTTOM_BAR_COMMAND_X 300 // this should be calculated

void render_bottom_bar(Editor *editor);
void render_list_editors();
#endif