#ifndef KERNEL_MEMORY_H
#define KERNEL_MEMORY_H 1

#include <bitmap.h>
#include <stdlib.h>

typedef struct {
    size_t total_page_c;
    bitmap_t page_map;
} memman_t;

memman_t memman_new();

#endif