#include "memory.h"
#include <bitmap.h>
#include <limine.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// request
static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST, .revision = 1};

struct limine_memmap_response *memmap_response;

bool _memmap_is_initialized;

bool memmap_is_initialized() { return _memmap_is_initialized; }

uint64_t total_memory = 0;
uint64_t total_pages = 0;
uint64_t usable_memory = 0;
uint64_t usable_pages = 0;

bitmap_t page_usage_map;

size_t memmap_get_usable_pages_c() { return usable_pages; }

// TODO: implement
struct limine_memmap_entry *page_to_entry(size_t page);

void memmap_init() {
    printf("initializing memmap {\n");
    if (memmap_request.response == NULL) {
        printf("} failed\n");
        return;
    }
    memmap_response = memmap_request.response;

    total_memory = 0;
    total_pages = 0;
    usable_memory = 0;
    usable_pages = 0;

    uint64_t not_perf = 0; // incomplete pages
    uint64_t best_page = 0;

    for (size_t ii = 0; ii < memmap_request.response->entry_count; ii++) {
        struct limine_memmap_entry *entry =
            memmap_request.response->entries[ii];
        if (entry->type == LIMINE_MEMMAP_USABLE) {
            best_page = total_pages + 1;
            usable_memory += entry->length;
            if (entry->length % 4096 != 0) {
                not_perf++;
            }
            usable_pages += entry->length / 4096;
            printf("  found entry: [%p + %dll]\n", entry->base, entry->length);
        }
        total_pages += entry->length / 4096;
        total_memory += entry->length;
    }

    uint64_t bitmap_size = total_pages / 8 + 1;
    uint64_t bitmap_page_size = bitmap_size / 4096;
    if (bitmap_size % 4096 > 0)
        bitmap_page_size++;
    page_usage_map = bitmap_new(memmap_get_page(best_page), total_pages);
    for (size_t ii = best_page; ii < bitmap_page_size; ii++) {
        bitmap_set(&page_usage_map, ii, true);
    }
    size_t page_index = 0;
    for (size_t ii = 0; ii < memmap_response->entry_count; ii++) {
        struct limine_memmap_entry *entry = memmap_response->entries[ii];
        for (size_t jj = 0; jj < entry->length; jj += 4096) {
            if (entry->type != LIMINE_MEMMAP_USABLE) {
                bitmap_set(&page_usage_map, page_index, true);
            }
            page_index++;
        }
    }
    // printf("\n\n");
    // bitmap_print(&page_usage_map);
    // printf("\n\n");

    printf("\n  total memory:  %dll MiB\n", total_memory / (1024 * 1024));
    printf("  usable memory: %dll MiB\n", usable_memory / (1024 * 1024));
    printf("  total pages: %dll\n", total_pages);
    printf("  usable pages:  %dll\n", usable_pages);
    if (not_perf > 0) {
        printf("  ERR: found incomplete page\n} failed\n");
        return;
    }

    printf("} success\n");
    _memmap_is_initialized = true;
}

void *memmap_get_usable_page(size_t index) {
    if (!(index < usable_pages)) {
        return NULL;
    }

    size_t curr_index = 0;
    for (size_t ii = 0; ii < memmap_response->entry_count; ii++) {
        struct limine_memmap_entry *entry = memmap_response->entries[ii];
        if (entry->type == LIMINE_MEMMAP_USABLE) {
            if ((entry->length / 4096) + curr_index > index) {
                size_t rel_index = index - curr_index;
                for (size_t jj = 0; jj < entry->length / 4096; jj++) {
                    if (jj == rel_index) {
                        return (void *)((char *)entry->base + jj);
                    }
                }
            } else {
                curr_index += entry->length / 4096;
            }
        }
    }

    printf("kernel/memory/get_page: error: could not find page '%dll'\n",
           index);
    return NULL;
}

void *memmap_get_page(size_t index) {
    if (!(index < usable_pages)) {
        return NULL;
    }

    size_t curr_index = 0;
    for (size_t ii = 0; ii < memmap_response->entry_count; ii++) {
        struct limine_memmap_entry *entry = memmap_response->entries[ii];
        if ((entry->length / 4096) + curr_index > index) {
            size_t rel_index = index - curr_index;
            for (size_t jj = 0; jj < entry->length / 4096; jj++) {
                if (jj == rel_index) {
                    return (void *)((char *)entry->base + jj);
                }
            }
        } else {
            curr_index += entry->length / 4096;
        }
    }

    printf("kernel/memory/get_page: error: could not find page '%dll'\n",
           index);
    return NULL;
}
