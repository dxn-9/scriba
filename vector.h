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

typedef struct
{
    size_t start, end;
} Range;

Vector vector_new(size_t element_size);
void vector_add(Vector *vec, size_t position, const void *element);
void vector_push(Vector *vec, const void *element);
void vector_pop(Vector *vec);
void vector_clear(Vector *vec);
void vector_free(Vector *vec);
void vector_remove(Vector *vec, size_t position);
void vector_remove_range(Vector *vec, Range range);
void debug_vec(Vector *vec);

#endif