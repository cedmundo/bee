//
// Created by carlos on 9/3/22.
//

#include <memory.h>
#include "common.h"

struct bee_rc_header {
    size_t rc;
    bee_free_f free_f;
};

void *bee_rc_alloc(size_t size, bee_free_f free_f) {
    assert(size != 0);
    struct bee_rc_header *header = calloc(1, sizeof(struct bee_rc_header) + size);
    if (header == NULL) {
        return NULL;
    }

    header->rc = 1;
    header->free_f = free_f;
    return ((char *)header)+sizeof(struct bee_rc_header);
}

void *bee_rc_realloc(void *handle, size_t new_size) {
    assert(new_size != 0);
    struct bee_rc_header *old_header = (struct bee_rc_header *) (handle-sizeof(struct bee_rc_header));
    struct bee_rc_header *new_header = realloc(old_header, sizeof(struct bee_rc_header)+new_size);
    return ((char *)new_header)+sizeof(struct bee_rc_header);
}

void *bee_rc_inc(void *handle) {
    assert(handle != NULL && "bee_rc_inc: handle cannot be null");
    struct bee_rc_header *header = (struct bee_rc_header *) (handle-sizeof(struct bee_rc_header));
    header->rc++;
    return handle;
}

void *bee_rc_dec(void *handle) {
    assert(handle != NULL && "bee_rc_dec: handle cannot be null");
    struct bee_rc_header *header = (struct bee_rc_header *) (handle-sizeof(struct bee_rc_header));
    assert(header->rc >= 0 && "bee_rc_dec: tried to decrement an already dropped value");

    header->rc--;
    if (header->rc == 0) {
        if (header->free_f != NULL) {
            header->free_f(handle);
        }

        free(header);
    }
    return handle;
}

size_t bee_rc_get(void *handle) {
    assert(handle != NULL && "bee_rc_get: handle cannot be null");
    struct bee_rc_header *header = (struct bee_rc_header *) (handle-sizeof(struct bee_rc_header));
    return header->rc;
}