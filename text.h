
#ifndef _TEXT_H
#define _TEXT_H

typedef struct
{
    char *text;
    int capacity;
    int length;
} TextBuffer;

TextBuffer text_new(char *initialStr);
void text_append(TextBuffer *buffer, char *str);

#endif // _TEXT_H