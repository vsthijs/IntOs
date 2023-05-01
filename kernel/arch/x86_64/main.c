#include "memory.h"
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
    printf("\n");
    memman_t memorymanager = memman_new();
    printf("\n");
}
