/**
 * @project Compiler IFJ22
 * @file    semantic.c
 * @authors  Matej Vadovič <xvadov01>
 */
#include "semantic.h"

#define VALUE(node) ((value_t *)node->value)

#define FREE_AND_RETURN          \
    symtable_free(symtable);     \
    symtable_free(fnc_symtable); \
    return result_state;

static builtin_fnc_def_t builtin_functions[] =
    {
        {"reads", {NULL}, "?string"},
        {"readi", {NULL}, "?int"},
        {"readf", {NULL}, "?float"},
        {"write", {NULL}, "void"},
        {"strlen", {"string"}, "int"},
        {"substring", {"string", "int", "int"}, "?string"},
        {"ord", {"string"}, "int"},
        {"chr", {"int"}, "string"},
        {"floatval", {NULL}, "float"},
        {"intval", {NULL}, "int"},
        {"strval", {NULL}, "string"},
        {NULL}};

enum error_state semantic_analysis(struct tree_node *node) {
    enum error_state result_state = OK;
    if (node == NULL) return INTERNAL_ERROR;

    symtable_t *fnc_symtable = symtable_init(&value_t_free);
    if (fnc_symtable != NULL) {
        if (symtable_add_frame(fnc_symtable)) {
            symtable_free(fnc_symtable);
            return INTERNAL_ERROR;
        }
    } else {
        return INTERNAL_ERROR;
    }

    symtable_t *symtable = symtable_init(&value_t_free);
    if (symtable != NULL) {
        if (symtable_add_frame(symtable)) {
            symtable_free(symtable);
            symtable_free(fnc_symtable);
            return INTERNAL_ERROR;
        }
    } else {
        symtable_free(fnc_symtable);
        return INTERNAL_ERROR;
    }

    result_state = declarations_traversal(fnc_symtable, node);
    if (result_state != OK) {
        FREE_AND_RETURN
    }

    result_state = add_builtin_functions(fnc_symtable);
    if (result_state != OK) {
        FREE_AND_RETURN
    }
    #define NDEBUG if(0)
    NDEBUG print_table(fnc_symtable);
    /* Spracovanie tela programu */
    result_state = traverse_body(fnc_symtable, symtable, node, "main");

    if (result_state == OK) {
        result_state = add_variables_node(symtable, node);
    }

    FREE_AND_RETURN
}

/* ------------------------------------------------------------- */
/*                      Auxiliary Functions                      */
/* ------------------------------------------------------------- */

enum error_state add_variables_node(symtable_t *symtable, struct tree_node *node) {
    enum error_state result_state = OK;
    struct tree_node *new_node = add_tree_node(node, VARIABLES, "list_of_variables");
    if (new_node == NULL) return INTERNAL_ERROR;
    htab_item_t *start;
    for (size_t i = 0; i < symtable->head->hash_table->arr_size && result_state == OK; i++) {
        start = symtable->head->hash_table->arr_ptr[i];
        for (htab_item_t *j = start; j != NULL && result_state == OK; j = j->next) {
            struct tree_node *new_var_node = add_tree_node(new_node, VAR_OPERAND, j->data.key);
            if (new_var_node == NULL) result_state = INTERNAL_ERROR;
        }
    }
    return result_state;
}

void value_t_free(void *toBeDeleted) {
    if (((value_t *)toBeDeleted)->type == value_fnc) {
        free(((value_t *)toBeDeleted)->val.fnc.parameters);
    }
    free((value_t *)toBeDeleted);
}

enum error_state add_builtin_functions(symtable_t *fnc_symtable) {
    enum error_state result_state = OK;
    for (builtin_fnc_def_t *item = builtin_functions; item->name != NULL; item++) {
        htab_pair_t *new = symtable_find(fnc_symtable, item->name);
        if (new == NULL) {
            new = symtable_add(fnc_symtable, item->name, &value_create_fnc);
            if (new == NULL) return INTERNAL_ERROR;
        } else {
            return REDECLARATION_UNDECLARED;
        }
        for (char **type = item->params_array; *type != NULL; type++) {
            result_state = add_parameter(VALUE(new), *type);
        }
        ((value_t *)new->value)->val.fnc.return_type = item->return_type;
    }
    return result_state;
}

enum error_state add_parameter(value_t *content, char *type) {
    value_fnc_t *fnc = &content->val.fnc;
    fnc->parameters = (char **)realloc(
        fnc->parameters, sizeof(char *) * (++fnc->number_of_parameters));
    if (fnc->parameters == NULL) return INTERNAL_ERROR;
    fnc->parameters[fnc->number_of_parameters - 1] = type;
    return OK;
}

enum error_state declarations_traversal(symtable_t *fnc_symtable, struct tree_node *node) {
    enum error_state result_state = OK;
    for (struct tree_node *tmp = node->head_child; tmp != NULL; tmp = tmp->next_sibling) {
        if (tmp->data->type == FUNC_DEC) {
            htab_pair_t *new_fnc = symtable_find(fnc_symtable, tmp->head_child->data->value);
            if (new_fnc != NULL) {
                return REDECLARATION_UNDECLARED;
            }
            new_fnc = symtable_add(fnc_symtable, tmp->head_child->data->value, &value_create_fnc);
            if (new_fnc == NULL) return INTERNAL_ERROR;
            for (struct tree_node *tmp1 = tmp->head_child->next_sibling->head_child; tmp1 != NULL; tmp1 = tmp1->next_sibling) {
                result_state = add_parameter(new_fnc->value, translate_type(tmp1->data->type));
                if (result_state != OK) return result_state;
            }
            VALUE(new_fnc)->val.fnc.return_type = tmp->head_child->next_sibling->next_sibling->data->value;
        }
    }
    return result_state;
}

void print_table(symtable_t *symtable) {
    struct htab *hash_table = symtable->head->hash_table;
    htab_item_t **tmp = hash_table->arr_ptr;
    for (size_t i = 0; i < hash_table->arr_size; i++) {
        printf("-%ld- ", i);
        for (htab_item_t *tmp1 = tmp[i]; tmp1 != NULL; tmp1 = tmp1->next) {
            if (VALUE((&(tmp1->data)))->type == value_fnc) {
                printf(" %s[type: %d]{", tmp1->data.key, VALUE((&(tmp1->data)))->type);
                for (size_t i = 0; i < VALUE((&(tmp1->data)))->val.fnc.number_of_parameters; i++) {
                    printf("%s ", VALUE((&(tmp1->data)))->val.fnc.parameters[i]);
                }
                printf("}{RETURN: %s}", VALUE((&(tmp1->data)))->val.fnc.return_type);

            } else {
                printf(" %s[type: %d]", tmp1->data.key, VALUE((&(tmp1->data)))->type);
            }
        }
        printf("\n");
    }
}

enum error_state add_params_into_frame(symtable_t *symtable, struct tree_node *parameters_node) {
    for (struct tree_node *tmp = parameters_node->head_child; tmp != NULL; tmp = tmp->next_sibling) {
        if (symtable_find(symtable, tmp->data->value) == NULL) {
            htab_pair_t *new_pair = symtable_add(symtable, tmp->data->value, &value_create_var);
            if (new_pair == NULL) return INTERNAL_ERROR;
        } else {
            return OTHER_SEM_ERROR;
        }
    }
    return OK;
}

char *translate_type(enum tree_node_type type) {
    char *ret_val;
    switch (type) {
    case T_INT:
        ret_val = "int";
        break;
    case T_STRING:
        ret_val = "string";
        break;
    case T_FLOAT:
        ret_val = "float";
        break;
    case INT_PARAMETER:
        ret_val = "int";
        break;
    case STR_PARAMETER:
        ret_val = "string";
        break;
    case FLOAT_PARAMETER:
        ret_val = "float";
        break;
    case STR_NULL_PARAMETER:
        ret_val = "?string";
        break;
    case INT_NULL_PARAMETER:
        ret_val = "?int";
        break;
    case FLOAT_NULL_PARAMETER:
        ret_val = "?float";
        break;
    case T_NULL:
        ret_val = "null";
        break;
    default:
        ret_val = "";
        break;
    }
    return ret_val;
}

enum error_state check_arguments(symtable_t *symtable, struct tree_node *node, htab_pair_t *fnc_rec) {
    enum error_state result_state = OK;

    size_t number_of_args = 0;
    for (struct tree_node *tmp = node->head_child; tmp != NULL; tmp = tmp->next_sibling) {
        number_of_args++;
        if (tmp->data->type == VAR_OPERAND) {
            if (symtable_find(symtable, tmp->data->value) == NULL) {
                return UNDEFINED_VAR;
            }
        }
    }
    if ((!strcmp(fnc_rec->key, "intval") || !strcmp(fnc_rec->key, "strval") || !strcmp(fnc_rec->key, "floatval")) && number_of_args == 1) {
        return OK;
    }
    if (!strcmp(fnc_rec->key, "write")) {
        return OK;
    }
    if (number_of_args != VALUE(fnc_rec)->val.fnc.number_of_parameters) {
        return WRONG_ARGUMENT_RETURN_TYPE_COUNT;
    }
    return result_state;
}

/* ------------------------------------------------------------- */
/*                      Traversals functions                     */
/* ------------------------------------------------------------- */

enum error_state traverse_body(symtable_t *fnc_symtable, symtable_t *symtable, struct tree_node *body_node, char *fnc_name) {
    enum error_state result_state = OK;
    if (body_node == NULL) return INTERNAL_ERROR;
    for (struct tree_node *tmp = body_node->head_child; tmp != NULL; tmp = tmp->next_sibling) {
        switch (tmp->data->type) {
        case ASSIGN:
            result_state = traverse_assign(fnc_symtable, symtable, tmp);
            break;
        case IF:
            result_state = traverse_if(fnc_symtable, symtable, tmp, fnc_name);
            break;
        case WHILE:
            result_state = traverse_while(fnc_symtable, symtable, tmp, fnc_name);
            break;
        case FUNC_DEC:
            result_state = traverse_fnc_dec(fnc_symtable, symtable, tmp);
            break;
        case FUNC_CALL:
            result_state = traverse_func_call(fnc_symtable, symtable, tmp);
            break;
        case EXPRESSION:
            result_state = traverse_expression(symtable, tmp);
            break;
        case RETURN:
            result_state = traverse_return(fnc_symtable, tmp, fnc_name);
            break;
        default:
            break;
        }
        if (result_state != OK) break;
    }
    return result_state;
}

enum error_state traverse_fnc_dec(symtable_t *fnc_symtable, symtable_t *symtable, struct tree_node *node) {
    enum error_state result_state = OK;
    if (symtable_add_frame(symtable)) {
        return INTERNAL_ERROR;
    }
    if (symtable->head == NULL) return INTERNAL_ERROR;
    for (struct tree_node *tmp = node->head_child; tmp != NULL; tmp = tmp->next_sibling) {
        switch (tmp->data->type) {
        case BODY:
            result_state = traverse_body(fnc_symtable, symtable, tmp, node->head_child->data->value);
            break;
        case PARAMETERS:
            result_state = add_params_into_frame(symtable, tmp);
            break;
        default:
            break;
        }
        if (result_state != OK) break;
    }
    if (result_state == OK) {
        result_state = add_variables_node(symtable, node->tail_child);
    }

    symtable_remove_frame(symtable);
    return result_state;
}

enum error_state traverse_assign(symtable_t *fnc_symtable, symtable_t *symtable, struct tree_node *node) {
    enum error_state result_state = OK;
    htab_pair_t *var = symtable_find(symtable, node->head_child->data->value);
    /*  Malo by byt FIRST ASSIGN, premenna este nie je v ramci*/
    if (var == NULL) {
        var = symtable_add(symtable, node->head_child->data->value, &value_create_var);
        if (var == NULL) {
            return INTERNAL_ERROR;
        }
    }
    switch (node->tail_child->data->type) {
    case EXPRESSION:
        result_state = traverse_expression(symtable, node->tail_child);
        break;
    case FUNC_CALL:
        result_state = traverse_func_call(fnc_symtable, symtable, node->tail_child);
        break;
    default:
        break;
    }
    return result_state;
}

enum error_state traverse_expression(symtable_t *symtable, struct tree_node *node) {
    enum error_state result_state = OK;
    switch (node->head_child->data->type) {
    case VAR_OPERAND:
        if (symtable_find(symtable, node->head_child->data->value) == NULL) {
            result_state = UNDEFINED_VAR;
        }
        break;
    case T_STRING:
    case T_INT:
    case T_FLOAT:
    case T_NULL:
        result_state = OK;
        break;
    case EQUAL_OPERATOR:
    case NOT_EQUAL_OPERATOR:
    case SMALLER_OPERATOR:
    case BIGGER_OPERATOR:
    case SMALLER_EQUAL_OPERATOR:
    case BIGGER_EQUAL_OPERATOR:
    case PLUS_OPERATOR:
    case MINUS_OPERATOR:
    case MULTIPLICATION_OPERATOR:
    case DIVISION_OPERATOR:
    case CONCATENATION_OPERATOR:
        result_state = traverse_operator(symtable, node->head_child);
        break;
    default:
        break;
    }
    return result_state;
}

enum error_state traverse_func_call(symtable_t *fnc_symtable, symtable_t *symtable, struct tree_node *node) {
    enum error_state result_state = OK;
    htab_pair_t *fnc_rec = symtable_find(fnc_symtable, node->head_child->data->value);
    if (fnc_rec == NULL) return REDECLARATION_UNDECLARED;

    result_state = check_arguments(symtable, node->tail_child, fnc_rec);
    if (result_state != OK) {
        return result_state;
    }
    if (add_tree_node(node, TYPE, VALUE(fnc_rec)->val.fnc.return_type) == NULL) return INTERNAL_ERROR;
    return result_state;
}

enum error_state traverse_operator(symtable_t *symtable, struct tree_node *node) {
    enum error_state result_state = OK;
    enum error_state left_operand;
    enum error_state right_operand;
    left_operand = traverse_operand(symtable, node->head_child);
    right_operand = traverse_operand(symtable, node->tail_child);

    if (left_operand != OK) {
        result_state = left_operand;
    } else if (right_operand != OK) {
        result_state = right_operand;
    }

    return result_state;
}

enum error_state traverse_operand(symtable_t *symtable, struct tree_node *node) {
    enum error_state result_state = OK;
    switch (node->data->type) {
    case VAR_OPERAND:
        if (symtable_find(symtable, node->data->value) == NULL) {
            result_state = UNDEFINED_VAR;
        }
        break;
    case T_STRING:
    case T_INT:
    case T_FLOAT:
    case T_NULL:
        result_state = OK;
        break;
    case EQUAL_OPERATOR:
    case NOT_EQUAL_OPERATOR:
    case SMALLER_OPERATOR:
    case BIGGER_OPERATOR:
    case SMALLER_EQUAL_OPERATOR:
    case BIGGER_EQUAL_OPERATOR:
        result_state = TYPE_COMPATIBILITY_ERROR;
        break;
    case PLUS_OPERATOR:
    case MINUS_OPERATOR:
    case MULTIPLICATION_OPERATOR:
    case DIVISION_OPERATOR:
    case CONCATENATION_OPERATOR:
        result_state = traverse_operator(symtable, node);
        break;
    default:
        break;
    }
    return result_state;
}

enum error_state traverse_if(symtable_t *fnc_symtable, symtable_t *symtable, struct tree_node *node, char *fnc_name) {
    enum error_state result_state = OK;
    for (struct tree_node *tmp = node->head_child; tmp != NULL; tmp = tmp->next_sibling) {
        switch (tmp->data->type) {
        case BODY:
            result_state = traverse_body(fnc_symtable, symtable, tmp, fnc_name);
            break;
        case EXPRESSION:
            result_state = traverse_expression(symtable, tmp);
            break;
        default:
            break;
        }
        if (result_state != OK) return result_state;
    }
    return result_state;
}

enum error_state traverse_while(symtable_t *fnc_symtable, symtable_t *symtable, struct tree_node *node, char *fnc_name) {
    enum error_state result_state = OK;
    for (struct tree_node *tmp = node->head_child; tmp != NULL; tmp = tmp->next_sibling) {
        switch (tmp->data->type) {
        case BODY:
            result_state = traverse_body(fnc_symtable, symtable, tmp, fnc_name);
            break;
        case EXPRESSION:
            result_state = traverse_expression(symtable, tmp);
            break;
        default:
            break;
        }
        if (result_state != OK) return result_state;
    }
    return result_state;
}

enum error_state traverse_return(symtable_t *fnc_symtable, struct tree_node *node, char *fnc_name) {
    size_t count = 0;
    for (struct tree_node *tmp = node->head_child; tmp != NULL; tmp = tmp->next_sibling) {
        count++;
    }
    if (!strcmp(fnc_name, "main")) {
        if (count > 1) return WRONG_RETURN_EXPR_COUNT;
        return OK;
    }

    htab_pair_t *fnc_rec = symtable_find(fnc_symtable, fnc_name);
    if (fnc_rec == NULL) {
        return REDECLARATION_UNDECLARED;
    }
    if (count > 1) {
        return WRONG_RETURN_EXPR_COUNT;
    } else if (count > 0 && !strcmp(VALUE(fnc_rec)->val.fnc.return_type, "void")) {
        return WRONG_RETURN_EXPR_COUNT;
    } else if (count == 0 && strcmp(VALUE(fnc_rec)->val.fnc.return_type, "void") != 0) {
        return WRONG_RETURN_EXPR_COUNT;
    }
    return OK;
}
