#ifndef _VECTOR_H
#define _VECTOR_H

#include <string.h>

/**
 * My implementation of a generic vector type. it's probably not optimal. (the type info probably should be done via macro)
 */

typedef struct
{
    void *data;
    unsigned short element_size;
    size_t length;
    size_t capacity;
} Vector;

Vector vector_new(size_t element_size);
void vector_add(Vector *vec, int position, const void *element);
void vector_push(Vector *vec, const void *element);
void vector_pop(Vector *vec);
void vector_free(Vector *vec);
void vector_remove(Vector *vec, int position);
void debug_vec(Vector *vec);

#endif