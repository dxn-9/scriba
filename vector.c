#include <stdlib.h>
#include <stdio.h>
#include "vector.h"
#include "text.h"

Vector vector_new(size_t element_size)
{
    Vector vec;
    vec.element_size = element_size;
    vec.length = 0;
    vec.capacity = element_size * 16;
    vec.data = calloc(vec.capacity, element_size); // Using calloc because we're using this for strings
    return vec;
}

void debug_vec(Vector *vec)
{
    printf("Vec::");
    for (int i = 0; i < vec->length; ++i)
    {
        if (vec->element_size == sizeof(Line))
        {
            printf("[Start: %i End: %i],", ((Line *)vec->data)[i].start, ((Line *)vec->data)[i].end);
        }
        else
        {
            printf("%c,", ((char *)vec->data)[i]);
        }
    }
    printf("\n");
}
void vector_remove(Vector *vec, int position)
{
    char *addr = &((char *)vec->data)[position * vec->element_size];
    memmove(addr, addr + vec->element_size, (vec->length - (position + 1)) * vec->element_size);
    vec->length--;
}

void vector_add(Vector *vec, int position, const void *element)
{
    if (position == vec->length)
    {
        vector_push(vec, element);
        return;
    }
    if (position < 0)
    {
        printf("Warning: position is -1\n");
        return;
    }
    if (vec->length == vec->capacity)
    {
        vec->capacity *= 2;
        vec->data = realloc(vec->data, vec->capacity * vec->element_size);
    }

    char *addr = &((char *)vec->data)[position * vec->element_size];
    memmove(addr + vec->element_size, addr, (vec->length - position) * vec->element_size);
    memcpy(addr, element, vec->element_size);
    vec->length++;
}

void vector_pop(Vector *vec)
{
    if (vec->length == 0)
    {
        return;
    }
    vec->length--;
}
void vector_push(Vector *vec, const void *element)
{
    if (vec->length == vec->capacity)
    {
        vec->capacity *= 2;
        vec->data = realloc(vec->data, vec->capacity * vec->element_size);
    }
    memcpy((char *)vec->data + vec->length * vec->element_size, element, vec->element_size);
    vec->length++;
}

void vector_free(Vector *vec)
{
    vec->element_size = 0;
    vec->capacity = 0;
    vec->length = 0;
    free(vec->data);
}
