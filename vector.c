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
            Line *line = &((Line *)vec->data)[i];
            printf("[Start: %i End: %i, Bytes: %zi, BytesOffset: %zi],", line->start, line->end, line->bytes, line->bytes_offset);
        }
        else
        {
            printf("%c,", ((char *)vec->data)[i]);
        }
    }
    printf("\n");
}

void vector_remove(Vector *vec, size_t position)
{
    char *addr = &((char *)vec->data)[position * vec->element_size];
    memmove(addr, addr + vec->element_size, (vec->length - (position + 1)) * vec->element_size);
    vec->length--;
}
// Removes a range from [start..end).
// end is not included
void vector_remove_range(Vector *vec, Range range)
{
    SDL_assert(range.end > range.start);
    char *addr = &((char *)vec->data)[range.start * vec->element_size];
    memmove(addr, addr + (range.end - range.start) * vec->element_size, (vec->length - range.end) * vec->element_size);
    vec->length -= (range.end - range.start);
}

void vector_add(Vector *vec, size_t position, const void *element)
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
