#include "semantic.h"

#define RETURN_CHECK(return_state, table) \
    if (return_state != 0) {              \
        symtable_free(fnc_symtable);      \
        symtable_free(table);             \
        return return_state;              \
    }

#define RETURN_TYPE_CHECK(return_type_state)                                                              \
    if (return_type_state == TYPE_COMPATIBILITY_ERROR || return_type_state == REDECLARATION_UNDECLARED) { \
        return return_type_state;                                                                         \
    }
#define TYPE(node) node->data->type

int semantic_analysis(struct tree_node *node) {
    enum tree_node_type return_state = 0;
    symtable_t *fnc_symtable = symtable_init(&value_t_free);
    if (fnc_symtable == NULL)
        return INTERNAL_ERROR;
    symtable_t *global_symtable = symtable_init(&value_t_free);
    if (global_symtable == NULL)
        return INTERNAL_ERROR;

    return_state = declarations_traversal(fnc_symtable, node);
    RETURN_CHECK(return_state, global_symtable)

    return_state = add_builtin_functions(fnc_symtable);
    RETURN_CHECK(return_state, global_symtable)

    /* Spracovanie tela programu */
    for (struct tree_node *tmp = node->head_child; tmp != NULL;
         tmp = tmp->next_sibling) {
        return_state = global_traversal(fnc_symtable, global_symtable, tmp);
        if (return_state != 0) break;
    }
    symtable_free(global_symtable);
    symtable_free(fnc_symtable);
    return return_state;
}

int declarations_traversal(symtable_t *fnc_symtable, struct tree_node *node) {
    for (struct tree_node *tmp = node->head_child; tmp != NULL; tmp = tmp->next_sibling) {
        if (tmp->data->type == FUNC_DEC) {
            /* Spracovanie nodes, ktore patria deklaracii funkcie */
            htab_pair_t *new_pair;
            for (struct tree_node *tmp1 = tmp->head_child; tmp1 != NULL;
                 tmp1 = tmp1->next_sibling) {
                if (tmp1->data->type == NAME) {
                    new_pair = symtable_find(fnc_symtable, tmp1->data->value);
                    if (new_pair == NULL) {
                        new_pair = symtable_add(fnc_symtable, tmp1->data->value);
                        if (new_pair == NULL) return INTERNAL_ERROR;
                    } else {
                        return REDECLARATION_UNDECLARED;
                    }
                }
                if (tmp1->data->type == PARAMETERS) {
                    if (process_params(tmp1->head_child, new_pair) != 0) return INTERNAL_ERROR;
                }
                if (tmp1->data->type == TYPE) {
                    ((value_t *)new_pair->value)->return_type =
                        eval_ret(tmp1->data->value);
                }
            }
        }
    }
    return 0;
}

int global_traversal(symtable_t *fnc_symtable, symtable_t *global_symtable, struct tree_node *node) {
    fprintf(stderr, "Spracovavana NODE: %s\n", node->data->value);
    enum tree_node_type return_state = 0;
    if (TYPE(node) == BODY) {
        symtable_add_frame(global_symtable);
        for (struct tree_node *tmp = node->head_child; tmp != NULL; tmp = tmp->next_sibling) {

            return_state = global_traversal(fnc_symtable, global_symtable, tmp);
            if (return_state != 0) return return_state;
        }
        symtable_remove_frame(global_symtable);
    } else if (TYPE(node) == ASSIGN) {
        htab_pair_t *pair = symtable_find(global_symtable, node->head_child->data->value);
        /*  Malo by byt FIRST ASSIGN, premenna este nie je v ramci */
        if (pair == NULL) {
            node->data = add_tn_data(node, FIRST_ASSIGN, "first_assign");
            return_state = global_traversal(fnc_symtable, global_symtable, node);
        } else {
            ((value_t *)pair->value)->arg_var_type = traversal_type_propagation(fnc_symtable, global_symtable, node->tail_child);
            if (((value_t *)pair->value)->arg_var_type == TYPE_COMPATIBILITY_ERROR) {
                return_state = TYPE_COMPATIBILITY_ERROR;
            } else if (((value_t *)pair->value)->arg_var_type == REDECLARATION_UNDECLARED) {
                return_state = REDECLARATION_UNDECLARED;
            } else if (((value_t *)pair->value)->arg_var_type == WRONG_ARGUMENT_RETURN) {
                return WRONG_ARGUMENT_RETURN;
            }
        }
    } else if (TYPE(node) == FIRST_ASSIGN) {
        htab_pair_t *pair = symtable_add(global_symtable, node->head_child->data->value);
        ((value_t *)pair->value)->arg_var_type = traversal_type_propagation(fnc_symtable, global_symtable, node->tail_child);
        if (((value_t *)pair->value)->arg_var_type == TYPE_COMPATIBILITY_ERROR) {
            return_state = TYPE_COMPATIBILITY_ERROR;
        } else if (((value_t *)pair->value)->arg_var_type == REDECLARATION_UNDECLARED) {
            return_state = REDECLARATION_UNDECLARED;
        } else if (((value_t *)pair->value)->arg_var_type == WRONG_ARGUMENT_RETURN) {
            return_state = WRONG_ARGUMENT_RETURN;
        }
    } else if (TYPE(node) == FUNC_DEC) {
        /* Spracovanie tela funkcie*/
        symtable_t *local_symtable = symtable_init(&value_t_free);
        if (local_symtable == NULL) return INTERNAL_ERROR;
        symtable_add_frame(local_symtable);
        return_state = add_params_into_frame(local_symtable, node->head_child->next_sibling);
        if (return_state != 0) {
            symtable_free(local_symtable);
            return return_state;
        }
        return_state = traversal_in_function(fnc_symtable, local_symtable, node->tail_child->head_child, node->head_child->data->value);
        symtable_free(local_symtable);
    } else if (TYPE(node) == IF) {
        for (struct tree_node *tmp = node->head_child; tmp != NULL; tmp = tmp->next_sibling) {
            return_state = global_traversal(fnc_symtable, global_symtable, tmp);
            if (return_state != 0) return return_state;
        }
    } else if (TYPE(node) == WHILE) {
        return_state = global_traversal(fnc_symtable, global_symtable, node->head_child);
        if (return_state != 0) return return_state;
    } else if (node->data->type == FUNC_CALL) {
        /* Najdenie funkcie v TS funckii */
        htab_pair_t *fnc_rec = symtable_find(fnc_symtable, node->head_child->data->value);
        if (fnc_rec == NULL) return REDECLARATION_UNDECLARED;
        /* Kontrola typu argumentov*/
        if (!strcmp(node->head_child->data->value, "write")) {
            return_state = check_write_arguments(node->tail_child, fnc_symtable, global_symtable);
        } else {
            return_state = check_arguments(fnc_symtable, global_symtable, node->tail_child, fnc_rec);
        }
    }
    return return_state;
}

int add_params_into_frame(symtable_t *local_symtable, struct tree_node *PARAMETERS) {
    for (struct tree_node *tmp = PARAMETERS->head_child; tmp != NULL; tmp = tmp->next_sibling) {
        htab_pair_t *new_pair = symtable_add(local_symtable, tmp->data->value);
        if (new_pair == NULL) return INTERNAL_ERROR;
        enum tree_node_type param_type = tmp->data->type;
        if (param_type == INT_NULL_PARAMETER || param_type == INT_PARAMETER) {
            ((value_t *)new_pair->value)->arg_var_type = T_INT;
        } else if (param_type == FLOAT_NULL_PARAMETER || param_type == FLOAT_PARAMETER) {
            ((value_t *)new_pair->value)->arg_var_type = T_FLOAT;
        } else if (param_type == STR_NULL_PARAMETER || param_type == STR_PARAMETER) {
            ((value_t *)new_pair->value)->arg_var_type = T_STRING;
        } else if (param_type == NULL_PARAMETER) {
            ((value_t *)new_pair->value)->arg_var_type = T_NULL;
        }
        fprintf(stderr, "Add param into frame. Name: %s Type: %d\n", new_pair->key, ((value_t *)new_pair->value)->arg_var_type);
    }
    return 0;
}

int traversal_type_propagation(symtable_t *fnc_symtable, symtable_t *t, struct tree_node *node) {
    enum tree_node_type type = TYPE_COMPATIBILITY_ERROR;
    if (TYPE(node) == EXPRESSION) {
        /* Spracovanie syna - nejaky operator */
        type = traversal_type_propagation(fnc_symtable, t, node->head_child);
    } else if (node->data->type == PLUS_OPERATOR ||
               node->data->type == MINUS_OPERATOR ||
               node->data->type == DIVISION_OPERATOR ||
               node->data->type == MULTIPLICATION_OPERATOR ||
               node->data->type == CONCATENATION_OPERATOR ||
               node->data->type == EQUAL_OPERATOR ||
               node->data->type == NOT_EQUAL_OPERATOR ||
               node->data->type == SMALLER_OPERATOR ||
               node->data->type == BIGGER_OPERATOR ||
               node->data->type == SMALLER_EQUAL_OPERATOR ||
               node->data->type == BIGGER_EQUAL_OPERATOR) {
        type = check_type(traversal_type_propagation(fnc_symtable, t, node->head_child), traversal_type_propagation(fnc_symtable, t, node->tail_child), node->data->type);
        RETURN_TYPE_CHECK(type)
    } else if (node->data->type == VAR_OPERAND) {
        type = ((value_t *)symtable_find(t, node->data->value)->value)->arg_var_type;
    } else if (node->data->type == FUNC_CALL) {
        /* Najdenie funkcie v TS funckii */
        htab_pair_t *fnc_rec = symtable_find(fnc_symtable, node->head_child->data->value);
        if (fnc_rec == NULL) return REDECLARATION_UNDECLARED;
        /* Kontrola typu argumentov*/
        enum tree_node_type return_state = check_arguments(fnc_symtable, t, node->tail_child, fnc_rec);
        if (return_state != 0) {
            return return_state;
        };
        /* Priradenie typu */
        type = (((value_t *)fnc_rec->value)->return_type);
        switch (type) {
        case INT_PARAMETER:
        case INT_NULL_PARAMETER:
        case T_INT:
            type = T_INT;
            break;
        case FLOAT_NULL_PARAMETER:
        case FLOAT_PARAMETER:
        case T_FLOAT:
            type = T_FLOAT;
            break;
        case STR_NULL_PARAMETER:
        case STR_PARAMETER:
        case T_STRING:
            type = T_STRING;
            break;
        default:
            type = WRONG_ARGUMENT_RETURN;
            break;
        }

    } else if (node->data->type == T_STRING || node->data->type == T_INT ||
               node->data->type == T_FLOAT ||
               node->data->type == STR_NULL_PARAMETER ||
               node->data->type == INT_NULL_PARAMETER ||
               node->data->type == FLOAT_NULL_PARAMETER) {
        type = node->data->type;
    }
    return type;
}

int traversal_in_function(symtable_t *fnc_symtable, symtable_t *local_symtable, struct tree_node *node, char *name) {
    fprintf(stderr, "Spracovavana NODE: %s - in func: %s\n", node->data->value, name);
    enum tree_node_type return_state = 0;
    if (TYPE(node) == BODY) {
        symtable_add_frame(local_symtable);
        for (struct tree_node *tmp = node->head_child; tmp != NULL; tmp = tmp->next_sibling) {

            return_state = traversal_in_function(fnc_symtable, local_symtable, tmp, name);
            if (return_state != 0) return return_state;
        }
        symtable_remove_frame(local_symtable);
    } else if (TYPE(node) == ASSIGN) {
        htab_pair_t *pair = symtable_find(local_symtable, node->head_child->data->value);
        /*  Malo by byt FIRST ASSIGN, premenna este nie je v ramci*/
        if (pair == NULL) {
            node->data = add_tn_data(node, FIRST_ASSIGN, "first_assign");
            return traversal_in_function(fnc_symtable, local_symtable, node, name);
        } else {
            ((value_t *)pair->value)->arg_var_type = traversal_type_propagation(fnc_symtable, local_symtable, node->tail_child);
            if (((value_t *)pair->value)->arg_var_type == TYPE_COMPATIBILITY_ERROR) {
                return TYPE_COMPATIBILITY_ERROR;
            } else if (((value_t *)pair->value)->arg_var_type == REDECLARATION_UNDECLARED) {
                return REDECLARATION_UNDECLARED;
            } else if (((value_t *)pair->value)->arg_var_type == WRONG_ARGUMENT_RETURN) {
                return WRONG_ARGUMENT_RETURN;
            }
        }
    } else if (TYPE(node) == FIRST_ASSIGN) {
        htab_pair_t *pair = symtable_add(local_symtable, node->head_child->data->value);
        ((value_t *)pair->value)->arg_var_type = traversal_type_propagation(fnc_symtable, local_symtable, node->tail_child);
        if (((value_t *)pair->value)->arg_var_type == TYPE_COMPATIBILITY_ERROR) {
            return TYPE_COMPATIBILITY_ERROR;
        } else if (((value_t *)pair->value)->arg_var_type == REDECLARATION_UNDECLARED) {
            return REDECLARATION_UNDECLARED;
        } else if (((value_t *)pair->value)->arg_var_type == WRONG_ARGUMENT_RETURN) {
            return WRONG_ARGUMENT_RETURN;
        }
    } else if (TYPE(node) == IF) {
        for (struct tree_node *tmp = node->head_child; tmp != NULL; tmp = tmp->next_sibling) {
            return_state = traversal_in_function(fnc_symtable, local_symtable, tmp, name);
            if (return_state != 0) return return_state;
        }
    } else if (TYPE(node) == WHILE) {
        return_state = traversal_in_function(fnc_symtable, local_symtable, node, name);
        if (return_state != 0) return return_state;
    } else if (TYPE(node) == RETURN) {
        return_state = check_return_type(fnc_symtable, local_symtable, node->head_child, name);
        if (return_state != 0) return WRONG_ARGUMENT_RETURN;
    }
    return return_state;
}

/* ----------------------- POMOCNE ------------------------------*/

int add_builtin_functions(symtable_t *fnc_symtable) {
    /* reads */
    htab_pair_t *reads = symtable_find(fnc_symtable, "reads");
    if (reads == NULL) {
        reads = symtable_add(fnc_symtable, "reads");
        if (reads == NULL) return INTERNAL_ERROR;
    } else {
        return REDECLARATION_UNDECLARED;
    }
    ((value_t *)reads->value)->number_of_parameters = 0;
    ((value_t *)reads->value)->return_type = STR_NULL_PARAMETER;
    /* readi */
    htab_pair_t *readi = symtable_find(fnc_symtable, "readi");
    if (readi == NULL) {
        readi = symtable_add(fnc_symtable, "readi");
        if (readi == NULL) return INTERNAL_ERROR;
    } else {
        return REDECLARATION_UNDECLARED;
    }
    ((value_t *)readi->value)->number_of_parameters = 0;
    ((value_t *)readi->value)->return_type = INT_NULL_PARAMETER;
    /* readf */
    htab_pair_t *readf = symtable_find(fnc_symtable, "readf");
    if (readf == NULL) {
        readf = symtable_add(fnc_symtable, "readf");
        if (readf == NULL) return INTERNAL_ERROR;
    } else {
        return REDECLARATION_UNDECLARED;
    }
    ((value_t *)readf->value)->number_of_parameters = 0;
    ((value_t *)readf->value)->return_type = FLOAT_NULL_PARAMETER;
    /* write */
    htab_pair_t *write = symtable_find(fnc_symtable, "write");
    if (write == NULL) {
        write = symtable_add(fnc_symtable, "write");
        if (write == NULL) return INTERNAL_ERROR;
    } else {
        return REDECLARATION_UNDECLARED;
    }

    return 0;
}

int process_params(struct tree_node *node, htab_pair_t *pair) {
    value_t *content = ((value_t *)pair->value);
    if (content == NULL) return INTERNAL_ERROR;
    for (struct tree_node *tmp = node; tmp != NULL; tmp = tmp->next_sibling) {
        /* spracovanie parametru a vytvorenie zaznamu*/
        content->array[content->number_of_parameters++] = node->data->type;
        content->array = (enum tree_node_type *)realloc(
            content->array, sizeof(int) * (content->number_of_parameters + 1));
        if (content->array == NULL) return INTERNAL_ERROR;
    }
    return 0;
}

int eval_ret(char *value) {
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

void value_t_free(void *toBeDeleted) {
    free(((value_t *)toBeDeleted)->array);
    free((value_t *)toBeDeleted);
}

int check_write_arguments(struct tree_node *node, symtable_t *fnc_symtable, symtable_t *t) {
    enum tree_node_type type;
    for (struct tree_node *tmp = node->head_child; tmp != NULL;
         tmp = tmp->next_sibling) {
        type = traversal_type_propagation(fnc_symtable, t, tmp);
        if (type != T_FLOAT && type != T_INT && type != T_STRING && type != T_NULL) {
            return WRONG_ARGUMENT_RETURN;
        }
    }
    return 0;
}

int check_arguments(symtable_t *fnc_symtable, symtable_t *local_symtable, struct tree_node *node, htab_pair_t *fnc_rec) {
    int processed_argument_i = 0;
    enum tree_node_type type;
    enum tree_node_type expected;
    value_t *content = ((value_t *)fnc_rec->value);
    for (struct tree_node *tmp = node->head_child; tmp != NULL;
         tmp = tmp->next_sibling) {
        if (processed_argument_i >= content->number_of_parameters) {
            return WRONG_ARGUMENT_RETURN;
        }
        type = traversal_type_propagation(fnc_symtable, local_symtable, tmp);
        fprintf(stderr, "NODE: %s GOT ARG TYPE: %d\n", tmp->data->value, type);
        if (type == TYPE_COMPATIBILITY_ERROR) {
            return TYPE_COMPATIBILITY_ERROR;
        }
        expected = (int)content->array[processed_argument_i++];
        fprintf(stderr, "NODE: %s EXPECTED ARG TYPE: %d\n", tmp->data->value, expected);
        if (expected == INT_PARAMETER && type != T_INT) {
            return WRONG_ARGUMENT_RETURN;
        } else if (expected == INT_NULL_PARAMETER && (type != T_INT && type != T_NULL)) {
            return WRONG_ARGUMENT_RETURN;
        } else if (expected == FLOAT_PARAMETER && type != T_FLOAT) {
            return WRONG_ARGUMENT_RETURN;
        } else if (expected == FLOAT_NULL_PARAMETER && (type != T_FLOAT && type != T_NULL)) {
            return WRONG_ARGUMENT_RETURN;
        } else if (expected == STR_PARAMETER && type != T_STRING) {
            return WRONG_ARGUMENT_RETURN;
        } else if (expected == STR_NULL_PARAMETER && (type != T_STRING && type != T_NULL)) {
            return WRONG_ARGUMENT_RETURN;
        }
    }
    return 0;
}

int check_type(int type_left_child, int type_right_child, int operator) {
    fprintf(stderr, "CHECKING OPERATION: LEFT_OP: %d OPERATOR: %d  RIGHT_OP:%d\n", type_left_child, operator, type_right_child);
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

            ;
        }
        if (type_left_child == T_FLOAT) {
            if (type_right_child == T_INT || type_right_child == T_FLOAT ||
                type_right_child == T_NULL)
                return T_FLOAT;
        }
        return TYPE_COMPATIBILITY_ERROR;
        break;
    case DIVISION_OPERATOR:
        if ((type_left_child == T_INT || type_left_child == T_FLOAT ||
             type_left_child == T_NULL) &&
            (type_right_child == T_INT || type_right_child == T_FLOAT ||
             type_right_child == T_NULL))
            return T_FLOAT;
        return TYPE_COMPATIBILITY_ERROR;
        break;
    case CONCATENATION_OPERATOR:
        if ((type_left_child == T_STRING || type_left_child == T_NULL) &&
            (type_right_child == T_STRING || type_right_child == T_NULL))
            return T_STRING;
        if (type_left_child == STR_NULL_PARAMETER ||
            type_right_child == STR_NULL_PARAMETER)
            return STR_NULL_PARAMETER;
        return TYPE_COMPATIBILITY_ERROR;
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
        return TYPE_COMPATIBILITY_ERROR;
        break;
    default:
        break;
    }
    return TYPE_COMPATIBILITY_ERROR;
}

int check_return_type(symtable_t *fnc_symtable, symtable_t *t, struct tree_node *node, char *name) {
    htab_pair_t *fnc_rec = symtable_find(fnc_symtable, name);
    enum tree_node_type expected = ((value_t *)fnc_rec->value)->return_type;
    enum tree_node_type return_exp = traversal_type_propagation(fnc_symtable, t, node->head_child);
    if (expected == return_exp) {
        return 0;
    } else {
        if (expected == INT_NULL_PARAMETER &&
            (return_exp == T_INT || return_exp == T_NULL))
            return 0;
        if (expected == FLOAT_NULL_PARAMETER &&
            (return_exp == T_FLOAT || return_exp == T_NULL))
            return 0;
        if (expected == STR_NULL_PARAMETER &&
            (return_exp == T_STRING || return_exp == T_NULL))
            return 0;
    }
    return WRONG_ARGUMENT_RETURN;
}