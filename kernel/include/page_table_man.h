#ifndef KERNEL_PAGE_TABLE_MAN_H
#define KERNEL_PAGE_TABLE_MAN_H 1

#include "paging.h"

typedef struct page_table_man {
    pagetable_t *pml4addr;
} page_table_man_t;

page_table_man_t page_table_man_new(pagetable_t *pml4addr);
void page_table_man_map_mem(page_table_man_t *self, void *vmem, void *pmem);

#endif