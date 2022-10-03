#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

/*

HOW TO USE
- give free function to symtable_init how do you want your structure freed
- returned pair from symtable_add/symtable_find has 2 attributes "key" and "value"
        - value is void* pointer where you can save/put your created structure

*/

#include "htab.c"
#include "structs.c"

// SYMTABLE
struct symtable * symtable_init(void (*func)(void*));

void symtable_add_frame(struct symtable * st);

void symtable_remove_frame(struct symtable * t);

void symtable_clear(struct symtable * t);

void symtable_free(struct symtable * t);

htab_pair_t *symtable_find(struct symtable *t, htab_key_t key);

htab_pair_t * symtable_add(struct symtable *t, htab_key_t key);

#endif // __SYMTABLE_H__
