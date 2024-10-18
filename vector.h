
#include <string.h>

/**
 * My implementation of a generic vector type. it's probably not optimal.
 */

typedef struct
{
    void *data;
    size_t element_size;
    size_t length;
    size_t capacity;
} Vector;

Vector vector_new(size_t element_size);
void vector_push(void *element);
void vector_free(Vector *vec);
