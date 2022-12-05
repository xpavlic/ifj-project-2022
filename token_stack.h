/**
 * @project Compiler IFJ22
 * @file    main.c
 * @authors Jan Pavlíček <xpavli95>
 */

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

/**
 * allocates and initiates token stack
 * @param stack stack to allocate
 * @return 0 if success else 1
 */
int init_token_stack(Token_stack *stack);

/**
 * Deallocates stack of tokens
 * @param stack stack of tokens
 */
void free_tokens_stack(Token_stack *stack);

/**
 * Returns token on top of stack of tokens
 * @param stack stack of tokens
 * @return token on top
 */
Token *get_top(Token_stack *stack);

/**
 * Checks if stack of tokens is empty
 * @param stack stack of tokens
 * @return 1 if empty else 0
 */
int is_empty(Token_stack *stack);

/**
 * Push token to stack of tokens
 * @param stack stack of tokens
 * @param token token
 * @return 0 if success else 1
 */
int push_token(Token_stack *stack, Token *token);

/**
 * Removes last token from stack of tokens
 * @param stack stack of tokens
 * @return 0 if success else 1
 */
int remove_last_token(Token_stack *stack);

#endif //IFJ_PROJECT_TOKEN_STACK_H
