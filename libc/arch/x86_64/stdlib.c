#include <stdlib.h>

__attribute__((noreturn)) void abort() {
    asm("cli");
    while (1) {
        asm volatile("hlt");
    }
}

////////////////
// int -> str //
////////////////

char u64_str_buf[128];
const char *u64_str(uint64_t value) {
    uint8_t size = 0;
    uint64_t sizetest = value;
    while (sizetest / 10 > 0) {
        sizetest /= 10;
        size++;
    }

    uint8_t index = 0;
    while (value / 10 > 0) {
        uint8_t remainder = value % 10;
        value /= 10;
        u64_str_buf[size - index] = remainder + '0';
        index++;
    }

    uint8_t remainder = value % 10;
    u64_str_buf[size - index] = remainder + '0';
    u64_str_buf[size + 1] = 0;

    return u64_str_buf;
}

char i64_str_buf[128];
const char *i64_str(int64_t value) {
    uint8_t isneg = 0;
    if (value < 0) {
        isneg = 1;
        value *= -1;
        i64_str_buf[0] = '-';
    }
    uint8_t size = 0;
    uint64_t sizetest = value;
    while (sizetest / 10 > 0) {
        sizetest /= 10;
        size++;
    }

    uint8_t index = 0;
    while (value / 10 > 0) {
        uint8_t remainder = value % 10;
        value /= 10;
        i64_str_buf[isneg + size - index] = remainder + '0';
        index++;
    }

    uint8_t remainder = value % 10;
    i64_str_buf[isneg + size - index] = remainder + '0';
    i64_str_buf[isneg + size + 1] = 0;

    return i64_str_buf;
}

////////////////
// int -> hex //
////////////////

char u64_hex_buf[128];
const char *u64_hex(uint64_t value) {
    uint64_t *valptr = &value;
    uint8_t *ptr;
    uint8_t tmp;
    uint8_t size = 8 * 2 - 1;
    for (uint8_t i = 0; i < size; i++) {
        ptr = ((uint8_t *)valptr + i);
        tmp = ((*ptr & 0xf0) >> 4);
        u64_hex_buf[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = (*ptr & 0x0f);
        u64_hex_buf[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    u64_hex_buf[size + 1] = 0;
    return u64_hex_buf;
}

char u32_hex_buf[128];
const char *u32_hex(uint32_t value) {
    uint32_t *valptr = &value;
    uint8_t *ptr;
    uint8_t tmp;
    uint8_t size = 4 * 2 - 1;
    for (uint8_t i = 0; i < size; i++) {
        ptr = ((uint8_t *)valptr + i);
        tmp = ((*ptr & 0xf0) >> 4);
        u32_hex_buf[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = (*ptr & 0x0f);
        u32_hex_buf[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    u32_hex_buf[size + 1] = 0;
    return u32_hex_buf;
}

char u16_hex_buf[128];
const char *u16_hex(uint16_t value) {
    uint16_t *valptr = &value;
    uint8_t *ptr;
    uint8_t tmp;
    uint8_t size = 2 * 2 - 1;
    for (uint8_t i = 0; i < size; i++) {
        ptr = ((uint8_t *)valptr + i);
        tmp = ((*ptr & 0xf0) >> 4);
        u16_hex_buf[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = (*ptr & 0x0f);
        u16_hex_buf[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    u16_hex_buf[size + 1] = 0;
    return u16_hex_buf;
}

char u8_hex_buf[128];
const char *u8_hex(uint8_t value) {
    uint8_t *valptr = &value;
    uint8_t *ptr;
    uint8_t tmp;
    uint8_t size = 1 * 2 - 1;
    for (uint8_t i = 0; i < size; i++) {
        ptr = ((uint8_t *)valptr + i);
        tmp = ((*ptr & 0xf0) >> 4);
        u8_hex_buf[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = (*ptr & 0x0f);
        u8_hex_buf[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    u8_hex_buf[size + 1] = 0;
    return u8_hex_buf;
}