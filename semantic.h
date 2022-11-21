
#include "structs.c"
#include "symtable.h"
#include "syntax_tree.h"

#define REDECLARATION_UNDECLARED 3
#define WRONG_ARGUMENT_RETURN 4
#define UNDEFINED_VAR 5
#define TYPE_COMPATIBILITY_ERROR 7
#define INTERNAL_ERROR 99

value_t *value_init();
void value_t_free(void *toBeDeleted);

int check_type(int type_left_child, int type_right_child, int operator);

int traversal_type_propagation(struct tree_node *node, symtable_t *t,
                               symtable_t *fnc_symtable);

void check_arguments(struct tree_node *node, htab_pair_t *fnc_rec);

void process_params(struct tree_node *node, htab_pair_t *pair);

void traversal_second(symtable_t *fnc_symtable, symtable_t *symtable_stack,
                      struct tree_node *node);

void add_builtin_functions(symtable_t *symtable_stack);

int semantic_analysis(struct tree_node *node);

void error_handler(int error);