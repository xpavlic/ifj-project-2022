#include "syntax_tree.h"


struct tn_data * init_tn_data(int type, const char(* value)){
    struct tn_data * data = malloc(sizeof(struct tn_data));
    if (data == NULL) return NULL;
    data->type = type;
    data->value = malloc((strlen(value)+1)*sizeof(char));
    strcpy(data->value,value);
    
    return data;

}

struct tn_data * add_tn_data(struct tree_node * node,int type, const char(* value)){
    node->data = init_tn_data(type,value);
    return node->data;
}


struct tree_node * init_tree_node(){
    struct tree_node * node = malloc(sizeof(struct tree_node));
    if(node == NULL)return NULL;
    
    node->count = 0;

    node->data = NULL;
    node->head_child = NULL;
    node->parent = NULL;
    node->next_sibling = NULL;
    node->tail_child = NULL;

    return node;
}

struct tree_node * add_tree_node(struct tree_node *parent_node,int type, const char(* value)){
    parent_node->count++;
    struct tree_node *new_node = init_tree_node();
    new_node->parent = parent_node;
    add_tn_data(new_node,type,value);

    if(parent_node->tail_child == NULL)  {
        parent_node->head_child = new_node;
        parent_node->tail_child = new_node;
    }
    else{
        parent_node->tail_child->next_sibling = new_node;
        parent_node->tail_child = new_node;
    }
    return new_node;
}




void free_tree_data(struct tn_data *data){
    if(data == NULL) return;
    if(data->value!=NULL){
        free(data->value);
    }
    free(data);    
}

void free_children(struct tree_node * node){
    for(struct tree_node * child_node = node->head_child;child_node!=NULL;){
        struct tree_node * tmp_sibling = child_node->next_sibling;
        free_tree_node(child_node);
        child_node = tmp_sibling;
    }
}

void free_tree_node(struct tree_node * node){
    free_children(node);
    free_tree_data(node->data);
    free(node);
}
