#include "bitmap.h"
#include "memory.h"
#include "stdio.h"
#include "stdlib.h"
#include "terminal.h"

#ifdef _TARGET_x86_64
#define _TARGET_NAME "x86_64"
#else
#define _TARGET_NAME "unknown"
#endif

#define _KERNEL_TEST 1

void main_x86_64() {
    term_init();
    printf("\n");
    memmap_init();
    printf("\n");

#ifdef _KERNEL_TEST
    // tests
    bitmap_test();
    printf("\n");
#endif
}
