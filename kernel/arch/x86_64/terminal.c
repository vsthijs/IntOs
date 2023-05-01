#include "terminal.h"
#include <limine.h>
#include <stdbool.h>
#include <stdio.h>

limine_terminal_write w;
bool _term_is_initialized;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-parameter"

void __term_req_callback(struct limine_terminal *__t, uint64_t type, uint64_t a,
                         uint64_t b, uint64_t c);

static volatile struct limine_terminal_request terminal_request = {
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0,
    .callback = &__term_req_callback};

void __term_req_callback(struct limine_terminal *__t, uint64_t type, uint64_t a,
                         uint64_t b, uint64_t c) {
    terminal_write("?", 1);
}

#pragma GCC diagnostic pop

void term_init() {
    if (terminal_request.response == NULL ||
        terminal_request.response->terminal_count < 1) {
        abort(); // error (can't display message)
    } else {
        w = terminal_request.response->write;
        printf("initializing deprecated terminal {} success\n");
        _term_is_initialized = true;
    }
}

bool term_is_initialized() { return _term_is_initialized; }

void terminal_write(const char *str, size_t len) {
    w(terminal_request.response->terminals[0], str, len);
}