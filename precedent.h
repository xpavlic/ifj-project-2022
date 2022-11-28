//
// Created by jan on 17.11.22.
//

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

int analyse_precedent(FILE *input_file, Token *first_token, Token *second_token, struct tree_node *expression_root,
                      int if_while);


#endif //IFJ_PROJECT_PRECEDENT_H
