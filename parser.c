//
// Created by carlos on 1/23/22.
//
#include "parser.h"
#include "lexer.h"
#include "error.h"

#include <stdio.h>

enum bee_parser_state {
    BEE_STATE_BASE,
    BEE_STATE_MODULE,
    BEE_STATE_MODULE_END,
    BEE_STATE_IMPORT,
    BEE_STATE_IMPORT_END,
    BEE_STATE_STRUCT,
    BEE_STATE_STRUCT_END,
    BEE_STATE_STRUCT_FIELDS,
    BEE_STATE_STRUCT_FIELD,
    BEE_STATE_STRUCT_FIELD_END,
    BEE_STATE_CONST,
    BEE_STATE_CONST_END,
    BEE_STATE_VAL,
    BEE_STATE_VAL_END,
    BEE_STATE_VAR,
    BEE_STATE_VAR_END,
    BEE_STATE_TYPE_BIND,
    BEE_STATE_ACCESS_PATH_ID,
    BEE_STATE_ACCESS_PATH_DOT,
};

#define here(t, m) printf("%lu:%lu \t| %s\n", (t)->row, (t)->col, (m))
#define consume(t) do{(t) = (t)->next;}while(0)
#define set_state(s) do{cur_state = (s);}while(0)
#define push_state(s) do{state_stack[++state_stack_top] = (s);}while(0)
#define pop_state() do{cur_state = state_stack[state_stack_top--]; state_stack[state_stack_top+1] = 0;}while(0)

void bee_parse(struct bee_token *token_start, struct bee_error *error_start) {
    enum bee_parser_state cur_state = BEE_STATE_BASE;
    struct bee_token *cur_token = token_start;
    struct bee_error *cur_error = error_start;
    (void) cur_error;

    enum bee_parser_state state_stack[512];
    size_t state_stack_top = 0L;

    while (cur_token != NULL) {
        if (cur_state == BEE_STATE_BASE) {
            if (bee_keyword_match(cur_token, BEE_KW_MODULE)) {
                here(cur_token, "module > start");
                set_state(BEE_STATE_MODULE);
                push_state(BEE_STATE_BASE);
            } else if (bee_keyword_match(cur_token, BEE_KW_IMPORT)) {
                here(cur_token, "import > start");
                set_state(BEE_STATE_IMPORT);
                push_state(BEE_STATE_BASE);
            } else if (bee_keyword_match(cur_token, BEE_KW_CONST)) {
                here(cur_token, "const > start");
                set_state(BEE_STATE_CONST);
                push_state(BEE_STATE_BASE);
            } else if (bee_keyword_match(cur_token, BEE_KW_VAL)) {
                here(cur_token, "val > start");
                set_state(BEE_STATE_VAL);
                push_state(BEE_STATE_BASE);
            } else if (bee_keyword_match(cur_token, BEE_KW_VAR)) {
                here(cur_token, "var > start");
                set_state(BEE_STATE_VAR);
                push_state(BEE_STATE_BASE);
            } else if (bee_keyword_match(cur_token, BEE_KW_STRUCT)) {
                here(cur_token, "struct > start");
                set_state(BEE_STATE_STRUCT);
                push_state(BEE_STATE_BASE);
            } else if (bee_token_match(cur_token, BEE_TT_EOL)) {
                consume(cur_token);
                continue;
            } else if (bee_token_match(cur_token, BEE_TT_EOF)) {
                here(cur_token, "root > end of file");
                break;
            } else {
                here(cur_token, "root > error");
                break;
            }
        } else if (cur_state == BEE_STATE_MODULE) {
            consume(cur_token);
            push_state(BEE_STATE_MODULE_END);
            set_state(BEE_STATE_ACCESS_PATH_ID);
        } else if (cur_state == BEE_STATE_MODULE_END) {
            if (bee_token_match(cur_token, BEE_TT_EOL)) {
                here(cur_token, "module > end");
                consume(cur_token);
                pop_state();
            } else {
                here(cur_token, "module > error: was expecting end of line");
                break;
            }
        } else if (cur_state == BEE_STATE_IMPORT) {
            consume(cur_token);
            push_state(BEE_STATE_IMPORT_END);
            set_state(BEE_STATE_ACCESS_PATH_ID);
        } else if (cur_state == BEE_STATE_IMPORT_END) {
            if (bee_token_match(cur_token, BEE_TT_EOL)) {
                here(cur_token, "import > end");
                consume(cur_token);
                pop_state();
            } else {
                here(cur_token, "import > error: was expecting end of line");
                break;
            }
        } else if (cur_state == BEE_STATE_CONST || cur_state == BEE_STATE_VAL || cur_state == BEE_STATE_VAR) {
            consume(cur_token);

            // Match id
            if (bee_token_match(cur_token, BEE_TT_WORD)) {
                here(cur_token, "const > id");
                consume(cur_token);
            } else {
                here(cur_token, "const > error: was expecting an id");
                break;
            }
        } else if (cur_state == BEE_STATE_CONST_END) {

        } else if (cur_state == BEE_STATE_VAL_END) {

        } else if (cur_state == BEE_STATE_VAR_END) {

        } else if (cur_state == BEE_STATE_STRUCT) {
            consume(cur_token); // struct

            if (bee_token_match(cur_token, BEE_TT_WORD)) {
                here(cur_token, "struct > id");
                consume(cur_token); // struct ID

                if (bee_token_match(cur_token, BEE_TT_EOL)) {
                    consume(cur_token); // struct ID EOL
                    push_state(BEE_STATE_STRUCT_END);
                    set_state(BEE_STATE_STRUCT_FIELDS);
                } else {
                    here(cur_token, "struct > error: was expecting end of line");
                    break;
                }
            } else {
                here(cur_token, "struct > error: was expecting id");
                pop_state();
                break;
            }
        } else if (cur_state == BEE_STATE_STRUCT_FIELDS) {
            if (bee_keyword_match(cur_token, BEE_KW_END)) {
                here(cur_token, "struct > fields > end");
                consume(cur_token); // end
                pop_state();
            } else {
                here(cur_token, "struct > fields > field start");
                push_state(BEE_STATE_STRUCT_FIELDS);
                set_state(BEE_STATE_STRUCT_FIELD);
            }
        } else if (cur_state == BEE_STATE_STRUCT_END) {
            if (bee_token_match(cur_token, BEE_TT_EOL)) {
                here(cur_token, "struct > end");
                consume(cur_token); // EOL
                pop_state();
                continue;
            } if (bee_keyword_match(cur_token, BEE_KW_STRUCT)) {
                here(cur_token, "struct > end [struct]");
                consume(cur_token); // struct
                continue;
            } if (bee_token_match(cur_token, BEE_TT_WORD)) {
                here(cur_token, "struct > end [id]");
                consume(cur_token); // any id
                continue;
            } else {
                here(cur_token, "struct > error: was expecting end of line");
                break;
            }
        } else if (cur_state == BEE_STATE_STRUCT_FIELD) {
            // Match modifier
            if (bee_keyword_match(cur_token, BEE_KW_CONST)) {
                here(cur_token, "struct > fields > field > const modifier");
                consume(cur_token);
            } else if (bee_keyword_match(cur_token, BEE_KW_VAL)) {
                here(cur_token, "struct > fields > field > val modifier");
                consume(cur_token);
            } else if (bee_keyword_match(cur_token, BEE_KW_VAR)) {
                here(cur_token, "struct > fields > field > var modifier");
                consume(cur_token);
            }

            // Match id
            if (bee_token_match(cur_token, BEE_TT_WORD)) {
                here(cur_token, "struct > fields > field > id");
                consume(cur_token);
            } else {
                here(cur_token, "struct > fields > field > error: was expecting an id");
                break;
            }

            push_state(BEE_STATE_STRUCT_FIELD_END);
            set_state(BEE_STATE_TYPE_BIND);
            here(cur_token, "struct > fields > field > type bind start");
        } else if (cur_state == BEE_STATE_STRUCT_FIELD_END) {
            if (bee_token_match(cur_token, BEE_TT_EOL)) {
                here(cur_token, "struct > fields > field > end");
                consume(cur_token); // EOL
                pop_state();
            } else {
                here(cur_token, "struct > fields > field > error: was expecting end of line");
                break;
            }
        } else if (cur_state == BEE_STATE_TYPE_BIND) {
            if (bee_punct_match(cur_token, ':')) {
                consume(cur_token);
                if (bee_token_match(cur_token, BEE_TT_WORD)) {
                    here(cur_token, "type bind > id");
                    consume(cur_token);
                    pop_state();
                } else {
                    here(cur_token, "type bind > error: was expecting an id");
                    break;
                }
            } else {
                here(cur_token, "type bind > error: was expecting punct ':'");
                break;
            }
        } else if (cur_state == BEE_STATE_ACCESS_PATH_ID) {
            if (bee_token_match(cur_token, BEE_TT_WORD)) {
                here(cur_token, "access path > id");
                consume(cur_token); // ID
                set_state(BEE_STATE_ACCESS_PATH_DOT);
            } else {
                here(cur_token, "access path > end");
                pop_state();
            }
        } else if (cur_state == BEE_STATE_ACCESS_PATH_DOT) {
            if (bee_punct_match(cur_token, '.')) {
                here(cur_token, "access path > dot");
                consume(cur_token); // '.'
                set_state(BEE_STATE_ACCESS_PATH_ID);
            } else {
                here(cur_token, "access path > end");
                pop_state();
            }
        }
    }
}
