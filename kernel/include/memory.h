#ifndef KERNEL_MEMORY_H
#define KERNEL_MEMORY_H 1

#include <bitmap.h>
#include <stdlib.h>

typedef struct {
    size_t total_page_c;
    size_t used_page_c;
    size_t free_page_c;
    bitmap_t page_map;
} memman_t;

memman_t memman_new();

void *memman_request_page(memman_t *);

void memman_free_pages(memman_t *, void *, size_t);
void memman_lock_pages(memman_t *, void *, size_t);

void memman_free_page(memman_t *, void *);
void memman_lock_page(memman_t *, void *);

extern memman_t glob_memman;

#endif