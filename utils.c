#include "utils.h"

Line *get_line_at(TextBuffer *buffer, int y)
{
    return ((Line *)buffer->lines.data + y);
}