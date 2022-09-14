//
// Created by carlos on 9/3/22.
//

#include "common.h"

struct bee_rc_header {
    size_t rc;
    bee_free_f free_f;
};

void *bee_rc_alloc(size_t size, bee_free_f free_f) {
    assert(size != 0);
    struct bee_rc_header *header = calloc(sizeof(char), sizeof(struct bee_rc_header) + size);
    if (header == NULL) {
        return NULL;
    }

    header->rc = 1;
    header->free_f = free_f;
    return header+sizeof(struct bee_rc_header);
}

void *bee_rc_inc(void *handle) {
    struct bee_rc_header *header = (struct bee_rc_header *) handle-sizeof(struct bee_rc_header);
    header->rc++;
    return handle;
}

void *bee_rc_dec(void *handle) {
    struct bee_rc_header *header = (struct bee_rc_header *) handle-sizeof(struct bee_rc_header);
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
    struct bee_rc_header *header = (struct bee_rc_header *) handle-sizeof(struct bee_rc_header);
    return header->rc;
}