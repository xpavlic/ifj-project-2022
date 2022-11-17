//
// Created by jan on 10/15/22.
//

#ifndef IFJ_PROJECT_RECURSIVE_DESCENT_H
#define IFJ_PROJECT_RECURSIVE_DESCENT_H

#include "token_stack.h"
#include "scanner.h"

int get_token_rec(FILE *input_file, Token_stack *token_stack);

int analyse_expression(FILE *input_file, Token_stack *token_stack, int if_while);

int analyse_arg(FILE *input_file, Token_stack *token_stack);

int analyse_assign(FILE *input_file, Token_stack *token_stack);

int analyse_return_type(FILE *input_file, Token_stack *token_stack);

int analyse_param(FILE *input_file, Token_stack *token_stack);

int analyse_body(FILE *input_file, Token_stack *token_stack);

int analyse_prog(FILE *input_file, Token_stack *token_stack);

int analyse_prolog(FILE* input_file , Token_stack *token_stack);

int analyse_syntax(FILE* input_file);


#endif //IFJ_PROJECT_RECURSIVE_DESCENT_H
