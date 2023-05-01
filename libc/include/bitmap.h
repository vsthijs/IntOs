#ifndef LIBC_BITMAP_H
#define LIBC_BITMAP_H 1

#include <stdlib.h>

typedef struct bitmap {
    void *base;
    size_t size;
} bitmap_t;

bitmap_t bitmap_new(void *, size_t);
bool bitmap_get(bitmap_t *, size_t);
void bitmap_set(bitmap_t *, size_t, bool);
void bitmap_print(bitmap_t *);
void bitmap_print_compact(bitmap_t *);
void bitmap_test();

#endif