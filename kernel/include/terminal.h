/*
    limine_terminal (deprecated)
*/

#ifndef KERNEL_TERMINAL_H
#define KERNEL_TERMINAL_H 1

#include <stdlib.h>

void term_init();

// true if the terminal is initialized.
bool term_is_initialized();

// low level write function
void terminal_write(const char *, size_t len);

#endif