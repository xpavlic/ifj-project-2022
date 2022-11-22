/**
 * @file semantic.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-11-04
 *
 * @copyright
 *
 */
#include "semantic.h"

int traversal_type_propagation(struct tree_node *node, symtable_t *t,
                               symtable_t *fnc_symtable) {
    int type = 0;
    if (node->data->type == EXPRESSION) {
        return traversal_type_propagation(node->head_child, t, fnc_symtable);
    }
    if (node->data->type == OPERATOR) {
        return check_type(
            traversal_type_propagation(node->head_child, t, fnc_symtable),
            traversal_type_propagation(node->tail_child, t, fnc_symtable),
            node->data->type);
    }
    if (node->data->type == VAR_OPERAND) {
        int a = ((value_t *)symtable_find(t, node->data->value)->value)
                    ->arg_var_type;
        fprintf(stderr, "%d", a);
        return (a);
    }
    if (node->data->type == FUNC_CALL) {
        fprintf(stderr, "vyhodnocujem pravu stranu funkciu");
        /* Najdenie funkcie v TS funckii */
        htab_pair_t *fnc_rec =
            symtable_find(fnc_symtable, node->head_child->data->value);
        if (fnc_rec == NULL) error_handler(REDECLARATION_UNDECLARED);
        /* Kontrola typu argumentov*/
        check_arguments(node->tail_child, fnc_rec);
        /* Priradenie typu */
        return (((value_t *)fnc_rec->value)->return_type);
    }
    return type;
}

int check_type(int type_left_child, int type_right_child, int operator) {
    switch (operator) {
        case PLUS_OPERATOR:
        case MINUS_OPERATOR:
        case MULTIPLICATION_OPERATOR:
            if (type_left_child == T_INT) {
                if (type_right_child == T_INT)
                    return T_INT;
                else if (type_right_child == T_NULL)
                    return T_INT;
                else if (type_right_child == T_FLOAT)
                    return T_FLOAT;

                error_handler(TYPE_COMPATIBILITY_ERROR);
            }
            if (type_left_child == T_FLOAT) {
                if (type_right_child == T_INT || type_right_child == T_FLOAT ||
                    type_right_child == T_NULL)
                    return T_FLOAT;
            }
            error_handler(TYPE_COMPATIBILITY_ERROR);
            break;
        case DIVISION_OPERATOR:
            if ((type_left_child == T_INT || type_left_child == T_FLOAT ||
                 type_left_child == T_NULL) &&
                (type_right_child == T_INT || type_right_child == T_FLOAT))
                return T_FLOAT;
            error_handler(TYPE_COMPATIBILITY_ERROR);
            break;
        case CONCATENATION_OPERATOR:
            if ((type_left_child == T_STRING || type_left_child == T_NULL) &&
                (type_right_child == T_STRING || type_right_child == T_NULL))
                return T_STRING;
            error_handler(TYPE_COMPATIBILITY_ERROR);
            break;
        case EQUAL_OPERATOR:
            break;
        case NOT_EQUAL_OPERATOR:
            break;
        case SMALLER_EQUAL_OPERATOR:
        case BIGGER_EQUAL_OPERATOR:
        case SMALLER_OPERATOR:
        case BIGGER_OPERATOR:
            if ((type_left_child == T_INT || type_left_child == T_FLOAT ||
                 type_left_child == T_STRING) &&
                (type_right_child == T_INT || type_right_child == T_FLOAT ||
                 type_right_child == T_STRING))
                return T_FLOAT;
            error_handler(TYPE_COMPATIBILITY_ERROR);
            break;
        default:
            break;
    }
    return TYPE_COMPATIBILITY_ERROR;
}

void check_arguments(struct tree_node *node, htab_pair_t *fnc_rec) {
    int processed_argument_i = 0;
    value_t *content = ((value_t *)fnc_rec->value);
    for (struct tree_node *tmp = node->head_child; tmp != NULL;
         tmp = tmp->next_sibling) {
        if (processed_argument_i >= content->number_of_parameters) {
            error_handler(WRONG_ARGUMENT_RETURN);
        }
        if (content->array[processed_argument_i++] != node->data->type) {
            error_handler(WRONG_ARGUMENT_RETURN);
        }
    }
}

void check_write_arguments(struct tree_node *node, symtable_t *fnc_symtable,
                           symtable_t *t) {
    for (struct tree_node *tmp = node->head_child; tmp != NULL;
         tmp = tmp->next_sibling) {
        fprintf(stderr, "node typ:%d, typ - %d, symbol - %s\n", tmp->data->type,
                (traversal_type_propagation(tmp, t, fnc_symtable)),
                tmp->data->value);
        if ((traversal_type_propagation(tmp, t, fnc_symtable) != T_STRING) &&
            (traversal_type_propagation(tmp, t, fnc_symtable) !=
             STR_NULL_PARAMETER)) {
            error_handler(WRONG_ARGUMENT_RETURN);
        }
    }
}

/**
 * @brief Value_t destructor
 *
 * @param toBeDeleted
 */
void value_t_free(void *toBeDeleted) {
    free(((value_t *)toBeDeleted)->array);
    free((value_t *)toBeDeleted);
}

/**
 * @brief Function to process parameters of a function and store types into
 * array in value_t structure
 *
 * @param node - pointer to a PARAMETERS NODE
 * @param pair - pointer to a pair in a hash table
 * @return void
 */
void process_params(struct tree_node *node, htab_pair_t *pair) {
    value_t *content = ((value_t *)pair->value);
    if (content == NULL) error_handler(INTERNAL_ERROR);
    for (struct tree_node *tmp = node; tmp != NULL; tmp = tmp->next_sibling) {
        /* spracovanie parametru a vytvorenie zaznamu*/
        content->array[content->number_of_parameters++] = node->data->type;
        content->array = (enum tree_node_type *)realloc(
            content->array, sizeof(int) * (content->number_of_parameters + 1));
        if (content->array == NULL) {
            error_handler(INTERNAL_ERROR);
        }
    }
    return;
}

/**
 * @brief
 *
 * @param fnc_symtable - pointer to a FUNCTION ST stack
 * @param symtable_stack - pointer to a PROGRAM ST stack
 * @param node - pointer to a node to be checked
 */
void traversal_second(symtable_t *fnc_symtable, symtable_t *symtable_stack,
                      struct tree_node *node) {
    if (node->data->type == FUNC_DEC) {
        /* Vytvornie noveho ramcu */
        symtable_add_frame(symtable_stack);
        traversal_second(fnc_symtable, symtable_stack, node->tail_child);
    } else if (node->data->type == IF) {
        traversal_type_propagation(node->head_child, symtable_stack,
                                   fnc_symtable);
        for (struct tree_node *tmp = node->head_child->next_sibling;
             tmp != NULL; tmp = tmp->next_sibling) {
            traversal_second(fnc_symtable, symtable_stack, node->tail_child);
        }
    } else if (node->data->type == WHILE) {
        traversal_type_propagation(node->head_child, symtable_stack,
                                   fnc_symtable);
        traversal_second(fnc_symtable, symtable_stack, node->tail_child);
    } else if (node->data->type == BODY) {
        symtable_add_frame(symtable_stack);
        for (struct tree_node *tmp = node->head_child; tmp != NULL;
             tmp = tmp->next_sibling) {
            traversal_second(fnc_symtable, symtable_stack, node->tail_child);
        }
    } else if (node->data->type == ASSIGN) {
        htab_pair_t *pair =
            symtable_find(symtable_stack, node->head_child->data->value);
        /*  Malo by byt FIRST ASSIGN, premenna este nie je v ramci*/
        if (pair == NULL) {
            node->data->type = FIRST_ASSIGN;
            node->data->value = "first_assign";
            traversal_second(fnc_symtable, symtable_stack, node);
        } else {
            /* Pridaneie aktualneho typu podla pravej strany */
            ((value_t *)pair->value)->arg_var_type = traversal_type_propagation(
                node->tail_child, symtable_stack, fnc_symtable);
        }
    } else if (node->data->type == FIRST_ASSIGN) {
        htab_pair_t *pair =
            symtable_add(symtable_stack, node->head_child->data->value);
        if (pair == NULL) {
            error_handler(INTERNAL_ERROR);
        }
        /* Pridaneie aktualneho typu podla pravej strany */
        ((value_t *)pair->value)->arg_var_type = traversal_type_propagation(
            node->tail_child, symtable_stack, fnc_symtable);
        fprintf(stderr, " typ po vyhodnoteni pravej strany %d\n",
                ((value_t *)pair->value)->arg_var_type);
    } else if (node->data->type == FUNC_CALL) {
        /* Najdenie funkcie v TS funckii */
        htab_pair_t *fnc_rec =
            symtable_find(fnc_symtable, node->head_child->data->value);
        if (fnc_rec == NULL) {
            error_handler(REDECLARATION_UNDECLARED);
        }
        if (!strcmp(node->head_child->data->value, "write")) {
            check_write_arguments(node->tail_child, fnc_symtable,
                                  symtable_stack);
        } else {
            check_arguments(node->tail_child, fnc_rec);
        }

    } else if (node->data->type == RETURN) {
        symtable_remove_frame(symtable_stack);
    }
}
void add_builtin_functions(symtable_t *fnc_symtable) {
    /* reads */
    htab_pair_t *reads = symtable_add(fnc_symtable, "reads");
    if (reads == NULL) error_handler(INTERNAL_ERROR);
    ((value_t *)reads->value)->number_of_parameters = 0;
    ((value_t *)reads->value)->return_type = STR_NULL_PARAMETER;

    /* readi */
    htab_pair_t *readi = symtable_add(fnc_symtable, "readi");
    if (readi == NULL) error_handler(INTERNAL_ERROR);
    ((value_t *)readi->value)->number_of_parameters = 0;
    ((value_t *)readi->value)->return_type = INT_NULL_PARAMETER;
    /* readf */
    htab_pair_t *readf = symtable_add(fnc_symtable, "readf");
    if (readf == NULL) error_handler(INTERNAL_ERROR);
    ((value_t *)readf->value)->number_of_parameters = 0;
    ((value_t *)readf->value)->return_type = FLOAT_NULL_PARAMETER;
    /* write */
    htab_pair_t *write = symtable_add(fnc_symtable, "write");
    if (write == NULL) error_handler(INTERNAL_ERROR);
    fprintf(stderr, "%s\n", write->key);
}

/**
 * @brief Semantic analysis
 *
 * @param node BODY node of a program
 */
int semantic_analysis(struct tree_node *node) {
    symtable_t *function_symbol_table = symtable_init(&value_t_free);
    if (function_symbol_table == NULL) error_handler(INTERNAL_ERROR);
    symtable_t *symbol_table_stack = symtable_init(&value_t_free);
    if (symbol_table_stack == NULL) error_handler(INTERNAL_ERROR);
    symtable_add_frame(function_symbol_table);
    add_builtin_functions(function_symbol_table);
    /* spracovanie deklaracii funckii v BODY node  */
    for (struct tree_node *tmp = node->head_child; tmp != NULL;
         tmp = tmp->next_sibling) {
        if (tmp->data->type == FUNC_DEC) {
            /* Spracovanie nodes, ktore patria deklaracii funkcie */
            htab_pair_t *new_pair;
            for (struct tree_node *tmp1 = tmp->head_child; tmp1 != NULL;
                 tmp1 = tmp1->next_sibling) {
                if (tmp1->data->type == NAME) {
                    new_pair =
                        symtable_add(function_symbol_table, tmp1->data->value);
                }
                if (tmp1->data->type == PARAMETERS) {
                    process_params(tmp1->head_child, new_pair);
                }
                if (tmp1->data->type == TYPE) {
                    ((value_t *)new_pair->value)->return_type =
                        eval_ret(tmp1->data->value);
                }
            }
        }
    }
    symtable_add_frame(symbol_table_stack);
    for (struct tree_node *tmp2 = node->head_child; tmp2 != NULL;
         tmp2 = tmp2->next_sibling) {
        traversal_second(function_symbol_table, symbol_table_stack, tmp2);
    }
    symtable_free(function_symbol_table);
    symtable_free(symbol_table_stack);
    return 0;
}

enum tree_node_type eval_ret(char *value) {
    if (!strcmp(value, "string"))
        return T_STRING;
    else if (!strcmp(value, "int"))
        return T_INT;
    else if (!strcmp(value, "float"))
        return T_FLOAT;
    else if (!strcmp(value, "?string"))
        return STR_NULL_PARAMETER;
    else if (!strcmp(value, "?int"))
        return INT_NULL_PARAMETER;
    else
        return FLOAT_NULL_PARAMETER;
}

/**
 * @brief Error handling function
 *
 */
void error_handler(int error) {
    switch (error) {
        case REDECLARATION_UNDECLARED:
            fprintf(stderr, "Redeclaration/undeclared function\n");
            exit(REDECLARATION_UNDECLARED);
        case WRONG_ARGUMENT_RETURN:
            fprintf(stderr, "Wrong argument type or return type\n");
            exit(REDECLARATION_UNDECLARED);
        case UNDEFINED_VAR:
            fprintf(stderr, "Undefined variable\n");
            exit(UNDEFINED_VAR);
        case TYPE_COMPATIBILITY_ERROR:
            fprintf(stderr, "Type compatibility error in expression\n");
            exit(REDECLARATION_UNDECLARED);
        case INTERNAL_ERROR:
            fprintf(stderr, "Internal error\n");
            exit(INTERNAL_ERROR);
        default:
            break;
    }
}