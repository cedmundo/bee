//
// Created by carlos on 1/23/22.
//

#ifndef BEE_PARSER_H
#define BEE_PARSER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

struct bee_token;
struct bee_error;

void bee_parse(struct bee_token *token_start, struct bee_error *error_start);

#endif //BEE_PARSER_H
