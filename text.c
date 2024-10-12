
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "text.h"

Text text = {
    .length = 0,
    .capacity = 16,
    .text = 0};

void text_init()
{
    text.text = malloc(sizeof(char) * text.capacity);
    text.text[0] = '\0'; // Null terminate the string or we get random garbage
}

void text_add(char *str)
{
    printf("Text add! %s", str);
    printf("Current text %s", text.text);
    size_t len = strlen(str);
    if (len + text.length >= text.capacity)
    {
        size_t s = sizeof(char) * text.capacity * 2;
        char *t = malloc(s);
        memcpy(t, text.text, text.length * sizeof(char));
        text.capacity = s;
    }
    strcat_s(text.text, text.capacity, str);
    text.length += len;
}