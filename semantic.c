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
enum token_types {
    UNDEF,     // 0
    INTEGER,   // 1
    FLOAT,     // 2
    STRING,    // 4
    NIL,       // 5
    FUNCTION,  // 6
};
enum tree_node_type {
    BODY,
    FUNC_DEC,
    FUNC_CALL,
    ASSIGN,
    FIRST_ASSIGN,
    IF,
    WHILE,
    ARGUMENTS,
    ARGUMENT,
    EXPRESSION,
    TYPE,
    NAME,
    OPERATOR,
    OPERAND,
    VAR_OPERAND,
    PARAMS,
    PARAM,
    RETURN,
};
/*sémantická/běhová chyba typové kompatibility v aritmetických, řetězcových a
 * relačních výrazech*/
#define TYPE_COMPATIBILITY_ERROR -1
#define UNDEFINED_VAR 5
int traversal_type_propagation(struct tree_node *node, symtable_t *t) {
    enum token_types type = NIL;
    switch (node->data->node_type) {
        case EXPRESSION:
            type = traversal_type_propagation(node->head_child, t);
            break;
        case OPERATOR:
            type = check_type(traversal_type_propagation(node->head_child, t),
                              traversal_type_propagation(node->tail_child, t),
                              node->data->operator);
            break;
        case OPERAND:
            return symtable_find(t, node->data->value)->content->data_type;
    }
    return type;
}
/**
 * @brief Funckia pre prvy priechod AST
 *
 * @param node ukazaten na koren stromu
 * @param t ukazatel na tabulku symbolov
 */
void traversal_first(struct tree_node *node, symtable_t *t) {
    switch (node->data->node_type) {
        case BODY:
            for (struct tree_node *tmp = node->head_child; tmp != NULL;
                 tmp = tmp->next_sibling) {
                traversal_first(tmp, t);
            }
        case FUNC_DEC:
            traversal_first(node->head_child, t); /* Ulozenie mena do TS */
            symtable_add_frame(t);                /* Novy frame v TS */
            for (struct tree_node *tmp = node->head_child->next_sibling;
                 tmp->data->node_type != BODY; tmp = tmp->next_sibling) {
                traversal_first(tmp, t); /* PARAMS */
            }
            traversal_first(node->tail_child, t->head); /* BODY */
            break;
        /* Prejdem paremetre funkcie */
        case PARAMS:
            htab_pair_t *symtab_rec = symtable_find(t, node->data->value);
            for (struct tree_node *tmp = node->head_child; tmp != NULL;
                 tmp = tmp->next_sibling) {
                symtab_rec->content->number_of_args++;
                traversal_first(tmp, t);
            }
        /* Kazdy jeden parameter funkcie vlozim do jej tabulky symbolov*/
        case PARAM:
            htab_pair_t *new_pair = symtable_add(t, node->data->value);
            new_pair->content->data_type =
                node->data->token_type; /* tu by mal byt typ zo SA v strome*/
            break;
        /* Vytvorenie noveho prvku htab_pair_t v tabulke symbolov */
        case NAME:
            htab_pair_t *new_pair = symtable_add(t, node->data->value);
            new_pair->content->declared = true;
            break;
            /* Tvorba novej polozky v TS a priradenie typu */

        default:
    }
}
void traversal_second(struct tree_node *node, symtable_t *t) {
    enum token_types type = NIL;
    switch (node->data->node_type) {
        case FIRST_ASSIGN:
            traversal(node->head_child, t);
            htab_pair_t *tmp = symtable_find(t, node->data->value);
            tmp->content->data_type =
                traversal_type_propagation(node->tail_child, t);
            break;
        case ASSIGN:
            htab_pair_t *tmp = symtable_find(t, node->data->value);
            /* priradenie do nedefinovanej premennej*/
            if (tmp == NULL) {
                undefined_var_error();
            }
            tmp->content->data_type =
                traversal_type_propagation(node->tail_child, t);
            break;
        default:
    }
}

int check_type(int type_left_child, int type_right_child, int operator) {
    switch (operator) {
        case NONE:
            /* vyriesti case */
            break;
        case ARITHMETIC_OP:
            if (type_left_child == INTEGER) {
                if (type_right_child == INTEGER)
                    return INTEGER;
                else if (type_right_child == FLOAT)
                    return FLOAT;
                return TYPE_COMPATIBILITY_ERROR;
            }
            if (type_left_child == FLOAT) {
                if (type_right_child == INTEGER || type_right_child == FLOAT)
                    return FLOAT;
                return TYPE_COMPATIBILITY_ERROR;
            }
            return TYPE_COMPATIBILITY_ERROR;
        case DIVISION_OP:
            if ((type_left_child == INTEGER || type_left_child == FLOAT) &&
                (type_right_child == INTEGER || type_right_child == FLOAT))
                return FLOAT;
            return TYPE_COMPATIBILITY_ERROR;
        case STRING_OP:
            if (type_left_child == STRING && type_right_child == STRING)
                return STRING;
            return TYPE_COMPATIBILITY_ERROR;
        case EQUALITY_OP:
            if (type_left_child == type_right_child) return type_left_child;
            return TYPE_COMPATIBILITY_ERROR; /* V tomto pripade to nie je
                                                chyba, len vysledok operacie
                                                bude false
                                              */
        case RELATIONAL_OP:
            if ((type_left_child == INTEGER || type_left_child == FLOAT ||
                 type_left_child == STRING) &&
                (type_right_child == INTEGER || type_right_child == FLOAT ||
                 type_right_child == STRING))
                return FLOAT;
            return TYPE_COMPATIBILITY_ERROR;
        default:
            break;
    }
}

// void type_error() {
//     fprintf(stderr, "Type conflict between %d and %d using op type %d\n", );
//     exit(1);
// }
void undefined_var_error() {
    fprintf(stderr, "Undefined variable");
    exit(5);
}