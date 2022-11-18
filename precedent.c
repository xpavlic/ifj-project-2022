//
// Created by jan on 17.11.22.
//

#include "precedent.h"

int evaluate_token(Token *token) {
    if (token->type == state_VARIABLE || token->type == state_INT || token->type == state_STRING ||
        token->type == state_FLOAT || strcmp(token->val.str, "null") == 0) {
        return NON_TERMINAL;
    } else if (token->type == state_LEFTPARENT) return state_LEFTPARENT;
    else if (token->type == state_RIGHTPARENT) return state_RIGHTPARENT;
    else if (token->type == state_SEMICOLON || token->type == state_CLEFTPARENT) return EXP_END;
    else if (token->type == state_TIMES || token->type == state_INTDIVIDE) return PRIO_MUL;
    else if (token->type == state_PLUS || token->type == state_MINUS || token->type == state_CONCANT) return PRIO_PLUS;
    else if (token->type == state_LESSTHAN || token->type == state_)
}

int get_token_rec(FILE *input_file, Token_stack *token_stack) {
    Token token;
    init_str(&token.val);
    if (get_token(input_file, &token) != 0) return 1;

    //printf("TOKEN: %s\n", token.val.str);
    //printf("TOKEN_TYPE: %i\n", token.type);

    if (push_token(token_stack, token) == 1) return 99;

    //printf("TOKEN_STACK TOP: %s\n", get_top(token_stack)->val.str);
    //printf("TOKEN_STACK TOP_TYPE: %i\n\n", get_top(token_stack)->type);
    return 0;
}

int analyse_precedent(FILE *input_file, Token *first_token, struct tree_node *expression_root) {
    Token token;
    Token_stack token_stack;
    init_token_stack(&token_stack);
    push_token(&token_stack, *first_token);
    while (!is_empty(&token_stack)) {

    }

}
