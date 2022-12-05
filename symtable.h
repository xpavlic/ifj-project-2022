/**
 * @project Compiler IFJ22
 * @file    symtable.h
 * @authors  Štěpán Nekula <xnekul04>, Matej Vadovič <xvadov01>
 */

#pragma once
#include "syntax_tree.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MIN_TABLE_SIZE 10
#define AVG_LEN_MAX 10
#define AVG_LEN_MIN 5
#define HTAB_INIT_VALUE 10

typedef const char *htab_key_t;

enum value_def_type {
    value_fnc,
    value_var,
    value_null
};

/**
 * @brief Hashtable pair variable value structure
 *
 */
typedef struct {
    enum tree_node_type var_type;
} value_var_t;

/**
 * @brief Hashtable pair function value structure
 *
 */
typedef struct {
    char *return_type;
    size_t number_of_parameters;
    char **parameters;
} value_fnc_t;

/**
 * @brief Structure to hold symtable record informations
 *
 */
typedef struct value_info {
    enum value_def_type type;
    union {
        value_fnc_t fnc;
        value_var_t var;
    } val;
} value_t;

/**
 * @brief Hash table pair
 */
typedef struct htab_pair {
    htab_key_t key;
    void *value;
} htab_pair_t;

/**
 * @brief Hashtable list
 *
 */
typedef struct htab_item {
    htab_pair_t data;
    struct htab_item *next;
} htab_item_t;

/**
 * @brief Hashtable
 *
 */
typedef struct htab {
    size_t size;
    size_t arr_size;
    htab_item_t **arr_ptr;
} htab_t;

/**
 * @brief Symtable frame
 *
 */
typedef struct symtable_item {
    struct htab *hash_table;
    struct symtable_item *next;
    char *name;
} symtable_item_t;

/**
 * @brief Symbol table structure
 *
 */
typedef struct symtable {
    struct symtable_item *head;
    size_t count;
    void (*free_func)(void *);
} symtable_t;

/**
 * @brief Symtable constructor, sets function for later data deallocation
 * @param void (*func)(void*)
 * @return struct symtable *
 */
struct symtable *symtable_init(void (*func)(void *));

/**
 * @brief table destructor
 * @param struct symtable * t
 * @return void
 */
void symtable_free(struct symtable *t);

/**
 * @brief Removes all frames from symtable
 * @param struct symtable * t
 * @return void
 */
void symtable_clear(struct symtable *t);

/**
 * @brief Adds new frame to the top of symtable
 * @param struct symtable * t
 * @return int 0 if sucess, 1 else
 */
int symtable_add_frame(struct symtable *t);

/**
 * @brief Removes top frame from symtable
 * @param struct symtable * t
 * @return void
 */
void symtable_remove_frame(struct symtable *t);

/**
 * @brief Finds htab_pair_t by string key
 * @param struct symtable * t
 * @param htab_key_t key
 * @return htab_pair_t *
 */
htab_pair_t *symtable_find(struct symtable *t, htab_key_t key);

/**
 * @brief Adds new htab_pair_t with given key
 *
 * @param t
 * @param key
 * @param value_create
 * @return htab_pair_t*
 */
htab_pair_t *symtable_add(struct symtable *t, htab_key_t key, value_t *(*value_create)());

/**
 * @brief hash function for htab
 * @param htab_key_t str
 * @return size_t
 */
size_t htab_hash_function(htab_key_t str);

/**
 * @brief Hashtable constructor
 * @param size_t n
 * @return htab_t*
 */
htab_t *htab_init(size_t n);

/**
 * @brief Hashtable destructor
 * @param htab_t * t
 * @param void (*f)(void*)
 */
void htab_free(htab_t *t, void (*f)(void *));

/**
 * @brief Removes all the pairs in the hashtable
 * @param htab_t * t
 * @param void (*f)(void*)
 */
void htab_clear(htab_t *t, void (*f)(void *));

/**
 * @brief Returns number of records in table
 * @param const htab_t * t
 * @return size_t
 */
size_t htab_size(const htab_t *t);

/**
 * @brief Returns size of array
 * @param const htab_t * t
 * @return size_t
 */
size_t htab_bucket_count(const htab_t *t);

/**
 * @brief Changes size of table
 * @param htab_t *t
 * @param size_t newn
 */
void htab_resize(htab_t *t, size_t newn);
/**
 * @brief finds pair in table
 * @param const htab_t * t
 * @param htab_key_t key
 * @return htab_pair_t *
 */
htab_pair_t *htab_find(htab_t *t, htab_key_t key);

/**
 * @brief Adds pair into table
 * @param const htab_t * t
 * @param htab_key_t key
 * @return htab_pair_t *
 */
htab_pair_t *htab_lookup_add(htab_t *t, htab_key_t key, value_t *(*value_create)());

/**
 * @brief Processes all pairs and calls a function on them. Function f cannot modify key and create/remove pair.
 *
 * @param t
 * @param f
 */
void htab_for_each(const htab_t *t, void (*f)(htab_pair_t *data));

/**
 * @brief does nothing
 */
void nope(void);

/**
 * @brief value_fnc_t constructor
 *
 * @return value_t*
 */
value_t *value_create_fnc();

/**
 * @brief value_var_t constructor
 *
 * @return value_t*
 */
value_t *value_create_var();

/**
 * @brief value_null_t constructor
 *
 * @return value_t*
 */
value_t *value_create_null();
