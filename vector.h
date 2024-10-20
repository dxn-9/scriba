
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
void vector_push(Vector *vec, void *element);
void vector_free(Vector *vec);
