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
    PRIO_EQUAL // === !===
};


#endif //IFJ_PROJECT_PRECEDENT_H
