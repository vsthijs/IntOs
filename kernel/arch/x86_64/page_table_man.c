#include "page_table_man.h"
#include "memory.h"
#include "paging.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

page_table_man_t page_table_man_new(pagetable_t *pml4addr) {
    page_table_man_t self = {0};
    self.pml4addr = pml4addr;
    return self;
}

void page_table_man_map_mem(page_table_man_t *self, void *vmem, void *pmem) {
    pagemapindexer_t indexer = pagemapindexer_new((uint64_t)vmem);
    pagedir_entry_t pde = self->pml4addr->entries[indexer.page_dirptr_index];
    pagetable_t *pdp;
    if (!pde.present) {
        pdp = (pagetable_t *)memman_request_page(&glob_memman);
        memset(pdp, 0, 0x1000);
        pde.address = (uint64_t)pdp >> 12;
        pde.present = true;
        pde.readwrite = true;
        self->pml4addr->entries[indexer.page_dirptr_index] = pde;
    } else {
        pdp = (pagetable_t *)((uint64_t)pde.address << 12);
    }

    pde = pdp->entries[indexer.page_dir_index];
    pagetable_t *pd;
    if (!pde.present) {
        pd = (pagetable_t *)memman_request_page(&glob_memman);
        memset(pdp, 0, 0x1000);
        pde.address = (uint64_t)pd >> 12;
        pde.present = true;
        pde.readwrite = true;
        pdp->entries[indexer.page_dir_index] = pde;
    } else {
        pd = (pagetable_t *)((uint64_t)pde.address << 12);
    }

    pde = pd->entries[indexer.page_dir_index];
    pagetable_t *pt;
    if (!pde.present) {
        pt = (pagetable_t *)memman_request_page(&glob_memman);
        memset(pdp, 0, 0x1000);
        pde.address = (uint64_t)pt >> 12;
        pde.present = true;
        pde.readwrite = true;
        pd->entries[indexer.page_dir_index] = pde;
    } else {
        pt = (pagetable_t *)((uint64_t)pde.address << 12);
    }

    pde = pt->entries[indexer.page_index];
    // TODO: continue
}