// syntax_tree.h - rozhraní modulu pro syntax tree (IFJ PROJEKT)
// Licence: žádná (Public domain)
// Autor: Štěpán Nekula, Jan pavlíček

#ifndef __SYNTAX_TREE_H__
#define __SYNTAX_TREE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum tree_node_type {
    BODY,         // null
    ASSIGN,       // null
    FIRST_ASSIGN, // null
    IF,           // null
    WHILE,        // null
    ARGUMENTS,    // null
    TYPE,         // string
    NAME,         // string

    RETURN,    // null
    FUNC_DEC,  // null
    FUNC_CALL, // null
    VARIABLES, // null - for list of later used variable

    OPERATOR, // string

    EXPRESSION, // null
    // OPERATORS
    PLUS_OPERATOR,           // +
    MINUS_OPERATOR,          // -
    DIVISION_OPERATOR,       // /
    MULTIPLICATION_OPERATOR, // *
    CONCATENATION_OPERATOR,  // .
    EQUAL_OPERATOR,          // ===
    NOT_EQUAL_OPERATOR,      // !==
    SMALLER_OPERATOR,        // <
    BIGGER_OPERATOR,         // >
    SMALLER_EQUAL_OPERATOR,  // <=
    BIGGER_EQUAL_OPERATOR,   // >=

    // TERMINALS
    T_STRING,    // string
    T_INT,       // string
    T_FLOAT,     // string
    T_NULL,      // null/string
    VAR_OPERAND, // string

    // PARAMETERS
    PARAMETERS,      // null
    STR_PARAMETER,   // string
    NULL_PARAMETER,  // string
    INT_PARAMETER,   // string
    FLOAT_PARAMETER, // string

    STR_NULL_PARAMETER,
    INT_NULL_PARAMETER,
    FLOAT_NULL_PARAMETER,

    VARIABLES,
    T_VOID,
    T_NO_TYPE,

};

struct tree_node;
struct tn_data;

struct tree_node {
    size_t count;
    struct tree_node *next_sibling;
    struct tree_node *parent;
    struct tree_node *head_child;
    struct tree_node *tail_child;

    struct tn_data *data;
};

struct tn_data {
    enum tree_node_type type;
    char *value;
};

// TODO REWRITE TO FIX LEAKS
typedef struct {
    struct tree_node *nodes;
    unsigned int free_index;
    unsigned int capacity;
} Node_stack;

/**
 * @brief allocates and initiates tn_data structure
 * @param int type
 * @param const char(* value)
 * @return struct tn_data *
 */
struct tn_data *init_tn_data(int type, const char(*value));

/**
 * @brief allocates and initiates tree_node
 * @return struct tree_node *
 */
struct tree_node *init_tree_node();

/**
 * @brief sets attribut data for given tree node
 * @param struct tree_node * node
 * @param int type
 * @param const char(* value)
 * @return struct tn_data *
 */
struct tn_data *add_tn_data(struct tree_node *node, int type, const char(*value));

/**
 * @brief adds new child node to linked list of parent node and sets its value/data
 * @param struct tree_node * parent_node
 * @param int type
 * @param const char(* value)
 * @return struct tree_node *
 */
struct tree_node *add_tree_node(struct tree_node *parent_node, int type, const char(*value));

/**
 * @brief frees attribute data of tree node
 * @param struct tn_data *data
 */
void free_tree_data(struct tn_data *data);

/**
 * @brief frees all children of node
 * @param struct tree_node * node
 */
void free_children(struct tree_node *node);

/**
 * @brief frees node with all its children
 * @param struct tree_node * node
 * @note OTHER NODES MIGHT BE LINKED TO THIS NODE!
 */
void free_tree_node(struct tree_node *node);

int init_node_stack(Node_stack *stack);

struct tree_node *add_tree_node_object(struct tree_node *parent_node, struct tree_node *new_node);

struct tree_node *node_stack_top(Node_stack *stack);

int is_empty_node_stack(Node_stack *stack);

int node_stack_push(Node_stack *stack, struct tree_node *node);

int node_stack_pop(Node_stack *stack);

void free_node_stack(Node_stack *stack);

#endif // __SYNTAX_TREE_H__
