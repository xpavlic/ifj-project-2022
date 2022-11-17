//
// Created by jan on 10/15/22.
//

#include "recursive_descent.h"


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

//TODO ONLY FOR TESTING OF RECURSIVE DESCENT -> WILL BE PERFORMED BY PRECEDENT ANALYSIS
// HAS TO SEND FIRST TOKEN TO PRECEDENT ANALYSIS STACK
int analyse_expression(FILE *input_file, Token_stack *token_stack, int if_while) {
    while (1) {
        if (if_while == 1 && get_top(token_stack)->type == state_CLEFTPARENT) return 0;
        if (if_while == 0 && get_top(token_stack)->type == state_SEMICOLON) return 0;
        if (get_token_rec(input_file, token_stack) != 0) return 1;
    }
}


int analyse_arg(FILE *input_file, Token_stack *token_stack) {
    if (get_token_rec(input_file, token_stack) != 0) return 1;
    if (get_top(token_stack)->type == state_RIGHTPARENT) {
        return 0;
    }
    while (1) {
        if (get_top(token_stack)->type == state_VARIABLE || get_top(token_stack)->type == state_INT ||
            get_top(token_stack)->type == state_STRING ||
            get_top(token_stack)->type == state_FLOAT || strcmp(get_top(token_stack)->val.str, "null") == 0) {
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type == state_COMMA) {
                if (get_token_rec(input_file, token_stack) != 0) return 1;
                continue;
            } else if (get_top(token_stack)->type == state_RIGHTPARENT) {
                return 0;
            } else return 2;
        } else return 2;
    }
}

int analyse_assign(FILE *input_file, Token_stack *token_stack) {
    if (get_token_rec(input_file, token_stack) != 0) return 1;
    if (get_top(token_stack)->type == state_IDENTIFIER) {
        if (get_token_rec(input_file, token_stack) != 0) return 1;
        if (get_top(token_stack)->type != state_LEFTPARENT) return 2;
        int result = analyse_arg(input_file, token_stack); //stops on )
        if (result != 0) return result;
        if (get_token_rec(input_file, token_stack) != 0) return 1;
        if (get_top(token_stack)->type != state_SEMICOLON) return 2;
        return 0;
    }
    return analyse_expression(input_file, token_stack, 0); //stops on ;
}


int analyse_return_type(FILE *input_file, Token_stack *token_stack) {
    if (get_token_rec(input_file, token_stack) != 0) return 1;
    if (get_top(token_stack)->type == state_COLON) {
        if (get_token_rec(input_file, token_stack) != 0) return 1;
        if (get_top(token_stack)->type != state_TYPE && strcmp(get_top(token_stack)->val.str, "void") != 0) return 2;
        if (get_token_rec(input_file, token_stack) != 0) return 1;
        if (get_top(token_stack)->type != state_CLEFTPARENT) return 2;
        return 0;
    } else if (get_top(token_stack)->type == state_CLEFTPARENT) return 0;
    return 2;
}


int analyse_param(FILE *input_file, Token_stack *token_stack) {
    if (get_token_rec(input_file, token_stack) != 0) return 1;
    if (get_top(token_stack)->type == state_RIGHTPARENT) {
        return 0;
    }
    while (1) {
        if (get_top(token_stack)->type == state_TYPE) {
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type == state_VARIABLE) {
                if (get_token_rec(input_file, token_stack) != 0) return 1;
                if (get_top(token_stack)->type == state_COMMA) {
                    if (get_token_rec(input_file, token_stack) != 0) return 1;
                    continue;
                } else if (get_top(token_stack)->type == state_RIGHTPARENT) {
                    return 0;
                } else return 2;
            } else return 2;
        } else return 2;
    }
}

int analyse_body(FILE *input_file, Token_stack *token_stack) {
    int result;
    while (1) {
        if (get_token_rec(input_file, token_stack) != 0) return 1;

        //function call
        if (get_top(token_stack)->type == state_IDENTIFIER) {
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type != state_LEFTPARENT) return 2;
            result = analyse_arg(input_file, token_stack); //stops on )
            if (result != 0) return result;
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type != state_SEMICOLON) return 2;
            continue;
        }

        //variable assign
        if (get_top(token_stack)->type == state_VARIABLE) {
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type != state_EQUAL) return 2;
            result = analyse_assign(input_file, token_stack); //stops on ;
            if (result != 0) return result;
            continue;
        }

        //if
        if (strcmp(get_top(token_stack)->val.str, "if") == 0) {
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type != state_LEFTPARENT) return 2;
            result = analyse_expression(input_file, token_stack, 1); //stops on {
            if (result != 0) return result;
            result = analyse_body(input_file, token_stack); //stops on }
            if (result != 0) return result;
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (strcmp(get_top(token_stack)->val.str, "else") != 0) return 2;
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type != state_CLEFTPARENT) return 2;
            result = analyse_body(input_file, token_stack); //stops on }
            if (result != 0) return result;
            continue;
        }
        //while
        if (strcmp(get_top(token_stack)->val.str, "if") == 0) {
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type != state_LEFTPARENT) return 2;
            result = analyse_expression(input_file, token_stack, 1); // stops on {
            if (result != 0) return result;
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type != state_CLEFTPARENT) return 2;
            result = analyse_body(input_file, token_stack); //stops on }
            if (result != 0) return result;
            continue;
        }

        //return
        if (strcmp(get_top(token_stack)->val.str, "return") == 0) {
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type != state_SEMICOLON) {
                result = analyse_expression(input_file, token_stack, 0); // stops on ;
                if (result != 0) return result;
            }
            continue;
        }

        if (get_top(token_stack)->type == state_CRIGHTPARENT) return 0;
        return 2;
    }
}


int analyse_prog(FILE *input_file, Token_stack *token_stack) {
    int result;
    while (1) {
        if (get_token_rec(input_file, token_stack) != 0) return 1;
        //function
        if (strcmp(get_top(token_stack)->val.str, "function") == 0) {
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type != state_IDENTIFIER) return 2;
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type == state_LEFTPARENT) {
                result = analyse_param(input_file, token_stack);
                if (result != 0) return result;
            } else return 2;
            result = analyse_return_type(input_file, token_stack); //stops on {
            if (result != 0) {
                return result;
            }
            result = analyse_body(input_file, token_stack); //stops on }
            if (result != 0) {
                return result;
            }
            continue;
        }

        //function call
        if (get_top(token_stack)->type == state_IDENTIFIER) {
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type != state_LEFTPARENT) return 2;
            result = analyse_arg(input_file, token_stack); //stops on )
            if (result != 0) return result;
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type != state_SEMICOLON) return 2;
            continue;
        }

        //variable assign
        if (get_top(token_stack)->type == state_VARIABLE) {
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type != state_EQUAL) return 2;
            result = analyse_assign(input_file, token_stack); //stops on ;
            if (result != 0) return result;
            continue;
        }

        //if
        if (strcmp(get_top(token_stack)->val.str, "if") == 0) {
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type != state_LEFTPARENT) return 2;
            result = analyse_expression(input_file, token_stack, 1); //stops on {
            if (result != 0) return result;
            result = analyse_body(input_file, token_stack); //stops on }
            if (result != 0) return result;
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (strcmp(get_top(token_stack)->val.str, "else") != 0) return 2;
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type != state_CLEFTPARENT) return 2;
            result = analyse_body(input_file, token_stack); //stops on }
            if (result != 0) return result;
            continue;
        }
        //while
        if (strcmp(get_top(token_stack)->val.str, "if") == 0) {
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type != state_LEFTPARENT) return 2;
            result = analyse_expression(input_file, token_stack, 1); // stops on {
            if (result != 0) return result;
            result = analyse_body(input_file, token_stack); //stops on }
            if (result != 0) return result;
            continue;
        }

        //return
        if (strcmp(get_top(token_stack)->val.str, "return") == 0) {
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type != state_SEMICOLON) {
                result = analyse_expression(input_file, token_stack, 0); // stops on ;
                if (result != 0) return result;
            }
            continue;
        }

        //?>
        if (strcmp(get_top(token_stack)->val.str, "?>") == 0) {
            continue;
        }

        //EOF
        if (get_top(token_stack)->type == state_EOF) {
            return 0;
        }
        return 2;
    }
}


int analyse_prolog(FILE *input_file, Token_stack *token_stack) {
    if (get_token_rec(input_file, token_stack) != 0) return 1;
    if (strcmp(get_top(token_stack)->val.str, "<?php") != 0) return 2;
    if (get_token_rec(input_file, token_stack) != 0) return 1;
    if (strcmp(get_top(token_stack)->val.str, "declare") != 0) return 2;
    if (get_token_rec(input_file, token_stack) != 0) return 1;
    if (strcmp(get_top(token_stack)->val.str, "(") != 0) return 2;
    if (get_token_rec(input_file, token_stack) != 0) return 1;
    if (strcmp(get_top(token_stack)->val.str, "strict_types") != 0) return 2;
    if (get_token_rec(input_file, token_stack) != 0) return 1;
    if (strcmp(get_top(token_stack)->val.str, "=") != 0) return 2;
    if (get_token_rec(input_file, token_stack) != 0) return 1;
    if (strcmp(get_top(token_stack)->val.str, "1") != 0) return 2;
    if (get_token_rec(input_file, token_stack) != 0) return 1;
    if (strcmp(get_top(token_stack)->val.str, ")") != 0) return 2;
    if (get_token_rec(input_file, token_stack) != 0) return 1;
    if (strcmp(get_top(token_stack)->val.str, ";") != 0) return 2;
    return analyse_prog(input_file, token_stack);
}

int analyse_syntax(FILE *input_file) {
    Token_stack token_stack;
    int stack_result = init_token_stack(&token_stack);
    if (stack_result == 1) return 99;

    //printf("STACK_RESULT: %i\n", stack_result);

    int result = analyse_prolog(input_file, &token_stack);

    //printf("TOKENSSTACK:\n ");
    //for (unsigned int i = 0; i < token_stack.free_index; i++) {
    //    printf("%s ", token_stack.tokens[i].val.str);
    //}
    //printf("\n");

    free_tokens_stack(&token_stack);
    return result;
}
