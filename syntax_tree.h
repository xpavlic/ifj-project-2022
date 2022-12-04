/**
 * @project Compiler IFJ22
 * @file    syntax_tree.h
 * @authors Stepan Nekula <xnekul04>
 */


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

//TODO:
// TODO REWRITE TO FIX LEAKS
typedef struct {
    struct tree_node *nodes;
    unsigned int free_index;
    unsigned int capacity;
} Node_stack;

/**
 * @brief allocates and initiates tn_data structure, that is used for tree_node
 * @param type: attribute type of to be initialized tn_data structure
 * @param value: attribute value of to be initialized tn_data structure
 * @return pointer to initialized struct tn_data
*/
struct tn_data * init_tn_data(enum tree_node_type type, const char(* value));

/**
 * @brief allocates and initiates tree_node
 * @return pointer to allocated tree node
*/
struct tree_node * init_tree_node();


/**
 * @brief sets attribut data for given tree node
 * @param node: pointer to the node that will be added attribute data
 * @param type: attribute type of to be initialized tn_data structure
 * @param value: attribute value of to be initialized tn_data structure
 * @return pointer to added struct tn_data
*/
struct tn_data * add_tn_data(struct tree_node * node,enum tree_node_type type, const char(* value));

/**
 * @brief adds new child node to back of linked list of parent node and sets its data
 * @param parent_node: node of the added child node
 * @param type: type of the new child node
 * @param value: value of the new child node
 * @return pointer to the new child node
*/
struct tree_node * add_tree_node(struct tree_node *parent_node,enum tree_node_type type, const char(* value));

/**
 * @brief frees attribute data of tree node
 * @param data: pointer to struct tn_data that will be freed
*/
void free_tree_data(struct tn_data *data);

/**
 * @brief frees all children of node
 * @param node: node whose all child nodes will be freed
*/
void free_children(struct tree_node * node);

/**
 * @brief frees node with all its children
 * @param node: to be freed node
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
