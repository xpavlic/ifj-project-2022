//
// Created by jan on 31.10.22.
//

#include <stdio.h>
#include "recursive_descent.h"

int main(void) {
    int result = analyse_syntax(stdin);
    //printf("%i\n", result);
    return result;
}
