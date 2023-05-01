#ifndef KERNEL_PAGING_H
#define KERNEL_PAGING_H 1

#include <stdbool.h>
#include <stdint.h>

typedef struct pagedir_entry {
    bool present : 1;
    bool readwrite : 1;
    bool superuser : 1;
    bool writethrough : 1;
    bool cachedisabled : 1;
    bool accessed : 1;
    bool ignore0 : 1;
    bool largerpages : 1;
    bool ignore1 : 1;
    uint8_t available : 3;
    uint64_t address : 52;
} pagedir_entry_t;

typedef struct pagetable {
    pagedir_entry_t entries[512];
} __attribute__((aligned(0x1000))) pagetable_t;

typedef struct pagemapindexer {
    uint64_t page_dirptr_index;
    uint64_t page_dir_index;
    uint64_t page_table_index;
    uint64_t page_index;
} pagemapindexer_t;

pagemapindexer_t pagemapindexer_new(uint64_t vaddr);

#endif