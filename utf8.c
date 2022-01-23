//
// Created by carlos on 1/19/22.
//

#include "utf8.h"

uint8_t utf8_encode(uint32_t codepoint, char *dest) {
    if (codepoint < 0x80) {
        dest[0] = (char) codepoint;
        return 1;
    }

    if (codepoint < 0x800) {
        dest[0] = (char) ((codepoint >> 6) | 0xC0);
        dest[1] = (char) ((codepoint & 0x3F) | 0x80);
        return 2;
    }

    if (codepoint < 0x10000) {
        dest[0] = (char) ((codepoint >> 12) | 0xE0);
        dest[1] = (char) (((codepoint >> 6) & 0x3F) | 0x80);
        dest[2] = (char) ((codepoint & 0x3F) | 0x80);
        return 3;
    }

    if (codepoint < 0x110000) {
        dest[0] = (char) ((codepoint >> 18) | 0xF0);
        dest[1] = (char) (((codepoint >> 12) & 0x3F) | 0x80);
        dest[2] = (char) (((codepoint >> 6) & 0x3F) | 0x80);
        dest[3] = (char) ((codepoint & 0x3F) | 0x80);
        return 4;
    }

    return 0;
}

uint8_t utf8_decode(const char src[4], uint32_t *codepoint) {
    uint8_t lead = (uint8_t) src[0];

    if ((~lead & 0x80) == 0x80 && src[0]) {
        *codepoint = (uint32_t) src[0];
        return 1;
    }

    if ((~lead & 0x20) == 0x20 && src[0] && src[1]) {
        *codepoint = (uint32_t) (src[0] & 0x1F) << 6;
        *codepoint |= (uint32_t) (src[1] & 0x3F);
        return 2;
    }

    if ((~lead & 0x10) == 0x10 && src[0] && src[1] && src[2]) {
        *codepoint = (uint32_t) (src[0] & 0x0F) << 12;
        *codepoint |= (uint32_t) (src[1] & 0x3F) << 6;
        *codepoint |= (uint32_t) (src[2] & 0x3F);
        return 3;
    }

    if ((~lead & 0x08) == 0x08 && src[0] && src[1] && src[2] && src[3]) {
        *codepoint = (uint32_t) (src[0] & 0x0F) << 18;
        *codepoint |= (uint32_t) (src[1] & 0x3F) << 12;
        *codepoint |= (uint32_t) (src[2] & 0x3F) << 6;
        *codepoint |= (uint32_t) (src[3] & 0x3F);
        return 4;
    }

    *codepoint = 0L;
    return 0;
}