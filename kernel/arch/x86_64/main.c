#include "memory.h"
#include "paging.h"
#include "terminal.h"
#include <bitmap.h>
#include <stdio.h>

#ifdef _TARGET_x86_64
#define _TARGET_NAME "x86_64"
#else
#define _TARGET_NAME "unknown"
#endif

#define _KERNEL_TEST 1

void main_x86_64() {
    term_init();
    glob_memman = memman_new();

    pagemapindexer_t pmi = pagemapindexer_new(0x1000 * 52 + 0x50000 * 7);
    printf("%dll %dll %dll %dll\n", pmi.page_dirptr_index, pmi.page_dir_index,
           pmi.page_table_index, pmi.page_index);
}
