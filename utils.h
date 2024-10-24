#include "text.h"

#ifndef _UTILS_H
#define _UTILS_H

Line *get_line_at(TextBuffer *buffer, int y);
size_t utf8_char_bytes_at(TextBuffer *buffer, size_t position);

#endif