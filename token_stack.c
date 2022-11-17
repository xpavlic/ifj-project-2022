//
// Created by jan on 30.10.22.
//

#include "token_stack.h"

const size_t token_stack_init_size = 16;

int init_token_stack(Token_stack *stack) {
    stack->tokens = (Token *) malloc(token_stack_init_size * sizeof(Token));
    if (!stack->tokens) {
        return 1;
    }
    stack->free_index = 0;
    stack->capacity = token_stack_init_size;
    return 0;
}


void free_tokens_stack(Token_stack *stack) {
    for (unsigned int i = 0; i < stack->free_index; i++) {
        free(stack->tokens[i].val.str);
    }
    free(stack->tokens);
    stack->tokens = NULL;
    stack->free_index = 0;
    stack->capacity = 0;
}


Token *get_top(Token_stack *stack) {
    if (stack->free_index == 0) {
        return NULL;
    }
    return &stack->tokens[stack->free_index - 1];
}


int is_empty(Token_stack *stack) {
    if (stack->free_index == 0) {
        return 0;
    }
    return 1;
}


int push_token(Token_stack *stack, Token token) {
    if (stack->free_index == stack->capacity) {
        stack->capacity *= 2;
        stack->tokens = (Token *) realloc(stack->tokens, stack->capacity * sizeof(Token));
        if (!stack->tokens) {
            return 1;
        }
    }
    stack->tokens[stack->free_index] = token;
    stack->free_index++;
    return 0;
}


int remove_last_token(Token_stack *stack) {
    if (stack->free_index == 0) {
        return 1;
    }

    stack->free_index--;
    free(stack->tokens[stack->free_index].val.str);
    return 0;
}
