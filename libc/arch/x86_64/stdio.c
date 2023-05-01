/*
 * int os - libk - stdio
 */

#include "stdio.h"

#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#ifdef __is_libk
// #include "terminal.h" // kernel header
void term_init();

// true if the terminal is initialized.
bool term_is_initialized();

// low level write function
void terminal_write(const char *, size_t len);
#endif

#ifdef __is_libk

char _print_buf[128];
size_t _print_buf_ptr = 0;
void flush() {
    terminal_write(_print_buf, _print_buf_ptr);
    _print_buf_ptr = 0;
}

static bool _print(const char *data, size_t length) {
    if (!term_is_initialized())
        return false;

    if (_print_buf_ptr + length > 128) {
        terminal_write(_print_buf, _print_buf_ptr);
    } else {
        bool should_flush = false;
        for (size_t ii = 0; ii < length; ii++) {
            if (data[ii] == '\n') {
                should_flush = true;
            }
            _print_buf[_print_buf_ptr++] = data[ii];
        }
        if (should_flush) {
            flush();
        }
    }

    return true;
}

int printf(const char *restrict format, ...) {
    va_list parameters;
    va_start(parameters, format);

    int written = 0;

    while (*format != '\0') {
        size_t maxrem = INT_MAX - written;

        if (format[0] != '%' || format[1] == '%') {
            if (format[0] == '%')
                format++;
            size_t amount = 1;
            while (format[amount] && format[amount] != '%')
                amount++;
            if (maxrem < amount) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            if (!_print(format, amount))
                return -1;
            format += amount;
            written += amount;
            continue;
        }

        const char *format_begun_at = format++;

        if (*format == 'c') {
            format++;
            char c = (char)va_arg(parameters, int /* char promotes to int */);
            if (!maxrem) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            if (!_print(&c, sizeof(c)))
                return -1;
            written++;
        } else if (*format == 's') {
            format++;
            const char *str = va_arg(parameters, const char *);
            size_t len = strlen(str);
            if (maxrem < len) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            if (!_print(str, len))
                return -1;
            written += len;
        } else if (*format == 'd') {
            format++;
            if (*format == 'h') {
                format++;
                if (*format == 'h') {
                    // char
                    format++;
                    const char *str =
                        i64_str((int64_t)va_arg(parameters, char));
                    size_t len = strlen(str);
                    if (maxrem < len) {
                        // TODO: Set errno to EOVERFLOW.
                        return -1;
                    }
                    if (!_print(str, len)) {
                        return -1;
                    }
                    written += len;
                } else {
                    // short
                    format++;
                    const char *str =
                        i64_str((int64_t)va_arg(parameters, short));
                    size_t len = strlen(str);
                    if (maxrem < len) {
                        // TODO: Set errno to EOVERFLOW.
                        return -1;
                    }
                    if (!_print(str, len)) {
                        return -1;
                    }
                    written += len;
                }
            } else if (*format == 'l') { // long *
                format++;
                if (*format == 'l') { // long long
                    format++;
                    const char *str =
                        i64_str((int64_t)va_arg(parameters, long long));
                    size_t len = strlen(str);
                    if (maxrem < len) {
                        // TODO: Set errno to EOVERFLOW.
                        return -1;
                    }
                    if (!_print(str, len)) {
                        return -1;
                    }
                    written += len;
                } else {
                    goto _PRINTF_INT_FMT;
                }
            } else {
            _PRINTF_INT_FMT:
                // int
                format++;
                const char *str = i64_str((int64_t)va_arg(parameters, int));
                size_t len = strlen(str);
                if (maxrem < len) {
                    // TODO: Set errno to EOVERFLOW.
                    return -1;
                }
                if (!_print(str, len)) {
                    return -1;
                }
                written += len;
            }
        } else if (*format == 'x') {
            format++;
            if (*format == 'h') {
                format++;
                if (*format == 'h') {
                    // char
                    format++;
                    const char *str =
                        u8_hex((uint8_t)va_arg(parameters, unsigned char));
                    size_t len = strlen(str);
                    if (maxrem < len) {
                        // TODO: Set errno to EOVERFLOW.
                        return -1;
                    }
                    if (!(_print("0x", 2) && _print(str, len))) {
                        return -1;
                    }
                    written += len + 2;
                } else {
                    // short
                    format++;
                    const char *str =
                        u16_hex((uint16_t)va_arg(parameters, unsigned short));
                    size_t len = strlen(str);
                    if (maxrem < len) {
                        // TODO: Set errno to EOVERFLOW.
                        return -1;
                    }
                    if (!(_print("0x", 2) && _print(str, len))) {
                        return -1;
                    }
                    written += len + 2;
                }
            } else if (*format == 'l') { // long *
                format++;
                if (*format == 'l') { // long long
                    format++;
                    const char *str = u64_hex(
                        (uint64_t)va_arg(parameters, unsigned long long));
                    size_t len = strlen(str);
                    if (maxrem < len) {
                        // TODO: Set errno to EOVERFLOW.
                        return -1;
                    }
                    if (!(_print("0x", 2) && _print(str, len))) {
                        return -1;
                    }
                    written += len + 2;
                } else {
                    goto _PRINTF_HEX_INT_FMT;
                }
            } else {
            _PRINTF_HEX_INT_FMT:
                // int
                format++;
                const char *str =
                    u32_hex((uint32_t)va_arg(parameters, unsigned int));
                size_t len = strlen(str);
                if (maxrem < len) {
                    // TODO: Set errno to EOVERFLOW.
                    return -1;
                }
                if (!(_print("0x", 2) && _print(str, len))) {
                    return -1;
                }
                written += len + 2;
            }
        } else if (*format == 'p') {
            format++;
            const char *str = u64_hex((uint64_t)va_arg(parameters, void *));
            size_t len = strlen(str);
            if (maxrem < len) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            if (!(_print("0x", 2) && _print(str, len))) {
                return -1;
            }
            written += len + 2;
        } else {
            format = format_begun_at;
            size_t len = strlen(format);
            if (maxrem < len) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            if (!_print(format, len))
                return -1;
            written += len;
            format += len;
        }
    }

    va_end(parameters);
    return written;
}

#endif