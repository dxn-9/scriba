
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "text.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define DEFAULT_CAPACITY = 16

TextBuffer text_new(char *initialStr)
{
    TextBuffer buffer;
    int len = strlen(initialStr);
    int capacity = MAX(len, 16);
    buffer.text = malloc(sizeof(char) * capacity);
    memset(buffer.text, 0, sizeof(char) * capacity);
    if (initialStr != NULL)
    {
        strcat(buffer.text, initialStr);
    }
    buffer.capacity = capacity;
    buffer.length = len;
    return buffer;
}

void text_append(TextBuffer *buffer, char *str)
{

    size_t len = strlen(str);
    // printf("text append! BufferLen: %i BufferCapacity: %i, strLen: %i, content: %s\n", buffer->length, buffer->capacity, len, buffer->text);
    if (len + buffer->length + 1 >= buffer->capacity)
    {
        // printf("Making buffer bigger! %i - current: %i\n", len, buffer->length);
        size_t s = sizeof(char) * (buffer->capacity * 2);
        char *t = malloc(s);
        memset(t, 0, s); // Clear the memory so that strcat works correctly
        memcpy(t, buffer->text, buffer->length * sizeof(char));
        free(buffer->text);
        buffer->text = t;
        buffer->capacity = s;
    }
    buffer->length += len;
    strcat(buffer->text, str);
}