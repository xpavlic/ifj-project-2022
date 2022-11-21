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
    enum tree_node_type type = T_NULL;
    if (node->data->type == EXPRESSION) {
        type = traversal_type_propagation(node->head_child, t, fnc_symtable);
    } else if (node->data->type == OPERATOR) {
        type = check_type(
            traversal_type_propagation(node->head_child, t, fnc_symtable),
            traversal_type_propagation(node->tail_child, t, fnc_symtable),
            node->data->type);
    } else if (node->data->type == VAR_OPERAND) {
        type = ((value_t *)symtable_find(t, node->data->value)->value)
                   ->arg_var_type;
    } else if (node->data->type == FUNC_CALL) {
        /* Najdenie funkcie v TS funckii */
        htab_pair_t *fnc_rec = symtable_find(fnc_symtable, node->data->value);
        if (fnc_rec == NULL) error_handler(REDECLARATION_UNDECLARED);
        /* Kontrola typu argumentov*/
        check_arguments(node->tail_child, fnc_rec);
        /* Priradenie typu */
        type = ((value_t *)fnc_rec->value)->return_type;
    } else if (node->data->type == RETURN) {
        /* Kontrola, ci sedi konecny vyraz s deklarovanou navratovou
         * hodnotou */
        htab_pair_t *fnc_rec = symtable_find(fnc_symtable, node->data->value);
        if (fnc_rec == NULL) error_handler(REDECLARATION_UNDECLARED);
        if (node->head_child->data->type == EXPRESSION) {
            if ((int)(((value_t *)fnc_rec->value)->return_type) !=
                traversal_type_propagation(node->head_child, t, fnc_symtable)) {
                error_handler(WRONG_ARGUMENT_RETURN);
            }
        } else {
            if (((value_t *)fnc_rec->value)->return_type !=
                node->head_child->data->type)
                error_handler(WRONG_ARGUMENT_RETURN);
        }
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

/**
 * @brief Function info structure initializator
 *
 * @return value_t*
 */
value_t *value_init() {
    value_t *ptr = (value_t *)malloc(sizeof(value_t));
    if (ptr == NULL) {
        error_handler(INTERNAL_ERROR);
    }
    ptr->array = (enum tree_node_type *)malloc(sizeof(int));
    if (ptr->array == NULL) {
        error_handler(INTERNAL_ERROR);
    }
    ptr->number_of_parameters = 0;
    return ptr;
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
    pair->value = value_init();
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
        symtable_add_frame(symtable_stack);
        traversal_type_propagation(node->head_child, symtable_stack,
                                   fnc_symtable);
        traversal_second(fnc_symtable, symtable_stack, node->tail_child);
    } else if (node->data->type == BODY) {
        for (struct tree_node *tmp = node; tmp != NULL;
             tmp = tmp->next_sibling) {
            traversal_second(fnc_symtable, symtable_stack, tmp);
        }
    } else if (node->data->type == BODY) {
        htab_pair_t *new_pair =
            symtable_add(symtable_stack, node->tail_child->data->value);
        ((value_t *)new_pair->value)->arg_var_type = traversal_type_propagation(
            node->tail_child, symtable_stack, fnc_symtable);
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
    }
}
void add_builtin_functions(symtable_t *symtable_stack) {
    /* reads */
    htab_pair_t *reads = symtable_add(symtable_stack, "reads");
    if (reads == NULL) error_handler(INTERNAL_ERROR);
    reads->value = value_init();
    ((value_t *)reads->value)->number_of_parameters = 0;
    ((value_t *)reads->value)->return_type = STR_NULL_PARAMETER;
    /* readi */
    htab_pair_t *readi = symtable_add(symtable_stack, "readi");
    if (readi == NULL) error_handler(INTERNAL_ERROR);
    readi->value = value_init();
    ((value_t *)readi->value)->number_of_parameters = 0;
    ((value_t *)readi->value)->return_type = INT_NULL_PARAMETER;
    /* readf */
    htab_pair_t *readf = symtable_add(symtable_stack, "readf");
    if (readf == NULL) error_handler(INTERNAL_ERROR);
    readf->value = value_init();
    ((value_t *)readf->value)->number_of_parameters = 0;
    ((value_t *)readf->value)->return_type = FLOAT_NULL_PARAMETER;
    /* write */
    htab_pair_t *write = symtable_add(symtable_stack, "readf");
    if (write == NULL) error_handler(INTERNAL_ERROR);
    write->value = value_init();
    ((value_t *)write->value)->number_of_parameters = 0;
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
                        symtable_add(function_symbol_table, node->data->value);
                }
                if (tmp1->data->type == PARAMETERS) {
                    process_params(tmp1->head_child, new_pair);
                }
                if (tmp1->data->type == RETURN_TYPE) {
                    ((value_t *)new_pair->value)->return_type =
                        node->head_child->data->type;
                }
            }
        }
    }
    symtable_add_frame(symbol_table_stack);
    for (struct tree_node *tmp2 = node->head_child; tmp2 != NULL;
         tmp2 = tmp2->next_sibling) {
        traversal_second(function_symbol_table, symbol_table_stack, node);
    }
    symtable_free(function_symbol_table);
    symtable_free(symbol_table_stack);
    return 0;
}

/**
 * @brief Error handling function
 *
 */
void error_handler(int error) {
    switch (error) {
        case REDECLARATION_UNDECLARED:
            fprintf(stderr, "Redeclaration/undeclared function");
            exit(REDECLARATION_UNDECLARED);
        case WRONG_ARGUMENT_RETURN:
            fprintf(stderr, "Wrong argument type or return type");
            exit(REDECLARATION_UNDECLARED);
        case UNDEFINED_VAR:
            fprintf(stderr, "Undefined variable");
            exit(UNDEFINED_VAR);
        case TYPE_COMPATIBILITY_ERROR:
            fprintf(stderr, "Type compatibility error in expression");
            exit(REDECLARATION_UNDECLARED);
        case INTERNAL_ERROR:
            fprintf(stderr, "Internal error");
            exit(INTERNAL_ERROR);
        default:
            break;
    }
}