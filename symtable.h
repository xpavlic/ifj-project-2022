/*

Table of Symbols

HOW TO USE
- give destructor of used structure as value to symtable_init 
- returned pair from symtable_add/symtable_find has 2 attributes "key" and "value"
        - value is void* pointer where you can save/put adress of your created structure

*/
#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

#include "htab.h"
#include "structs.c"

/**
 * @brief symtable constructor, sets function for later data deallocation
 * @param void (*func)(void*)
 * @return struct symtable *
*/
struct symtable * symtable_init(void (*func)(void*));

/**
 * @brief Adds new frame to the top of symtable
 * @param struct symtable * t
 * @return void
*/
void symtable_add_frame(struct symtable * t);

/**
 * @brief Removes top frame from symtable
 * @param struct symtable * t
 * @return void
*/
void symtable_remove_frame(struct symtable * t);

/**
 * @brief Removes all frames from symtable
 * @param struct symtable * t
 * @return void
*/
void symtable_clear(struct symtable * t);

/**
 * @brief table destructor
 * @param struct symtable * t
 * @return void
*/
void symtable_free(struct symtable * t);

/**
 * @brief Finds htab_pair_t by string key
 * @param struct symtable * t
 * @param htab_key_t key
 * @return htab_pair_t *
*/
htab_pair_t *symtable_find(struct symtable *t, htab_key_t key);

/**
 * @brief Adds new htab_pair_t with given key
 * @param struct symtable * t
 * @param htab_key_t key
 * @return htab_pair_t *
*/
htab_pair_t * symtable_add(struct symtable *t, htab_key_t key);

#endif // __SYMTABLE_H__
