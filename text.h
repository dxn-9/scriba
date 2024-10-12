
#ifndef _TEXT_H
#define _TEXT_H

typedef struct
{
    char *text;
    int capacity;
    int length;
} Text;

extern Text text;

void text_init();
void text_add(char *str);

#endif // _TEXT_H