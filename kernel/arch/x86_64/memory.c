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

const char *LIMINE_MEMMAP_TYPES[] = {
    "LIMINE_MEMMAP_USABLE",
    "LIMINE_MEMMAP_RESERVED",
    "LIMINE_MEMMAP_ACPI_RECLAIMABLE",
    "LIMINE_MEMMAP_ACPI_NVS",
    "LIMINE_MEMMAP_BAD_MEMORY",
    "LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE",
    "LIMINE_MEMMAP_KERNEL_AND_MODULES",
    "LIMINE_MEMMAP_FRAMEBUFFER",
};

bool _influence_mem_stats = false;

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
    size_t used_pages = 0;

    // TASK 1 (loop entries and get page count)
    for (size_t ii = 0; ii < memmap->entry_count; ii++) {
        memmap_entry_t *entry = memmap->entries[ii];
        size_t pages = entry->length / 4096;
        if (entry->type == LIMINE_MEMMAP_USABLE) {
            usable_pages += pages;
        } else {
            used_pages += pages;
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
                   .used_page_c = used_pages,
                   .free_page_c = usable_pages,
                   .page_map = (bitmap_t){found_page, total_pages}};

    // TASK 5 (loop through pages and set bitmap)
    printf("  indexing memory {\n");
    // size_t bitmap_page_index = page_index; // ?
    page_index = 0;
    for (size_t ii = 0; ii < memmap->entry_count; ii++) {
        memmap_entry_t *entry = memmap->entries[ii];
        size_t pages_start = page_index;
        size_t pages_end = pages_start + (entry->length / 4096);
        if (entry->type != LIMINE_MEMMAP_USABLE) {
            printf("    found %dll %s pages\n", pages_end - pages_start,
                   LIMINE_MEMMAP_TYPES[entry->type]);
            memman_lock_pages(&mm, (void *)entry->base, (entry->length / 4096));
        } else {
            printf("    found %dll usable pages\n", pages_end - pages_start);
            memman_free_pages(&mm, (void *)entry->base, (entry->length / 4096));
        }
    }
    printf("  } success\n");
    printf("  page summary {\n");
    printf("    total: %dll\n", mm.total_page_c);
    printf("    free: %dll\n", mm.free_page_c);
    printf("    used: %dll\n", mm.used_page_c);
    printf("  }\n");
    printf("} success\n");

    initialized++;
    _influence_mem_stats = true;
    return mm;
}

void *memman_request_page(memman_t *self) {
    for (uint64_t ii = 0; ii < self->page_map.size; ii++) {
        if (bitmap_get(&self->page_map, ii) == false) {
            memman_lock_page(self, (void *)(ii * 4096));
            return (void *)(ii * 4096);
        }
    }

    printf("kernel/memory: error: out of memory, returning NULL\n");
    return NULL;
}

void memman_free_page(memman_t *self, void *addr) {
    uint64_t index = (uint64_t)addr / 4096;
    if (bitmap_get(&self->page_map, index) == false)
        return;
    bitmap_set(&self->page_map, index, false);
    if (_influence_mem_stats) {
        self->free_page_c++;
        self->used_page_c--;
    }
}

void memman_lock_page(memman_t *self, void *addr) {
    uint64_t index = (uint64_t)addr / 4096;
    if (bitmap_get(&self->page_map, index) == true)
        return;
    bitmap_set(&self->page_map, index, true);
    if (_influence_mem_stats) {
        self->free_page_c--;
        self->used_page_c++;
    }
}

void memman_free_pages(memman_t *self, void *addr, size_t pages) {
    for (size_t ii = 0; ii < pages; ii++) {
        memman_free_page(self, (void *)((uint64_t)addr + (ii * 4096)));
    }
}

void memman_lock_pages(memman_t *self, void *addr, size_t pages) {
    for (size_t ii = 0; ii < pages; ii++) {
        memman_lock_page(self, (void *)((uint64_t)addr + (ii * 4096)));
    }
}