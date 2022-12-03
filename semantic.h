/**
 * @project Compiler IFJ22
 * @file    semantic.h
 * @authors  Matej Vadovič <xvadov01>
 */

#pragma once
#include "symtable.h"
#include "syntax_tree.h"

enum error_state {
    OK = 0,
    REDECLARATION_UNDECLARED = 3,
    WRONG_ARGUMENT_RETURN_TYPE_COUNT = 4,
    UNDEFINED_VAR = 5,
    WRONG_RETURN_EXPR_COUNT = 6,
    TYPE_COMPATIBILITY_ERROR = 7,
    INTERNAL_ERROR = 99
};

/**
 * @brief Structure to hold information about builtin function
 *
 */
typedef struct builtin_fnc_def {
    char *name;
    char *params_array[5];
    char *return_type;
} builtin_fnc_def_t;

/**
 * @brief The main function of semantic analysis
 *
 * @param node
 * @return enum error_state
 */
enum error_state semantic_analysis(struct tree_node *node);

/* ------------------------------------------------------------- */
/*                      Auxiliary Functions                      */
/* ------------------------------------------------------------- */

/**
 * @brief Creates and fills VARIABLES node in AST
 *
 * @param symtable
 * @param node
 * @return enum error_state
 */
enum error_state add_variables_node(symtable_t *symtable, struct tree_node *node);

/**
 * @brief value_t destructor
 *
 * @param toBeDeleted
 */
void value_t_free(void *toBeDeleted);

/**
 * @brief Create builtin function entries in a symbol table
 *
 * @param fnc_symtable
 * @return enum error_state
 */
enum error_state add_builtin_functions(symtable_t *fnc_symtable);

/**
 * @brief Adds parameter entry into a value_t structure
 *
 * @param content
 * @param type
 * @return enum error_state
 */
enum error_state add_parameter(value_t *content, char *type);

/**
 * @brief Scan program and creates function declaration entries in symbol table
 *
 * @param fnc_symtable
 * @param node
 * @return enum error_state
 */
enum error_state declarations_traversal(symtable_t *fnc_symtable, struct tree_node *node);

/**
 * @brief Prints symbol table entries
 *
 * @param symtable
 */
void print_table(symtable_t *symtable);

/**
 * @brief Adds parameters as variables into a function frame
 *
 * @param local_symtable
 * @param parameters_node
 * @return enum error_state
 */
enum error_state add_params_into_frame(symtable_t *local_symtable, struct tree_node *parameters_node);

/**
 * @brief Translates type of enum tree_node_type into char*
 *
 * @param type
 * @return char*
 */
char *translate_type(enum tree_node_type type);

/**
 * @brief Function arguments check
 *
 * @param symtable
 * @param node ARGUMENTS NODE
 * @param fnc_rec
 * @return enum error_state
 */
enum error_state check_arguments(symtable_t *symtable, struct tree_node *node, htab_pair_t *fnc_rec);

/* ------------------------------------------------------------- */
/*                      Traversals functions                     */
/* ------------------------------------------------------------- */

/**
 * @brief Traverses BODY node
 *
 * @param fnc_symtable
 * @param symtable
 * @param body_node
 * @param fnc_name
 * @return enum error_state
 */
enum error_state traverse_body(symtable_t *fnc_symtable, symtable_t *symtable, struct tree_node *body_node, char *fnc_name);

/**
 * @brief Traverses FNC_DEC node
 *
 * @param fnc_symtable
 * @param symtable
 * @param node
 * @return enum error_state
 */
enum error_state traverse_fnc_dec(symtable_t *fnc_symtable, symtable_t *symtable, struct tree_node *node);

/**
 * @brief Traverses ASSIGN node
 *
 * @param fnc_symtable
 * @param symtable
 * @param node
 * @return enum error_state
 */
enum error_state traverse_assign(symtable_t *fnc_symtable, symtable_t *symtable, struct tree_node *node);

/**
 * @brief Traverses EXPRESSION node
 *
 * @param symtable
 * @param node
 * @return enum error_state
 */
enum error_state traverse_expression(symtable_t *symtable, struct tree_node *node);

/**
 * @brief Traverses FUNC_CALL node
 *
 * @param fnc_symtable
 * @param symtable
 * @param node
 * @return enum error_state
 */
enum error_state traverse_func_call(symtable_t *fnc_symtable, symtable_t *symtable, struct tree_node *node);

/**
 * @brief Traverses OPERATOR node
 *
 * @param symtable
 * @param node
 * @return enum error_state
 */
enum error_state traverse_operator(symtable_t *symtable, struct tree_node *node);

/**
 * @brief Traverses OPERAND node
 *
 * @param symtable
 * @param node
 * @return enum error_state
 */
enum error_state traverse_operand(symtable_t *symtable, struct tree_node *node);

/**
 * @brief Traverses IF node
 *
 * @param fnc_symtable
 * @param symtable
 * @param node
 * @param fnc_name
 * @return enum error_state
 */
enum error_state traverse_if(symtable_t *fnc_symtable, symtable_t *symtable, struct tree_node *node, char *fnc_name);

/**
 * @brief Traverses WHILE node
 *
 * @param fnc_symtable
 * @param symtable
 * @param node
 * @param fnc_name
 * @return enum error_state
 */
enum error_state traverse_while(symtable_t *fnc_symtable, symtable_t *symtable, struct tree_node *node, char *fnc_name);

/**
 * @brief Traverse RETURN node
 *
 * @param fnc_symtable
 * @param node
 * @param fnc_name
 * @return enum error_state
 */
enum error_state traverse_return(symtable_t *fnc_symtable, struct tree_node *node, char *fnc_name);
