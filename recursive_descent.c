//
// Created by jan on 10/15/22.
//

#include "recursive_descent.h"

int get_token_rec(FILE *input_file, Token_stack *token_stack) {
    Token token;
    init_str(&token.val);
    if (get_token(input_file, &token) != 0) return 1;

    // printf("TOKEN: %s\n", token.val.str);
    // printf("TOKEN_TYPE: %i\n", token.type);

    if (push_token(token_stack, &token) == 1) return 99;

    // printf("TOKEN_STACK TOP: %s\n", get_top(token_stack)->val.str);
    // printf("TOKEN_STACK TOP_TYPE: %i\n\n", get_top(token_stack)->type);
    return 0;
}

int analyse_expression(FILE *input_file, Token_stack *token_stack, int if_while, struct tree_node *expression_root,
                       int spec_expression) {
    int result;
    if (spec_expression == 0) {
        result = analyse_precedent(input_file, get_top(token_stack), NULL, expression_root, if_while);
    } else {
        result = analyse_precedent(input_file, get_top(token_stack),
                                   &token_stack->tokens[token_stack->free_index - 2],
                                   expression_root, if_while);
    }
    return result;
}

int analyse_arg(FILE *input_file, Token_stack *token_stack,
                struct tree_node *tree) {
    if (get_token_rec(input_file, token_stack) != 0) return 1;
    if (get_top(token_stack)->type == state_RIGHTPARENT) {
        return 0;
    }
    while (1) {
        if (get_top(token_stack)->type == state_VARIABLE) {
            tree->tail_child =
                add_tree_node(tree, VAR_OPERAND, get_top(token_stack)->val.str);
        } else if (get_top(token_stack)->type == state_INT) {
            tree->tail_child =
                add_tree_node(tree, T_INT, get_top(token_stack)->val.str);
        } else if (get_top(token_stack)->type == state_STRING) {
            tree->tail_child =
                add_tree_node(tree, T_STRING, get_top(token_stack)->val.str);
        } else if (get_top(token_stack)->type == state_FLOAT) {
            tree->tail_child =
                add_tree_node(tree, T_FLOAT, get_top(token_stack)->val.str);
        } else if (strcmp(get_top(token_stack)->val.str, "null") == 0) {
            tree->tail_child =
                add_tree_node(tree, T_NULL, get_top(token_stack)->val.str);
        } else
            return 2;
        if (get_token_rec(input_file, token_stack) != 0) return 1;
        if (get_top(token_stack)->type == state_COMMA) {
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            continue;
        } else if (get_top(token_stack)->type == state_RIGHTPARENT) {
            return 0;
        } else
            return 2;
    }
}

int analyse_assign(FILE *input_file, Token_stack *token_stack,
                   struct tree_node *tree) {
    if (get_token_rec(input_file, token_stack) != 0) return 1;
    if (get_top(token_stack)->type == state_IDENTIFIER) {
        tree->tail_child = add_tree_node(tree, FUNC_CALL, "function_call");
        tree->tail_child->tail_child = add_tree_node(
            tree->tail_child, NAME, get_top(token_stack)->val.str);
        if (get_token_rec(input_file, token_stack) != 0) return 1;
        if (get_top(token_stack)->type != state_LEFTPARENT) return 2;
        tree->tail_child->tail_child =
            add_tree_node(tree->tail_child, ARGUMENTS, "function_args");
        int result = analyse_arg(input_file, token_stack,
                                 tree->tail_child->tail_child);  // stops on )
        if (result != 0) return result;
        if (get_token_rec(input_file, token_stack) != 0) return 1;
        if (get_top(token_stack)->type != state_SEMICOLON) return 2;
        return 0;
    }
    add_tree_node(tree, EXPRESSION, "expression");
    return analyse_expression(input_file, token_stack, 0, tree->tail_child, 0); //stops on ;
}

int analyse_return_type(FILE *input_file, Token_stack *token_stack,
                        struct tree_node *parent) {
    if (get_token_rec(input_file, token_stack) != 0) return 1;
    if (get_top(token_stack)->type == state_COLON) {
        if (get_token_rec(input_file, token_stack) != 0) return 1;
        if (get_top(token_stack)->type != state_TYPE &&
            strcmp(get_top(token_stack)->val.str, "void") != 0)
            return 2;
        parent->tail_child =
            add_tree_node(parent, TYPE, get_top(token_stack)->val.str);
        if (get_token_rec(input_file, token_stack) != 0) return 1;
        if (get_top(token_stack)->type != state_CLEFTPARENT) return 2;
        return 0;
    } else if (get_top(token_stack)->type == state_CLEFTPARENT)
        return 0;
    return 2;
}

int analyse_param(FILE *input_file, Token_stack *token_stack,
                  struct tree_node *tree) {
    if (get_token_rec(input_file, token_stack) != 0) return 1;
    if (get_top(token_stack)->type == state_RIGHTPARENT) {
        return 0;
    }
    while (1) {
        if (get_top(token_stack)->type == state_TYPE) {
            int param_type = NULL_PARAMETER;
            if (!strcmp(get_top(token_stack)->val.str, "float")) {
                param_type = FLOAT_PARAMETER;
            }
            if (!strcmp(get_top(token_stack)->val.str, "int")) {
                param_type = INT_PARAMETER;
            }
            if (!strcmp(get_top(token_stack)->val.str, "string")) {
                param_type = STR_PARAMETER;
            }
            if (!strcmp(get_top(token_stack)->val.str, "?float")) {
                param_type = FLOAT_NULL_PARAMETER;
            }
            if (!strcmp(get_top(token_stack)->val.str, "?int")) {
                param_type = INT_NULL_PARAMETER;
            }
            if (!strcmp(get_top(token_stack)->val.str, "?string")) {
                param_type = STR_NULL_PARAMETER;
            }
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type == state_VARIABLE) {
                tree->tail_child = add_tree_node(tree, param_type,
                                                 get_top(token_stack)->val.str);
                if (get_token_rec(input_file, token_stack) != 0) return 1;
                if (get_top(token_stack)->type == state_COMMA) {
                    if (get_token_rec(input_file, token_stack) != 0) return 1;
                    continue;
                } else if (get_top(token_stack)->type == state_RIGHTPARENT) {
                    return 0;
                } else
                    return 2;
            } else
                return 2;
        } else
            return 2;
    }
}

int analyse_body(FILE *input_file, Token_stack *token_stack,
                 struct tree_node *tree) {
    int result;
    while (1) {
        if (get_token_rec(input_file, token_stack) != 0) return 1;
        // function call
        if (get_top(token_stack)->type == state_IDENTIFIER) {
            tree->tail_child = add_tree_node(tree, FUNC_CALL, "function_call");
            tree->tail_child->tail_child = add_tree_node(
                tree->tail_child, NAME, get_top(token_stack)->val.str);
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type != state_LEFTPARENT) return 2;
            tree->tail_child->tail_child =
                add_tree_node(tree->tail_child, ARGUMENTS, "function_args");
            result = analyse_arg(input_file, token_stack,
                                 tree->tail_child->tail_child);  // stops on )
            if (result != 0) return result;
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type != state_SEMICOLON) return 2;
            continue;
        }

        // variable assign
        if (get_top(token_stack)->type == state_VARIABLE) {
            char variable_name[strlen(get_top(token_stack)->val.str) + 1];
            strcpy(variable_name, get_top(token_stack)->val.str);
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type != state_EQUAL) {
                struct tree_node *assign_node = add_tree_node(tree, EXPRESSION, "expression");
                result = analyse_expression(input_file, token_stack, 0, assign_node, 1);
                if (result != 0) return result;
                continue;
            }
            struct tree_node *assign_node = add_tree_node(tree, ASSIGN, "assign");
            add_tree_node(assign_node, NAME, variable_name);
            result = analyse_assign(input_file, token_stack,
                                    assign_node);  // stops on ;
            if (result != 0) return result;
            continue;
        }

        if (get_top(token_stack)->type == state_STRING || get_top(token_stack)->type == state_INT ||
            get_top(token_stack)->type == state_FLOAT || strcmp(get_top(token_stack)->val.str, "null") == 0 ||
            get_top(token_stack)->type == state_LEFTPARENT) {
            struct tree_node *assign_node = add_tree_node(tree, EXPRESSION, "expression");
            result = analyse_expression(input_file, token_stack, 0, assign_node, 0);
            if (result != 0) return result;
            continue;
        }

        // if
        if (strcmp(get_top(token_stack)->val.str, "if") == 0) {
            tree->tail_child = add_tree_node(tree, IF, "IF");
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type != state_LEFTPARENT) return 2;
            tree->tail_child->tail_child = add_tree_node(tree->tail_child, EXPRESSION, "expression");
            result = analyse_expression(input_file, token_stack, 1, tree->tail_child->tail_child, 0); //stops on {
            if (result != 0) return result;
            tree->tail_child->tail_child =
                add_tree_node(tree->tail_child, BODY, "IF_BODY");
            result = analyse_body(input_file, token_stack,
                                  tree->tail_child->tail_child);  // stops on }
            if (result != 0) return result;
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (strcmp(get_top(token_stack)->val.str, "else") != 0) return 2;
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type != state_CLEFTPARENT) return 2;
            tree->tail_child->tail_child =
                add_tree_node(tree->tail_child, BODY, "ELSE_body");
            result = analyse_body(input_file, token_stack,
                                  tree->tail_child->tail_child);  // stops on }
            if (result != 0) return result;
            continue;
        }
        // while
        if (strcmp(get_top(token_stack)->val.str, "while") == 0) {
            tree->tail_child = add_tree_node(tree, WHILE, "while");
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type != state_LEFTPARENT) return 2;
            tree->tail_child->tail_child = add_tree_node(tree->tail_child, EXPRESSION, "expression");
            result = analyse_expression(input_file, token_stack, 1, tree->tail_child->tail_child, 0); // stops on {
            if (result != 0) return result;
            tree->tail_child->tail_child =
                add_tree_node(tree->tail_child, BODY, "while body");
            result = analyse_body(input_file, token_stack,
                                  tree->tail_child->tail_child);  // stops on }
            if (result != 0) return result;
            continue;
        }

        // return
        if (strcmp(get_top(token_stack)->val.str, "return") == 0) {
            tree->tail_child = add_tree_node(tree, RETURN, "return");
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type != state_SEMICOLON) {
                tree->tail_child->tail_child = add_tree_node(tree->tail_child, EXPRESSION, "expression");
                result = analyse_expression(input_file, token_stack, 0, tree->tail_child->tail_child, 0); // stops on ;
                if (result != 0) return result;
            }
            continue;
        }

        if (get_top(token_stack)->type == state_CRIGHTPARENT) return 0;
        return 2;
    }
}

int analyse_prog(FILE *input_file, Token_stack *token_stack,
                 struct tree_node *tree) {
    int result;
    while (1) {
        if (get_token_rec(input_file, token_stack) != 0) return 1;
        // function
        if (strcmp(get_top(token_stack)->val.str, "function") == 0) {
            tree->tail_child =
                add_tree_node(tree, FUNC_DEC, "function_declaration");
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type != state_IDENTIFIER) return 2;
            tree->tail_child->tail_child =
                add_tree_node(tree->tail_child, NAME,
                              get_top(token_stack)->val.str);  // nazev funkce
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type == state_LEFTPARENT) {
                tree->tail_child->tail_child =
                    add_tree_node(tree->tail_child, PARAMETERS,
                                  "function_params");  // zapis argumentu funkce
                result = analyse_param(input_file, token_stack,
                                       tree->tail_child->tail_child);
                if (result != 0) return result;
            } else
                return 2;
            result = analyse_return_type(input_file, token_stack,
                                         tree->tail_child);  // stops on {
            if (result != 0) {
                return result;
            }
            tree->tail_child->tail_child =
                add_tree_node(tree->tail_child, BODY, "function_body");
            result = analyse_body(input_file, token_stack,
                                  tree->tail_child->tail_child);  // stops on }
            if (result != 0) {
                return result;
            }
            continue;
        }
        // function call
        if (get_top(token_stack)->type == state_IDENTIFIER) {
            tree->tail_child = add_tree_node(tree, FUNC_CALL, "function_call");
            tree->tail_child->tail_child = add_tree_node(
                tree->tail_child, NAME, get_top(token_stack)->val.str);
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type != state_LEFTPARENT) return 2;
            tree->tail_child->tail_child =
                add_tree_node(tree->tail_child, ARGUMENTS, "function_args");
            result = analyse_arg(input_file, token_stack,
                                 tree->tail_child->tail_child);  // stops on )
            if (result != 0) return result;
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type != state_SEMICOLON) return 2;
            continue;
        }

        // variable assign
        if (get_top(token_stack)->type == state_VARIABLE) {
            char variable_name[strlen(get_top(token_stack)->val.str) + 1];
            strcpy(variable_name, get_top(token_stack)->val.str);
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type != state_EQUAL) {
                struct tree_node *assign_node = add_tree_node(tree, EXPRESSION, "expression");
                result = analyse_expression(input_file, token_stack, 0, assign_node, 1);
                if (result != 0) return result;
                continue;
            }
            struct tree_node *assign_node = add_tree_node(tree, ASSIGN, "assign");
            add_tree_node(assign_node, NAME, variable_name);
            result = analyse_assign(input_file, token_stack,
                                    assign_node);  // stops on ;
            if (result != 0) return result;
            continue;
        }

        if (get_top(token_stack)->type == state_STRING || get_top(token_stack)->type == state_INT ||
            get_top(token_stack)->type == state_FLOAT || strcmp(get_top(token_stack)->val.str, "null") == 0 ||
            get_top(token_stack)->type == state_LEFTPARENT) {
            struct tree_node *assign_node = add_tree_node(tree, EXPRESSION, "expression");
            result = analyse_expression(input_file, token_stack, 0, assign_node, 0);
            if (result != 0) return result;
            continue;
        }

        // if
        if (strcmp(get_top(token_stack)->val.str, "if") == 0) {
            tree->tail_child = add_tree_node(tree, IF, "IF");
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type != state_LEFTPARENT) return 2;
            tree->tail_child->tail_child = add_tree_node(tree->tail_child, EXPRESSION, "expression");
            result = analyse_expression(input_file, token_stack, 1, tree->tail_child->tail_child, 0); //stops on {
            if (result != 0) return result;
            tree->tail_child->tail_child =
                add_tree_node(tree->tail_child, BODY, "IF_body");
            result = analyse_body(input_file, token_stack,
                                  tree->tail_child->tail_child);  // stops on }
            if (result != 0) return result;
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (strcmp(get_top(token_stack)->val.str, "else") != 0) return 2;
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type != state_CLEFTPARENT) return 2;
            tree->tail_child->tail_child =
                add_tree_node(tree->tail_child, BODY, "ELSE_body");
            result = analyse_body(input_file, token_stack,
                                  tree->tail_child->tail_child);  // stops on }
            if (result != 0) return result;
            continue;
        }
        // while
        if (strcmp(get_top(token_stack)->val.str, "while") == 0) {
            tree->tail_child = add_tree_node(tree, WHILE, "while");
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            if (get_top(token_stack)->type != state_LEFTPARENT) return 2;
            tree->tail_child->tail_child = add_tree_node(tree->tail_child, EXPRESSION, "expression");
            result = analyse_expression(input_file, token_stack, 1, tree->tail_child->tail_child, 0); // stops on {
            if (result != 0) return result;
            tree->tail_child->tail_child =
                add_tree_node(tree->tail_child, BODY, "WHILE_body");
            result = analyse_body(input_file, token_stack,
                                  tree->tail_child->tail_child);  // stops on }
            if (result != 0) return result;
            continue;
        }

        // return
        if (strcmp(get_top(token_stack)->val.str, "return") == 0) {
            if (get_token_rec(input_file, token_stack) != 0) return 1;
            tree->tail_child = add_tree_node(tree, RETURN, "main_return");
            if (get_top(token_stack)->type != state_SEMICOLON) {
                tree->tail_child->tail_child = add_tree_node(tree->tail_child, EXPRESSION, "expression");
                result = analyse_expression(input_file, token_stack,
                                            0, tree->tail_child->tail_child, 0); // stops on ;
                if (result != 0) return result;
            }
            continue;
        }

        //?>
        if (strcmp(get_top(token_stack)->val.str, "?>") == 0) {
            continue;
        }

        // EOF
        if (get_top(token_stack)->type == state_EOF) {
            return 0;
        }
        return 2;
    }
}

int analyse_prolog(FILE *input_file, Token_stack *token_stack,
                   struct tree_node *tree) {
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

    return analyse_prog(input_file, token_stack, tree);
}

// CALL WITH LEVEL = 0
void print_tree(struct tree_node *root, int level) {
    if (root == NULL) return;
    for (int i = 0; i < level; i++) printf(i == level - 1 ? "|" : "  ");
    printf("%s TYPE: %i\n", root->data->value, root->data->type);
    struct tree_node *child = root->head_child;
    while (child != NULL) {
        print_tree(child, level + 1);
        child = child->next_sibling;
    }
}

int analyse_syntax(FILE *input_file) {
    Token_stack token_stack;
    int stack_result = init_token_stack(&token_stack);
    if (stack_result == 1) return 99;

    // printf("STACK_RESULT: %i\n", stack_result);

    struct tn_data *body_data = init_tn_data(BODY, "main");
    struct tree_node *tree = init_tree_node();
    tree->data = body_data;
    int result = analyse_prolog(input_file, &token_stack, tree);

    DEBUG printf("AST\n");
    DEBUG printf("root: ");
    print_tree(tree, 0);
    printf("result: %i\n", result);
    if (result == 0) {
        result = semantic_analysis(tree);
        printf("AFTER SEMANTIC TREE\n");
        printf("root: ");
        print_tree(tree, 0);
    }
    printf("result: %i\n", result);
    if (result == 0) {
        // call generator
    }

    /*printf("%s\n", tree->data->value);
    printf("%s\n", tree->head_child->data->value);*/

    // printf("TOKENSSTACK:\n ");
    /*for (unsigned int i = 0; i < token_stack.free_index; i++) {
        printf("%s ", token_stack.tokens[i].val.str);
    }*/

    // printf("\n");

    free_tree_node(tree);
    free_tokens_stack(&token_stack);
    return result;
}
