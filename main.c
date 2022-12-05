/**
 * @project Compiler IFJ22
 * @file    main.c
 * @authors Jan Pavlíček <xpavli95>
 */


#include <stdio.h>
#include "recursive_descent.h"

int main(void) {
    int result = analyse_syntax(stdin);
    return result;
}
