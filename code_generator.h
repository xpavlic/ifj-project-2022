/**
 * @project Compiler IFJ22
 * @file    code_generaotr.h
 * @authors Stepan Nekula <xnekul04>
 */

#ifndef _CODEGEN_H__
#define _CODEGEN_H__

#include "syntax_tree.h"
#include <ctype.h>

/**
 * @brief Generates IFJcode2022 on stdout based on syntax tree
 * @param node: root node of the syntax tree with attribute type set as BODY
*/
void code_generator(struct tree_node * node);


///////////////////////////////////////////
///////////////////////////////////////////
///////////////////////////////////////////
///////////////////////////////////////////
///////////////////////////////////////////

//BASIC TREE NODE HANDLING
/**
 * @brief Generates code for return node
 * @param node: node with type set as RETURN
*/
void print_return_node(struct tree_node * node);
/**
 * @brief Generates code for expression node (and all other nodes that behave as expression)
 * @param node: node with type set as EXPRESSION
*/
void print_expression_node(struct tree_node * node);
/**
 * @brief Generates code for while node
 * @param node: node with type set as WHILE
*/
void print_while_node(struct tree_node * node);
/**
 * @brief Generates code for if
 * @param node: node with type set as IF
*/
void print_if_node(struct tree_node * node);
/**
 * @brief Generates code for variable assign node
 * @param node: node with type set as ASSAIGN
*/
void print_assaign_node(struct tree_node * node);
/**
 * @brief Generates generic code for call of custom functions
 * @param node: node with type set as FUNC_CALL
*/
void print_general_callfunc_node(struct tree_node * node);
/**
 * @brief Generates generic code for call of all functions
 * @param node: node with type set as FUNC_CALL
*/
void print_callfunc_node(struct tree_node * node);
/**
 * @brief Generates code for function declaration
 * @param node: node with type set as FUNC_DEC
*/
void print_decfunc_node(struct tree_node * node);
/**
 * @brief Generates generic code for body node
 * @param node: node with type set as BODY
*/
void print_body_node(struct tree_node * body);
/**
 * @brief Generates code for variable definitions
 * @param node: node with type set as VARIABLES
*/
void print_defvar_node(struct tree_node * body);




// EXPRESSION NODE HANDLING + everything similar to expression 

/**
 * @brief formats string for the needs of expression
 * @param string: string that needs to be formated
*/
void print_string_for_expression(char * string);

/**
 * @brief decides type of node of the expression subnode
 * @param node: node in the expression tree
*/
void choose_expr_print(struct tree_node * node);
/**
 * @brief generates code for usage of operater in expression
 * @param type: enum type of the operator
*/
void print_operator(int type);
/**
 * @brief recursive postorder traversal of the expression tree
 * @param node: root of the expression tree
*/
void postorder(struct tree_node * node);

/**
 * @brief generates definition of function for implicit conversion of int to float
*/
void print_codefunc_int2float_conversion();

/**
 * @brief called recursively, pushes arguments into the stack in reverse order
 * @param node: first argument/ argument to be pushed in the stack
*/
void pushs_arguments(struct tree_node * node);

/**
 * @brief iterates through childrens of body node and decides its handling
 * @param node: bodys node children node
*/
void inbody_scan_node(struct tree_node * node);


/**
 * @brief generates code that is necessary for the start of the generated proram
*/
void print_init_code();

/*
    FUNCTIONS DEFINITION AT THE START OF THE PROGRAM
    -- definitions of functions, that they can be declared
*/

/**
 * @brief generates definiton of inbuild substring function
*/
void print_substring();
/**
 * @brief generates definition of inbuild ord function
*/
void print_ord();
/**
 * @brief generates definition of inbuild strval function
*/
void print_strval();
/**
 * @brief generates definition of inbuild intval function
*/
void print_intval();
/**
 * @brief generates definition of inbuild floatval function
*/
void print_floatval();
/**
 * @brief generates definition of inbuild boolval function
*/
void print_boolval();
/**
 * @brief generates function definition for operators NLTS and NGTS
 * @param instruction: name of the instruction NLTS/NGTS
*/
void print_NLTS_NGTS(char*instruction);
/**
 * @brief generates function definition for operators EQS LTS and GTS
 * @param instruction: name of the instruction EQS/LTS/GTS
*/
void print_EQS_LTS_GTS(char* instruction);


/*
    ALTERNATIVE FUNCTIONS
    -- functions that are implemented as inline
*/
/**
 * @brief generates inline function instead of normal function code
 * @param node: function call node
*/
void print_chr(struct tree_node * node);
/**
 * @brief generates inline function instead of normal function code
 * @param node: function call node
*/
void print_strlen(struct tree_node * node);
/**
 * @brief generates inline function instead of normal function code
 * @param node: function call node
*/
void print_write(struct tree_node * node);
/**
 * @brief generates inline function instead of normal function code
*/
void print_readf();
/**
 * @brief generates inline function instead of normal function code
*/
void print_readi();
/**
 * @brief generates inline function instead of normal function code
*/
void print_reads();



/**
 * @brief generates code for parameter dynamic type check
 * @param type: type of the parameter
*/
void parameter_check_jump(int type);
/**
 * @brief generates code for returned value dynamic type check after the call function
 * @param return_type_node: node for return type
*/
void check_returned_value(struct tree_node * return_type_node);
/**
 * @brief generates code for dynamic type check if top 2 items in stack are neither type of bool or string 
*/
void nor_string_bool();

/**
 * @brief generates code for operator in expression
 * @param type: enum of the operator
*/
void print_operator(int type);

/**
 * @brief checks if number is written as okta
 * @param string: string of the number
 * @return returns 1 if string is okta
*/
int isokta(char * string);
/**
 * @brief chenges hexadecimal digit to decimal number
 * @param c: hexadecimal digit
 * @return returns decimal value of c
*/
int hexa2decimal(char c);

/**
 * @brief  finds childnode of given type
 * @param node: parent node of the searched node
 * @param type: type of the searched node
 * @return pointer to searched child node, if node isnt found return NULL
*/
struct tree_node * find_child_node(struct tree_node * node, int type);

#endif
