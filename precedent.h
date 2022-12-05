/**
 * @project Compiler IFJ22
 * @file    precedent.h
 * @authors Jan Pavlíček <xpavli95>
 */

#ifndef IFJ_PROJECT_PRECEDENT_H
#define IFJ_PROJECT_PRECEDENT_H

#include "token_stack.h"
#include "scanner.h"
#include "syntax_tree.h"

enum precedent_types {
    NON_TERMINAL, // non-terminal
    LEFT_PARENT, // (
    RIGHT_PARENT, // )
    EXP_END, //expression end ; or {
    PRIO_MUL, // * /
    PRIO_PLUS, // + - .
    PRIO_BIGGER, // < <= > >=
    PRIO_EQUAL, // === !===
    EXP_ERROR,
    SPEC_NON_TERMINAL,
    EXP_EMPTY
};

/**
 * Returns precedent type of input token
 * @param token input token
 * @param if_while analysing if or while expression == 1  else 0
 * @return precedent_type of input token
 */
enum precedent_types evaluate_token(Token *token, int if_while);

/**
 * Free dynamically allocated token stacks passed as parameter
 * @param token_stack
 * @param postfix_stack
 */
void free_precedent_token_stacks(Token_stack *token_stack, Token_stack *postfix_stack);

/**
 * Convert scanner_token type to syntax tree node type
 * @param scanner_type type to convert
 * @return tree node type
 */
enum tree_node_type state_to_node_type(state scanner_type);

/**
 * Builds syntax tree for analysed expression from token_stack with tokens sorted in postfix form
 * @param postfix_stack token stack with tokens in postfix form
 * @param expression_root root of expression syntax tree
 * @return
 */
int build_expression_tree(Token_stack *postfix_stack, struct tree_node *expression_root);

/**
 * Function is calling scanner for input tokens and is checking if they are syntactically right, it also builds expression
 * in postfix format and calls function for building syntax tree for analysed expression.
 * @param input_file file from which tokens are read
 * @param first_token already read token by which we decided, that we need analyse expression
 * @param second_token already read token by which we decided, that we need analyse expression
 * @param expression_root root node for expression syntax tree
 * @param if_while 1 analysing if or while expression else 0
 * @return 0 if expression is syntactically right else error code
 */
int analyse_precedent(FILE *input_file, Token *first_token, Token *pre_first_token, struct tree_node *expression_root,
                       int if_while);


#endif //IFJ_PROJECT_PRECEDENT_H
