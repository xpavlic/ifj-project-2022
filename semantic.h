
#include "structs.c"
#include "symtable.h"
#include "syntax_tree.h"

#define REDECLARATION_UNDECLARED 3
#define WRONG_ARGUMENT_RETURN_TYPE 4
#define UNDEFINED_VAR 5
#define WRONG_RETURN 6
#define TYPE_COMPATIBILITY_ERROR 7
#define INTERNAL_ERROR 99

int semantic_analysis(struct tree_node *node);

void value_t_free(void *toBeDeleted);

int eval_ret(char *value);

int process_params(struct tree_node *node, htab_pair_t *pair);

/**
 * @brief Find and add function declarations into a symbol table
 *
 * @param fnc_symtable
 * @param node
 * @return int
 */
int declarations_traversal(symtable_t *fnc_symtable, struct tree_node *node);

/**
 * @brief Add built in functions into a symbol table
 *
 * @param fnc_symtable
 * @return int
 */
int add_builtin_functions(symtable_t *fnc_symtable);

int global_traversal(symtable_t *fnc_symtable, symtable_t *global_symtable, struct tree_node *node);

int traversal_in_function(symtable_t *fnc_symtable, symtable_t *local_symtable, struct tree_node *node, char *name);

int check_type(int type_left_child, int type_right_child, int operator);

int traversal_type_propagation(symtable_t *fnc_symtable, symtable_t *t, struct tree_node *node);

int check_write_arguments(struct tree_node *node, symtable_t *fnc_symtable, symtable_t *t);

int check_arguments(symtable_t *fnc_symtable, symtable_t *local_symtable, struct tree_node *node, htab_pair_t *fnc_rec);

int check_return_type(symtable_t *fnc_symtable, symtable_t *t, struct tree_node *node, char *name);

/**
 * @brief Value_t destructor
 *
 * @param toBeDeleted
 */
void value_t_free(void *toBeDeleted);
int check_write_arguments(struct tree_node *node, symtable_t *fnc_symtable, symtable_t *t);

int check_arguments(symtable_t *fnc_symtable, symtable_t *local_symtable, struct tree_node *node, htab_pair_t *fnc_rec);

int check_return_type(symtable_t *fnc_symtable, symtable_t *t, struct tree_node *node, char *name);

int add_params_into_frame(symtable_t *local_symtable, struct tree_node *PARAMETERS);
