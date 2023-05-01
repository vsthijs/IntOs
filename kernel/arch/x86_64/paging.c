#include "paging.h"

pagemapindexer_t pagemapindexer_new(uint64_t vaddr) {
    pagemapindexer_t pmi = (pagemapindexer_t){0};
    vaddr >>= 12;
    pmi.page_index = vaddr & 0x1ff;
    vaddr >>= 9;
    pmi.page_table_index = vaddr & 0x1ff;
    vaddr >>= 9;
    pmi.page_dir_index = vaddr & 0x1ff;
    vaddr >>= 9;
    pmi.page_dirptr_index = vaddr & 0x1ff;
    vaddr >>= 9;
    return pmi;
}