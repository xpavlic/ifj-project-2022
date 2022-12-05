/**
 * @project Compiler IFJ22
 * @file    recursive_descent.h
 * @authors Jan Pavlíček <xpavli95>
 */

#ifndef IFJ_PROJECT_RECURSIVE_DESCENT_H
#define IFJ_PROJECT_RECURSIVE_DESCENT_H

#include "token_stack.h"
#include "scanner.h"
#include "syntax_tree.h"
#include "precedent.h"
#include "semantic.h"

/**
 * Function is calling scanner for tokens and saving tokens to token_stack
 * @param input_file file from which tokens are read
 * @param token_stack storing tokens
 * @return 0 if token was read else error code
 */
int get_token_rec(FILE *input_file, Token_stack *token_stack);

/**
 * Function for calling precedent analysis with right parameters
 * @param input_file file from which tokens are read
 * @param token_stack storing input tokens
 * @param if_while 1 analysing if or while expression else 0
 * @param tree tree node which will be root for expression syntax tree
 * @param spec_expression 1 if analysing expression where we need to pass two last tokens else 0
 * @return 0 if expression is syntactically right else error code
 */
int analyse_expression(FILE *input_file, Token_stack *token_stack, int if_while, struct tree_node *tree,
                       int spec_expression);

/**
 * Function for analysing arguments of function call stopping on ')' token
 * @param input_file file from which tokens are read
 * @param token_stack storing input tokens
 * @param tree_node syntax tree function call node
 * @return 0 if syntactically right else error code
 */
int analyse_arg(FILE *input_file, Token_stack *token_stack, struct tree_node *tree_node);

/**
 * Function for analysing assign (function call or expression)
 * @param input_file file from which tokens are read
 * @param token_stack storing input tokens
 * @param tree_node syntax tree node for assign
 * @return 0 if syntactically right else error code
 */
int analyse_assign(FILE *input_file, Token_stack *token_stack, struct tree_node *tree_node);

/**
 * Function for analysing return type of function
 * @param input_file file from which tokens are read
 * @param token_stack storing input tokens
 * @param tree_node syntax tree node for function declaration
 * @return 0 if syntactically right else error code
 */
int analyse_return_type(FILE *input_file, Token_stack *token_stack, struct tree_node *tree_node);

/**
 * Function for analysing parameters of function
 * @param input_file file from which tokens are read
 * @param token_stack storing input tokens
 * @param tree_node syntax tree node for function declaration
 * @return 0 if syntactically right else error code
 */
int analyse_param(FILE *input_file, Token_stack *token_stack, struct tree_node *tree_node);

/**
 * Function for analysing function, if, else and while bodies
 * @param input_file file from which tokens are read
 * @param token_stack storing input tokens
 * @param tree_node syntax tree parent node
 * @return 0 if syntactically right else error code
 */
int analyse_body(FILE *input_file, Token_stack *token_stack, struct tree_node *tree_node);

/**
 * Function for analysing main program
 * @param input_file file from which tokens are read
 * @param token_stack storing input tokens
 * @param tree_node syntax tree node for main program body
 * @return 0 if syntactically right else error code
 */
int analyse_prog(FILE *input_file, Token_stack *token_stack, struct tree_node *tree_node);

/**
 * Function for analysing prolog
 * @param input_file file from which tokens are read
 * @param token_stack storing input tokens
 * @param tree_node syntax tree node for main program body
 * @return 0 if syntactically right else error code
 */
int analyse_prolog(FILE *input_file, Token_stack *token_stack, struct tree_node *tree_node);

/**
 * Function for printing syntax tree
 * @param root root node
 * @param level how deep node is (first call should be with 0)
 */
void print_tree(struct tree_node *root, int level);

/**
 * Function for starting syntax driven translation. At first it analyses syntax and builds syntax tree.
 * If syntax is right, function calls semantic check. If it is also right, it call code generator.
 * @param input_file file from which tokens are read
 * @return 0 if syntactically and semantically right else error code
 */
int analyse_syntax(FILE *input_file);


#endif //IFJ_PROJECT_RECURSIVE_DESCENT_H
