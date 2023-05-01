#ifndef LIBC_STDLIB_H
#define LIBC_STDLIB_H 1

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// kernel: freeze forever.
// user: not implemented (does same as kernel)
__attribute__((noreturn)) void abort();

// allocate memory
void *malloc(size_t);

// free memory
void free(void *);

/// *_str: turn integer in a string.
/// *_hex: turn integer in a hexadecimal string.

// u64 -> str
const char *u64_str(uint64_t);
// i64 -> str
const char *i64_str(int64_t);

// u64 -> hex
const char *u64_hex(uint64_t);
// u32 -> hex
const char *u32_hex(uint32_t);
// u16 -> hex
const char *u16_hex(uint16_t);
// u8  -> hex
const char *u8_hex(uint8_t);

#endif