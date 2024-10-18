#include <stdlib.h>
#include "vector.h"

Vector vector_new(size_t element_size)
{
    Vector vec;
    vec.element_size = element_size;
    vec.capacity = element_size * 16;
    vec.data = calloc(vec.capacity, element_size); // Using calloc because we're using this for strings
    return vec;
}

void vector_push(void *element)
{
}

void vector_free(Vector *vec)
{
    free(vec->data);
}
