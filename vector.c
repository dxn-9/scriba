#include <stdlib.h>
#include "vector.h"

Vector vector_new(size_t element_size)
{
    Vector vec;
    vec.element_size = element_size;
    vec.length = 0;
    vec.capacity = element_size * 16;
    vec.data = calloc(vec.capacity, element_size); // Using calloc because we're using this for strings
    return vec;
}

void vector_push(Vector *vec, const void *element)
{
    if (vec->length == vec->capacity)
    {
        vec->capacity *= 2;
        vec->data = realloc(vec->data, vec->capacity * vec->element_size);
    }

    // Char* is 1 byte
    ((char *)vec->data)[vec->length * vec->element_size] = *(char *)element;
    vec->length++;
}

void vector_free(Vector *vec)
{
    free(vec->data);
}
