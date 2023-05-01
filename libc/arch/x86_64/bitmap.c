#include <bitmap.h>
#include <stdbool.h>
#include <stdio.h>

bitmap_t bitmap_new(void *base, size_t size) { return (bitmap_t){base, size}; }

bool bitmap_get(bitmap_t *self, size_t index) {
    size_t major = index / 8;
    size_t minor = index % 8;
    if (major < self->size) {
        char *ptr = (char *)self->base + major;
        return *ptr & (char)(1 << minor);
    } else {
        // raise error?
        return false;
    }
}

void bitmap_set(bitmap_t *self, size_t index, bool value) {
    size_t major = index / 8;
    size_t minor = index % 8;
    if (major < self->size) {
        char *ptr = (char *)self->base + major;
        if (value) { // true
            *ptr |= (1 << minor);
        } else { // false
            *ptr &= ~(1 << minor);
        }
    } else {
        // raise error?
    }
}

void bitmap_print(bitmap_t *self) {
    for (size_t ii = 0; ii < self->size; ii++) {
        printf("%d", bitmap_get(self, ii));
    }
    printf("\n");
}

void bitmap_test() {
    printf("testing bitmap {\n");
    char bmbase[2] = {0};
    bitmap_t bm = bitmap_new(bmbase, 2 * 8);
    for (size_t ii = 0; ii < 2 * 8; ii++) {
        if (bitmap_get(&bm, ii) != false) {
            printf("  error: bitmap_get does not return false (index %dll)\n} "
                   "failed\n",
                   ii);
            return;
        }
        bitmap_set(&bm, ii, true);
        if (bitmap_get(&bm, ii) != true) {
            printf("  error: bitmap_get does not return true after set (index "
                   "%dll)\n} failed\n",
                   ii);
            return;
        }
    }
    printf("} success\n");
}