#include "memory.h"
#include <bitmap.h>
#include <limine.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// request
typedef struct limine_memmap_entry memmap_entry_t;
struct limine_memmap_response *memmap;
static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST, .revision = 1};

memman_t memman_new() {
    static int initialized = 0;

    if (initialized > 0) {
        printf("kernel/memory: error: memman already initialized\n");
    }
    printf("initializing memman {\n");
    if (memmap_request.response == NULL) {
        printf("} failed\n");
        return (memman_t){0};
    }

    memmap = memmap_request.response;

    size_t total_pages = 0;
    size_t usable_pages = 0;

    // TASK 1 (loop entries and get page count)
    for (size_t ii = 0; ii < memmap->entry_count; ii++) {
        memmap_entry_t *entry = memmap->entries[ii];
        size_t pages = entry->length / 4096;
        if (entry->type == LIMINE_MEMMAP_USABLE) {
            usable_pages += pages;
        }
        total_pages += pages;
    }

    // TASK 2 (calculate bitmap size)
    size_t bitmap_byte_size = total_pages / 8;
    if (total_pages % 8 > 0)
        bitmap_byte_size++;

    size_t bitmap_page_size = bitmap_byte_size / 4096;
    if (bitmap_byte_size % 4096 > 0)
        bitmap_page_size++;

    printf("  bm bytesize = %dll; bm pagesize = %dll;\n", bitmap_byte_size,
           bitmap_page_size);
    if (bitmap_page_size >= usable_pages) {
        printf("  kernel/memory: fatal: sufficient available memory to "
               "allocate page map (%dll available, %dll needed)\n} failed\n",
               usable_pages, bitmap_page_size);
        abort();
    }

    // TASK 3 (find best page(s) to store bitmap)
    size_t page_index = 0;
    void *found_page = NULL;
    for (size_t ii = 0; ii < memmap->entry_count; ii++) {
        memmap_entry_t *entry = memmap->entries[ii];
        if (entry->type == LIMINE_MEMMAP_USABLE) {
            if (entry->length / 4096 >= bitmap_page_size) {
                found_page = (void *)entry->base;
                break;
            }
        }
        page_index += entry->length / 4096;
    }
    if (found_page == NULL) {
        printf("  kernel/memory: fatal: sufficient available memory to "
               "allocate page map (%dll available, %dll needed)\n} failed\n",
               usable_pages, bitmap_page_size);
        abort();
    }

    // TASK 4 (initialize bitmap)
    memman_t mm = {.total_page_c = total_pages,
                   .page_map = (bitmap_t){found_page, total_pages}};

    // TASK 5 (loop through pages and set bitmap)
    printf("  indexing memory {\n");
    size_t bitmap_page_index = page_index;
    page_index = 0;
    for (size_t ii = 0; ii < memmap->entry_count; ii++) {
        memmap_entry_t *entry = memmap->entries[ii];
        size_t pages_start = page_index;
        size_t pages_end = pages_start + (entry->length / 4096);
        if (entry->type != LIMINE_MEMMAP_USABLE) {
            for (size_t jj = pages_start; jj < pages_end; jj++) {
                bitmap_set(&mm.page_map, jj, true); // true = not usable
            }
        } else {
            printf("    found %dll usable pages\n", pages_end - pages_start);
            for (size_t jj = pages_start; jj < pages_end; jj++) {
                if ((bitmap_page_index <= jj) ||
                    (bitmap_page_index + bitmap_page_size >= jj))
                    bitmap_set(&mm.page_map, jj, true); // used by bitmap
                else
                    bitmap_set(&mm.page_map, jj, false); // false = usable
            }
        }
    }
    printf("  } success\n");
    printf("} success\n");

    initialized++;
    return mm;
}
