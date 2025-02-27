#ifndef _CONSTANTS_H
#define _CONSTANTS_H

#define HORIZONTAL_VIEW_OFFSET 2
#define VERTICAL_VIEW_OFFSET 2
#define FPS_SAMPLE_SIZE 20
#define TABS_VIEW_SIZE 4
#define CURSOR_VIEW_SIZE 3
#define LINE_NUMBER_SPACE 5
#define TEXT_COLOR \
    (SDL_Color) { 255, 255, 255, 255 }
#define LINE_TEXT_COLOR \
    (SDL_Color) { 200, 200, 200, 255 }
#define BG_COLOR \
    (SDL_Color) { 45, 42, 46, 255 }
#define BG_ACCENT_COLOR \
    (SDL_Color) { 145, 145, 105, 255 }
#define LINE_COLOR \
    (SDL_Color) { 64, 60, 66, 255 }
#define EMPTY_LINE_SELECTED_VIEW_WIDTH 0.2

#define TEXT_PADDING 2.5
#define DEFAULT_WIDTH 1100
#define DEFAULT_HEIGHT 800
#define SCROLL_MULT 4.0
#define SCROLL_Y_MAX_PADDING 2
#define SCROLL_X_MAX_PADDING 2

#endif
