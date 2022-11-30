//
// Created by jan on 10/15/22.
//

#ifndef IFJ_PROJECT_RECURSIVE_DESCENT_H
#define IFJ_PROJECT_RECURSIVE_DESCENT_H

#include "token_stack.h"
#include "scanner.h"
#include "syntax_tree.h"
#include "precedent.h"
#include "code_generator.h"

typedef struct {
    char **variables;
    int free_index;
    int size;
} Variable_array;

int get_token_rec(FILE *input_file, Token_stack *token_stack);

int analyse_expression(FILE *input_file, Token_stack *token_stack, int if_while, struct tree_node *tree,
                       int spec_expression);

int analyse_arg(FILE *input_file, Token_stack *token_stack, struct tree_node *arg_body);

int analyse_assign(FILE *input_file, Token_stack *token_stack, struct tree_node *assign_node);

int analyse_return_type(FILE *input_file, Token_stack *token_stack, struct tree_node *return_node);

int analyse_param(FILE *input_file, Token_stack *token_stack, struct tree_node *param_node, Variable_array *var_arr);

int analyse_body(FILE *input_file, Token_stack *token_stack, struct tree_node *tree, Variable_array *var_array, int remove_index);

int analyse_prog(FILE *input_file, Token_stack *token_stack, struct tree_node *root_node, Variable_array *prog_var_array);

int analyse_prolog(FILE *input_file, Token_stack *token_stack, struct tree_node *tree);

int analyse_syntax(FILE *input_file);


#endif //IFJ_PROJECT_RECURSIVE_DESCENT_H
