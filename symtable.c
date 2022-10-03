#include "symtable.h"

#define HTAB_INIT_VALUE 10

struct symtable * symtable_init(void (*func)(void*)){
    struct symtable * ptr = malloc(sizeof(struct symtable));
    ptr->count = 0;
    ptr->head = NULL;
    ptr->free_func = func;
    return ptr;
}

void symtable_add_frame(struct symtable * st){
    //check
    if(st == NULL) return;

    st->count++;
    symtable_item_t * tmp = st->head;
    st->head = malloc(sizeof(symtable_item_t));
    st->head->next = tmp;

    st->head->hash_table = htab_init(HTAB_INIT_VALUE);

}

void symtable_remove_frame(struct symtable * t){
    if(t == NULL) return;
    if(t->head == NULL) return;
    t->count--;
    symtable_item_t * tmp = t->head;
    t->head = tmp->next;

    htab_free(tmp->hash_table,t->free_func);
    free(tmp);
}

void symtable_clear(struct symtable * t){
    if(t == NULL) return;
    while(t->head!=NULL){
        symtable_remove_frame(t);
    }
}

void symtable_free(struct symtable * t){
    symtable_clear(t);
    free(t);
}

htab_pair_t *symtable_find(struct symtable *t, htab_key_t key){
    if(t == NULL) return NULL;
    htab_pair_t * found_value = NULL;
    for(struct symtable_item* node = t->head;node != NULL;node = node->next){
        

        found_value = htab_find(node->hash_table, key);
        if(found_value != NULL) return found_value;
    }

    return NULL;

}

htab_pair_t * symtable_add(struct symtable *t, htab_key_t key){
    if(t == NULL) return NULL;
    if(t->head == NULL) symtable_add_frame(t);
    return htab_lookup_add(t->head->hash_table,key);
}



