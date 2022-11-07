//
// Created by jan on 30.10.22.
//

#ifndef IFJ_PROJECT_TOKEN_STACK_H
#define IFJ_PROJECT_TOKEN_STACK_H

#include "strings.h"

typedef struct {
    dyn_str val;
    int type;
    int line;

} Token;

typedef struct {
    Token *tokens;
    unsigned int free_index;
    unsigned int capacity;
} Token_stack;

int init_token_stack(Token_stack *stack);

void free_tokens_stack(Token_stack *stack);

Token *get_top(Token_stack *stack);

int is_empty(Token_stack *stack);

int push_token(Token_stack *stack, Token token);

int remove_last_token(Token_stack *stack);

#endif //IFJ_PROJECT_TOKEN_STACK_H
