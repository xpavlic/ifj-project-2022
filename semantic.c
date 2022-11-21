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
    switch (node->data->type) {
        case EXPRESSION:
            type =
                traversal_type_propagation(node->head_child, t, fnc_symtable);
            break;
        case OPERATOR:
            type = check_type(
                traversal_type_propagation(node->head_child, t, fnc_symtable),
                traversal_type_propagation(node->tail_child, t, fnc_symtable),
                node->data->type);
            break;
        case VAR_OPERAND:
            type = symtable_find(t, node->data->value)
                       ->((value_t *)value)
                       ->data_type;
            break;
        case FUNC_CALL:
            /* Najdenie funkcie v TS funckii */
            htab_pair_t *fnc_rec =
                symtable_find(fnc_symtable, node->data->value);
            if (fnc_rec == NULL) error_handler(REDECLARATION_UNDECLARED);
            /* Kontrola typu argumentov*/
            check_arguments(node->tail_child, fnc_rec);
            /* Priradenie typu */
            type = fnc_rec->((value_t *)value)->return_type;
            break;
        case RETURN:
            /* Kontrola, ci sedi konecny vyraz s deklarovanou navratovou
             * hodnotou */
            if (node->head_child == EXPRESSION) {
                if (fnc_rec->((value_t *)value)->return_type !=
                    traversal_type_propagation(node->head_child, t,
                                               fnc_symtable)) {
                    error_handler(WRONG_ARGUMENT_RETURN);
                }
            } else {
                if (fnc_rec->((value_t *)value)->return_type !=
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
        case NOT_EQUAL_OPERATOR:
            if (type_left_child == type_right_child) return type_left_child;
            error_handler(TYPE_COMPATIBILITY_ERROR);
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
        default:
            break;
    }
}

void check_arguments(struct tree_node *node, htab_pair_t *fnc_rec) {
    int processed_argument_i = 0;
    value_t *content = fnc_rec->((value_t *)value);
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
    ptr = (enum tree_node_type *)malloc(sizeof(int));
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
void value_t_free(value_t *toBeDeleted) {
    free(toBeDeleted->array);
    free(toBeDeleted);
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
    pair->((value_t *)value) = func_i_init();
    value_t *content = pair->((value_t *)value);
    if (content == NULL) error_handler(INTERNAL_ERROR);
    for (struct tree_node *tmp = node; tmp->data->type != NULL;
         tmp = tmp->next_sibling) {
        /* spracovanie parametru a vytvorenie zaznamu*/
        content->array[content->number_of_parameters++] = node->data->type;
        content->array = (enum tree_node_type *)realloc(
            sizeof(int), content->number_of_parameters + 1);
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
    switch (node->data->type) {
        case FUNC_DEC:
            /* Vytvornie noveho ramcu */
            symtable_add_frame(symtable_stack);
            traversal_second(fnc_symtable, symtable_stack, node->tail_child);
        case IF:
            symtable_add_frame(symtable_stack);
        case WHILE:
            symtable_add_frame(symtable_stack);
        case BODY:
            for (struct tree_node *tmp = node; tmp != NULL; tmp->next_sibling) {
                traversal_second(fnc_symtable, symtable_stack, tmp);
            }
        case FIRST_ASSIGN:
            htab_pair_t *new_pair =
                symtable_add(symtable_stack, node->tail_child->data->value);
            new_pair->((value_t *)value)->arg_var_type =
                traversal_type_propagation(node->tail_child, symtable_stack,
                                           fnc_symtable);
        case ASSIGN:
            htab_pair_t *pair =
                symtable_find(symtable_stack, node->head_child->data->value);
            /*  Malo by byt FIRST ASSIGN, premenna este nie je v ramci*/
            if (pair == NULL) {
                node->data->type = FIRST_ASSIGN;
                traversal_second(fnc_symtable, symtable_stack, node);
            }
            /* Pridaneie aktualneho typu podla pravej strany */
            pair->((value_t *)value)->arg_var_type = traversal_type_propagation(
                node->tail_child, symtable_stack, fnc_symtable);
        default:
    }
}
void add_builtin_functions(symtable_t *symtable_stack) {
    /* reads */
    htab_pair_t *reads = symtable_add(symtable_stack, "reads");
    reads->((value_t *)value) = func_i_init();
    reads->((value_t *)value)->number_of_parameters = 0;
    reads->((value_t *)value)->return_type = STR_NULL_PARAMETER;
    /* readi */
    htab_pair_t *readi = symtable_add(symtable_stack, "readi");
    readi->((value_t *)value) = func_i_init();
    readi->((value_t *)value)->number_of_parameters = 0;
    readi->((value_t *)value)->return_type = INT_NULL_PARAMETER;
    /* readf */
    htab_pair_t *readf = symtable_add(symtable_stack, "readf");
    readf->((value_t *)value) = func_i_init();
    readf->((value_t *)value)->number_of_parameters = 0;
    readf->((value_t *)value)->return_type = FLOAT_NULL_PARAMETER;
    /* write */
    htab_pair_t *readf = symtable_add(symtable_stack, "readf");
    readf->((value_t *)value) = func_i_init();
    readf->((value_t *)value)->number_of_parameters = 0;
}

/**
 * @brief Semantic analysis
 *
 * @param node BODY node of a program
 */
void semantic_analysis(struct tree_node *node) {
    symtable_t *function_symbol_table = symtable_init(&value_t_free);
    if (function_symbol_table == NULL) error_handler(INTERNAL_ERROR);
    symtable_add_frame(function_symbol_table);
    /* spracovanie deklaracii funckii v BODY node  */
    for (struct tree_node *tmp = node->head_child; tmp != NULL;
         tmp = tmp->next_sibling) {
        if (tmp->data->type == FUNC_DEC) {
            /* Spracovanie nodes, ktore patria deklaracii funkcie */
            htab_pair_t *new_pair;
            for (struct tree_node *tmp1 = tmp->head_child; tmp1 != NULL;
                 tmp1 = tmp1->next_sibling)
                switch (tmp1->data->type) {
                    case NAME:
                        new_pair = symtable_add(function_symbol_table,
                                                node->data->value);
                        break;
                    case PARAMETERS:
                        process_params(tmp1->head_child, new_pair);
                        break;
                    case RETURN_TYPE:
                        new_pair->((value_t *)value)->return_type =
                            node->head_child->data->type;
                        break;
                    default:
                }
        }
    }
    symtable_t *symbol_table_stack = symtable_init(&value_t_free);
    if (function_symbol_table == NULL) error_handler(INTERNAL_ERROR);
    symtable_add_frame(symbol_table_stack);
    for (struct tree_node *tmp2 = node->head_child; tmp2 != NULL;
         tmp2 = tmp2->next_sibling) {
        traversal_second(function_symbol_table, symbol_table_stack, node);
    }
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
        case UNDEFINED_VAR:
            fprintf(stderr, "Undefined variable");
            exit(UNDEFINED_VAR);
        case INTERNAL_ERROR:
            fprintf(stderr, "Internal error");
            exit(INTERNAL_ERROR);
        default:
            break;
    }
}
