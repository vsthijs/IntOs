#ifndef KERNEL_MEMORY_H
#define KERNEL_MEMORY_H 1

#include <stdlib.h>

bool memmap_is_initialized();
void memmap_init();
void *memmap_get_usable_page(size_t index);
void *memmap_get_page(size_t index);
size_t memmap_get_usable_pages_c();

#endif