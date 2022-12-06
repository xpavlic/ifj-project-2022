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
    OTHER_SEM_ERROR = 8,
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
 * @param node main program BODY syntax tree node 
 * @return OK if sucess, else error enum
 */
enum error_state semantic_analysis(struct tree_node *node);

/* ------------------------------------------------------------- */
/*                      Auxiliary Functions                      */
/* ------------------------------------------------------------- */

/**
 * @brief Creates and fills VARIABLES node in AST
 *
 * @param symtable symbol table stack where function variables are stored
 * @param node parent syntax tree node
 * @return OK if success, else error enum
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
 * @param fnc_symtable symbol table stack where function entries will be added
 * @return OK if success, else error enum
 */
enum error_state add_builtin_functions(symtable_t *fnc_symtable);

/**
 * @brief Adds parameter entry into a value_t structure
 *
 * @param content value_t structure where type will be stored
 * @param type variable type
 * @return OK if success, else error enum
 */
enum error_state add_parameter(value_t *content, char *type);

/**
 * @brief Scan program and creates function declaration entries in symbol table
 *
 * @param fnc_symtable symbol table stack where function entries will be
 * @param node main program BODY syntax tree node
 * @return OK if success, else error enum
 */
enum error_state declarations_traversal(symtable_t *fnc_symtable, struct tree_node *node);

/**
 * @brief Prints symbol table entries
 *
 * @param symtable symbol table stack, where top hashtable will be printed
 */
void print_table(symtable_t *symtable);

/**
 * @brief Adds parameters as variables into a function frame
 *
 * @param symtable symbol table stack
 * @param parameters_node PARAMETERS syntax tree node
 * @return OK if success, else error enum
 */
enum error_state add_params_into_frame(symtable_t *symtable, struct tree_node *parameters_node);

/**
 * @brief Translates type of enum tree_node_type into string
 *
 * @param type syntax tree node representing data type
 * @return string of coresponding data type
 */
char *translate_type(enum tree_node_type type);

/**
 * @brief Function arguments check
 *
 * @param symtable symbol table stack with a top frame where variables will be searched
 * @param node VARIABLES of a function call syntax tree node
 * @param fnc_rec hashtable record of the called function
 * @return OK if success, else error enum
 */
enum error_state check_arguments(symtable_t *symtable, struct tree_node *node, htab_pair_t *fnc_rec);

/* ------------------------------------------------------------- */
/*                      Traversals functions                     */
/* ------------------------------------------------------------- */

/**
 * @brief Traverse and process BODY node
 *
 * @param fnc_symtable symbol table stack with function records
 * @param symtable symbol table stack with local hashtables(including main program)
 * @param body_node BODY syntax tree node
 * @param fnc_name function name string
 * @return OK if success, else error enum
 */
enum error_state traverse_body(symtable_t *fnc_symtable, symtable_t *symtable, struct tree_node *body_node, char *fnc_name);

/**
 * @brief Traverse and process FNC_DEC node
 *
 * @param fnc_symtable symbol table stack with function records
 * @param symtable symbol table stack with local hashtables(including main program)
 * @param node FNC_DEC syntax tree node
 * @return OK if success, else error enum
 */
enum error_state traverse_fnc_dec(symtable_t *fnc_symtable, symtable_t *symtable, struct tree_node *node);

/**
 * @brief Traverse and process ASSIGN node
 *
 * @param fnc_symtable symbol table stack with function records
 * @param symtable symbol table stack with local hashtables(including main program)
 * @param node ASSIGN syntax tree node
 * @return OK if success, else error enum
 */
enum error_state traverse_assign(symtable_t *fnc_symtable, symtable_t *symtable, struct tree_node *node);

/**
 * @brief Traverse and process EXPRESSION node
 *
 * @param symtable symbol table stack with local hashtables(including main program)
 * @param node EXPRESSION syntax tree node
 * @return OK if success, else error enum
 */
enum error_state traverse_expression(symtable_t *symtable, struct tree_node *node);

/**
 * @brief Traverse and process FUNC_CALL node
 *
 * @param fnc_symtable symbol table stack with function records
 * @param symtable symbol table stack with local hashtables(including main program)
 * @param node FUNC_CALL syntax tree node
 * @return OK if success, else error enum
 */
enum error_state traverse_func_call(symtable_t *fnc_symtable, symtable_t *symtable, struct tree_node *node);

/**
 * @brief Traverse and process operator node
 *
 * @param symtable symbol table stack with local hashtables(including main program)
 * @param node opearator syntax tree node
 * @return OK if success, else error enum
 */
enum error_state traverse_operator(symtable_t *symtable, struct tree_node *node);

/**
 * @brief Traverse and process OPERAND node
 *
 * @param symtable symbol table stack with local hashtables(including main program)
 * @param node OPERAND syntax tree node
 * @return OK if success, else error enum
 */
enum error_state traverse_operand(symtable_t *symtable, struct tree_node *node);

/**
 * @brief Traverse and process IF node
 *
 * @param fnc_symtable symbol table stack with function records
 * @param symtable symbol table stack with local hashtables(including main program)
 * @param node IF syntax tree node
 * @param fnc_name functin name string
 * @return OK if success, else error enum
 */
enum error_state traverse_if(symtable_t *fnc_symtable, symtable_t *symtable, struct tree_node *node, char *fnc_name);

/**
 * @brief Traverse and process WHILE node
 *
 * @param fnc_symtable symbol table stack with function records
 * @param symtable symbol table stack with local hashtables(including main program)
 * @param node WHILE syntax tree node
 * @param fnc_name functin name string
 * @return OK if success, else error enum
 */
enum error_state traverse_while(symtable_t *fnc_symtable, symtable_t *symtable, struct tree_node *node, char *fnc_name);

/**
 * @brief Traverse and process RETURN node
 *
 * @param fnc_symtable symbol table stack with function records
 * @param node RETURN syntax tree node
 * @param fnc_name functin name string
 * @return OK if success, else error enum
 */
enum error_state traverse_return(symtable_t *fnc_symtable, struct tree_node *node, char *fnc_name);
