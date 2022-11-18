#include "syntax_tree.h"
#include "token_stack.h"
#include "scanner.h"


typedef struct {
	struct tree_node *nodes;
} node_stack;


//struct tree_node * check_sons(struct tree_node *root_node, int node_type, char * node_value );

//truct tree_node * ast_parent(struct tree_node * tree, int node_type, char * node_value);

void ast_test(struct tree_node * tree);

