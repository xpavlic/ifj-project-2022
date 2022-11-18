//
// Created by Martin Soukup
//

#include "ast.h"


void ast_test(struct tree_node * tree)
{
	if( tree->tail_child != NULL)
	{
		struct tree_node * tmp = tree->tail_child;
		
		printf("Root je : %s\n", tmp->data->value);
		if( tmp->head_child != NULL)
		{
			printf("Prvni syn : %s\n", tmp->head_child->data->value);

			if( tmp->head_child->next_sibling != NULL )
			{
				printf("Druhy syn : %s\n", tmp->head_child->next_sibling->data->value);

				if( tmp->head_child->next_sibling->next_sibling != NULL )
				{
					printf("Treti syn : %s\n", tmp->head_child->next_sibling->next_sibling->data->value);

					if ( tmp->head_child->next_sibling->next_sibling->next_sibling != NULL )
					{
						printf("Ctvrt syn : %s\n", tmp->head_child->next_sibling->next_sibling->next_sibling->data->value);						
					}
				}
			}
		}
		tmp = tmp->next_sibling;	
	}
}
