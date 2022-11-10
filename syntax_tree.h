// syntax_tree.h - rozhraní modulu pro syntax tree (IFJ PROJEKT)
// Licence: žádná (Public domain)
// Autor: Štěpán Nekula


#ifndef __SYNTAX_TREE_H__
#define __SYNTAX_TREE_H__
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

enum tree_node_type{
    BODY, // null
    ASSIGN, // null
    FIRST_ASSIGN, // null
    IF, // null
    WHILE, // null
    ARGUMENTS, // null
    TYPE, // string
    NAME, // string

    RETURN, // null
    FUNC_DEC, // null
    FUNC_CALL, // null

    OPERATOR, // string

    //TERMINALS
    T_STRING, //string
    T_INT, // string
    T_FLOAT, // string
    T_NULL, // null/string
    VAR_OPERAND, // string

    //PARAMETERS
    PARAMETERS, // null
    STR_PARAMETER,// string 
    NULL_PARAMETER,// string
    INT_PARAMETER,//string 
    FLOAT_PARAMETER,// string

    ARGUMENT, //TO DELETE, instead use 'terminal'/VAR_OPERAND
    EXPRESSION, // TO DELETE, instead use OPERATOR/terminal/VAR_OPERAND

};



struct tree_node;
struct tn_data;


struct tree_node{
    size_t count;
    struct tree_node * next_sibling;
    struct tree_node * parent;
    struct tree_node * head_child;
    struct tree_node * tail_child;
    
    struct tn_data * data;

};

struct tn_data{
    enum tree_node_type type;
    char*value;
};


/**
 * @brief allocates and initiates tn_data structure
 * @param int type
 * @param const char(* value)
 * @return struct tn_data *
*/
struct tn_data * init_tn_data(int type, const char(* value));

/**
 * @brief allocates and initiates tree_node
 * @return struct tree_node *
*/
struct tree_node * init_tree_node();


/**
 * @brief sets attribut data for given tree node
 * @param struct tree_node * node
 * @param int type
 * @param const char(* value)
 * @return struct tn_data *
*/
struct tn_data * add_tn_data(struct tree_node * node,int type, const char(* value));

/**
 * @brief adds new child node to linked list of parent node and sets its value/data
 * @param struct tree_node * parent_node
 * @param int type
 * @param const char(* value)
 * @return struct tree_node *
*/
struct tree_node * add_tree_node(struct tree_node *parent_node,int type, const char(* value));

/**
 * @brief frees attribute data of tree node
 * @param struct tn_data *data
*/
void free_tree_data(struct tn_data *data);

/**
 * @brief frees all children of node
 * @param struct tree_node * node
*/
void free_children(struct tree_node * node);

/**
 * @brief frees node with all its children
 * @param struct tree_node * node
 * @note OTHER NODES MIGHT BE LINKED TO THIS NODE!
*/
void free_tree_node(struct tree_node * node);
 

#endif // __SYNTAX_TREE_H__
