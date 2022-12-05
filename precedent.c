/**
 * @project Compiler IFJ22
 * @file    precedent.c
 * @authors Jan Pavlíček <xpavli95>
 */

#include "precedent.h"

enum precedent_types evaluate_token(Token *token, int if_while) {
    if (token == NULL) return EXP_EMPTY;
    else if (token->type == state_VARIABLE || token->type == state_INT || token->type == state_STRING ||
             token->type == state_FLOAT || strcmp(token->val.str, "null") == 0) {
        return NON_TERMINAL;
    } else if (token->type == state_LEFTPARENT) return LEFT_PARENT;
    else if (token->type == state_RIGHTPARENT) return RIGHT_PARENT;
    else if (token->type == state_TIMES || token->type == state_INTDIVIDE) return PRIO_MUL;
    else if (token->type == state_PLUS || token->type == state_MINUS || token->type == state_CONCANT) return PRIO_PLUS;
    else if (token->type == state_LESSTHAN || token->type == state_LESSEQUAL || token->type == state_MORETHAN ||
             token->type == state_MOREEQUAL)
        return PRIO_BIGGER;
    else if (token->type == state_OPEQUAL || token->type == state_NOTEQUAL) return PRIO_EQUAL;
    else if (if_while == 0 && token->type == state_SEMICOLON) return EXP_END;
    else if (if_while == 1 && token->type == state_CLEFTPARENT) return EXP_END;
    else return EXP_ERROR;
}

void free_precedent_token_stacks(Token_stack *token_stack, Token_stack *postfix_stack) {
    free_tokens_stack(token_stack);
    free_tokens_stack(postfix_stack);
}

enum tree_node_type state_to_node_type(state scanner_type) {
    if (scanner_type == state_VARIABLE) return VAR_OPERAND;
    else if (scanner_type == state_INT) return T_INT;
    else if (scanner_type == state_FLOAT) return T_FLOAT;
    else if (scanner_type == state_STRING) return T_STRING;
    else if (scanner_type == state_PLUS) return PLUS_OPERATOR;
    else if (scanner_type == state_MINUS) return MINUS_OPERATOR;
    else if (scanner_type == state_TIMES) return MULTIPLICATION_OPERATOR;
    else if (scanner_type == state_INTDIVIDE) return DIVISION_OPERATOR;
    else if (scanner_type == state_CONCANT) return CONCATENATION_OPERATOR;
    else if (scanner_type == state_LESSTHAN) return SMALLER_OPERATOR;
    else if (scanner_type == state_LESSEQUAL) return SMALLER_EQUAL_OPERATOR;
    else if (scanner_type == state_OPEQUAL) return EQUAL_OPERATOR;
    else if (scanner_type == state_MORETHAN) return BIGGER_OPERATOR;
    else if (scanner_type == state_MOREEQUAL) return BIGGER_EQUAL_OPERATOR;
    else if (scanner_type == state_NOTEQUAL) return NOT_EQUAL_OPERATOR;
    return T_NULL;
}

int build_expression_tree(Token_stack *postfix_stack, struct tree_node *expression_root) {
    Node_stack node_stack;
    init_node_stack(&node_stack);
    Token token;
    for (unsigned int i = 0; i < postfix_stack->free_index; i++) {
        token = postfix_stack->tokens[i];
        if (token.type == state_VARIABLE || token.type == state_INT || token.type == state_STRING ||
            token.type == state_FLOAT || strcmp(token.val.str, "null") == 0) {
            struct tree_node *new_node = init_tree_node();
            if (strcmp(token.val.str, "null") == 0) {
                add_tn_data(new_node, T_NULL, token.val.str);
            } else {
                add_tn_data(new_node, state_to_node_type(token.type), token.val.str);
            }
            node_stack_push(&node_stack, new_node);
        } else {
            struct tree_node *r_node = node_stack_top(&node_stack);
            node_stack_pop(&node_stack);
            struct tree_node *l_node = node_stack_top(&node_stack);
            node_stack_pop(&node_stack);
            struct tree_node *new_node = init_tree_node();
            if (add_tn_data(new_node, state_to_node_type(token.type), token.val.str) == NULL) return 99;
            add_tree_node_object(new_node, l_node);
            add_tree_node_object(new_node, r_node);
            node_stack_push(&node_stack, new_node);
        }
    }
    add_tree_node_object(expression_root, node_stack_top(&node_stack));
    free_node_stack(&node_stack);
    return 0;
}

int analyse_precedent(FILE *input_file, Token *first_token, Token *pre_first_token, struct tree_node *expression_root,
                      int if_while) {
    Token token;
    Token_stack token_stack;
    Token_stack postfix_stack;

    init_token_stack(&postfix_stack);
    init_token_stack(&token_stack);

    int prev_token_eval = evaluate_token(first_token, if_while);

    if (pre_first_token == NULL) {
        if (prev_token_eval == EXP_ERROR || prev_token_eval == RIGHT_PARENT || prev_token_eval == PRIO_MUL ||
            prev_token_eval == PRIO_PLUS || prev_token_eval == PRIO_BIGGER) {
            free_precedent_token_stacks(&token_stack, &postfix_stack);
            return 2;
        }
    } else {
        if (prev_token_eval != PRIO_MUL && prev_token_eval != PRIO_PLUS &&
            prev_token_eval != PRIO_BIGGER && prev_token_eval != EXP_END) {
            free_precedent_token_stacks(&token_stack, &postfix_stack);
            return 2;
        }
        push_token(&postfix_stack, pre_first_token);
    }
    if (prev_token_eval == NON_TERMINAL) {
        push_token(&postfix_stack, first_token);
    } else if (prev_token_eval != EXP_END) {
        push_token(&token_stack, first_token);
    }
    int stack_top_eval;
    while (prev_token_eval != EXP_END) {
        init_str(&token.val);
        if (get_token(input_file, &token) != 0) return 1;
        int token_eval = evaluate_token(&token, if_while);
        stack_top_eval = evaluate_token(get_top(&token_stack), if_while);
        if (token_eval == EXP_ERROR) {
            free_precedent_token_stacks(&token_stack, &postfix_stack);
            return 2;
        }

        if ((token_eval == PRIO_MUL || token_eval == PRIO_BIGGER || token_eval == PRIO_EQUAL ||
             token_eval == PRIO_PLUS) && (prev_token_eval != NON_TERMINAL && prev_token_eval != RIGHT_PARENT)) {
            free_precedent_token_stacks(&token_stack, &postfix_stack);
            return 2;
        }

        if (token_eval == RIGHT_PARENT && (prev_token_eval != NON_TERMINAL && prev_token_eval != RIGHT_PARENT)) {
            free_precedent_token_stacks(&token_stack, &postfix_stack);
            return 2;
        }

        if (token_eval == NON_TERMINAL && (prev_token_eval == RIGHT_PARENT || prev_token_eval == NON_TERMINAL)) {
            free_precedent_token_stacks(&token_stack, &postfix_stack);
            return 2;
        }

        if (token_eval == EXP_END && (prev_token_eval != RIGHT_PARENT && prev_token_eval != NON_TERMINAL)) {
            free_precedent_token_stacks(&token_stack, &postfix_stack);
            return 2;
        }

        if (token_eval == LEFT_PARENT && (prev_token_eval == RIGHT_PARENT || prev_token_eval == NON_TERMINAL)) {
            free_precedent_token_stacks(&token_stack, &postfix_stack);
            return 2;
        }

        if (token_eval == NON_TERMINAL) {
            push_token(&postfix_stack, &token);
        } else if (token_eval == LEFT_PARENT) {
            push_token(&token_stack, &token);
        } else if (token_eval == PRIO_MUL) {
            if (stack_top_eval == PRIO_MUL) {
                while (1) {
                    push_token(&postfix_stack, get_top(&token_stack));
                    remove_last_token(&token_stack);
                    stack_top_eval = evaluate_token(get_top(&token_stack), if_while);
                    if (stack_top_eval != PRIO_MUL) {
                        break;
                    }
                }
            }
            push_token(&token_stack, &token);
        } else if (token_eval == PRIO_PLUS) {
            if (stack_top_eval == PRIO_PLUS || stack_top_eval == PRIO_MUL) {
                while (1) {
                    push_token(&postfix_stack, get_top(&token_stack));
                    remove_last_token(&token_stack);
                    stack_top_eval = evaluate_token(get_top(&token_stack), if_while);
                    if (stack_top_eval != PRIO_MUL && stack_top_eval != PRIO_PLUS) {
                        break;
                    }
                }
            }
            push_token(&token_stack, &token);
        } else if (token_eval == PRIO_BIGGER) {
            if (stack_top_eval == PRIO_PLUS || stack_top_eval == PRIO_MUL || stack_top_eval == PRIO_BIGGER) {
                while (1) {
                    push_token(&postfix_stack, get_top(&token_stack));
                    remove_last_token(&token_stack);
                    stack_top_eval = evaluate_token(get_top(&token_stack), if_while);
                    if (stack_top_eval != PRIO_MUL && stack_top_eval != PRIO_PLUS &&
                        stack_top_eval != PRIO_BIGGER) {
                        break;
                    }
                }
            }
            push_token(&token_stack, &token);
        } else if (token_eval == PRIO_EQUAL) {
            if (stack_top_eval != LEFT_PARENT && stack_top_eval != EXP_EMPTY) {
                while (stack_top_eval != EXP_EMPTY) {
                    push_token(&postfix_stack, get_top(&token_stack));
                    remove_last_token(&token_stack);
                    stack_top_eval = evaluate_token(get_top(&token_stack), if_while);
                    if (stack_top_eval == LEFT_PARENT) {
                        break;
                    }
                }
            }
            push_token(&token_stack, &token);
        } else if (token_eval == RIGHT_PARENT) {
            if (stack_top_eval == LEFT_PARENT) {
                remove_last_token(&token_stack);
            } else {
                while (stack_top_eval != EXP_EMPTY) {
                    if (stack_top_eval == LEFT_PARENT) {
                        remove_last_token(&token_stack);
                        break;
                    } else {
                        push_token(&postfix_stack, get_top(&token_stack));
                        remove_last_token(&token_stack);
                        stack_top_eval = evaluate_token(get_top(&token_stack), if_while);
                    }
                }
            }
            if (stack_top_eval == EXP_EMPTY) {
                free_precedent_token_stacks(&token_stack, &postfix_stack);
                return 2;
            }
        }
        prev_token_eval = token_eval;
    }

    while (!is_empty(&token_stack)) {
        if (get_top(&token_stack)->type == state_LEFTPARENT) {
            free_precedent_token_stacks(&token_stack, &postfix_stack);
            return 2;
        }
        push_token(&postfix_stack, get_top(&token_stack));
        remove_last_token(&token_stack);
    }

    if (!is_empty(&postfix_stack) && expression_root != NULL) {
        build_expression_tree(&postfix_stack, expression_root);
    }

    free_precedent_token_stacks(&token_stack, &postfix_stack);
    return 0;
}
