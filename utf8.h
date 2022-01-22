//
// Created by carlos on 1/19/22.
//

#ifndef BEE_UTF8_H
#define BEE_UTF8_H

#include <stddef.h>
#include <stdint.h>

uint8_t utf8_encode(uint32_t codepoint, char *dest);
uint8_t utf8_decode(const char src[4], uint32_t *codepoint);

#endif //BEE_UTF8_H