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
typedef struct {
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
typedef struct {
    size_t size;
    size_t arr_size;
    htab_item_t **arr_ptr;
} htab_t;

/**
 * @brief Symtable frame
 *
 */
typedef struct symtable_item {
    htab_t *hash_table;
    struct symtable_item *next;
    char *name;
} symtable_item_t;

/**
 * @brief Symbol table structure
 *
 */
typedef struct {
    symtable_item_t *head;
    size_t count;
    void (*free_func)(void *);
} symtable_t;

/**
 * @brief Symbl table constructor, sets function for later data deallocation
 * @param func function for data deallocation
 * @return symbol table stack if success, else NULL
 */
symtable_t *symtable_init(void (*func)(void *));

/**
 * @brief Table destructor
 * @param t symbol table stack to be freed
 */
void symtable_free(symtable_t *t);

/**
 * @brief Removes all frames from symbol table
 * @param t symbol table stack where frames will be cleared
 */
void symtable_clear(symtable_t *t);

/**
 * @brief Adds new frame to the top of symbol table
 * @param t symbol table stack to add frame
 * @return 0 if sucess, else 1
 */
int symtable_add_frame(symtable_t *t);

/**
 * @brief Removes top frame from symbol table
 * @param t symbol table stack to remove frame from
 */
void symtable_remove_frame(symtable_t *t);

/**
 * @brief Finds htab_pair_t by string key
 * @param t symbol table stack where top frame will be searched
 * @param key string
 * @return structure with matching key if sucess, else NULL
 */
htab_pair_t *symtable_find(symtable_t *t, htab_key_t key);

/**
 * @brief Adds new htab_pair_t with given key
 *
 * @param t symbol table stack where new pair will be added
 * @param key string
 * @param value_create function to create matching value_t structure
 * @return structure with matching key if sucess, else NULL
 */
htab_pair_t *symtable_add(symtable_t *t, htab_key_t key, value_t *(*value_create)());

/**
 * @brief Hash function
 * @param str string
 * @return index to a bucket array if success, else 0
 */
size_t htab_hash_function(htab_key_t str);

/**
 * @brief Hashtable constructor
 * @param n initialization size of a bucket array
 * @return hashtable structure if success, else NULL
 */
htab_t *htab_init(size_t n);

/**
 * @brief Hashtable destructor
 * @param t hashtable structure
 * @param f function for data deallocation
 */
void htab_free(htab_t *t, void (*f)(void *));

/**
 * @brief Removes all the pairs in the hashtable
 * @param t hashtable structure
 * @param f function for data deallocation
 */
void htab_clear(htab_t *t, void (*f)(void *));

/**
 * @brief Returns number of records in table
 * @param const htab_t * t
 * @return number of record in hashtable on success, else 0
 */
size_t htab_size(const htab_t *t);

/**
 * @brief Returns size of array
 * @param t hashtable structure
 * @return number of buckets of hashtable on success, else 0
 */
size_t htab_bucket_count(const htab_t *t);

/**
 * @brief Changes size of hashtable
 * @param t hashtable structure tio resize
 * @param newn new number of buckets to resize
 */
void htab_resize(htab_t *t, size_t newn);

/**
 * @brief Finds pair in hashtable
 * @param t hashtable strusture to search in
 * @param htab_key_t key
 * @return pair structure with matching key if sucess, else NULL
 */
htab_pair_t *htab_find(htab_t *t, htab_key_t key);

/**
 * @brief Adds pair into table
 * @param t hahstable structure to search in/add into
 * @param key string
 * @return pair structure with matching key if sucess, else NULL
 */
htab_pair_t *htab_lookup_add(htab_t *t, htab_key_t key, value_t *(*value_create)());

/**
 * @brief Processes all pairs and calls a function on them(function f cannot modify key and create/remove pair)
 *
 * @param t hashtable structure to process
 * @param f function to call on a pair
 */
void htab_for_each(const htab_t *t, void (*f)(htab_pair_t *data));

/**
 * @brief Does nothing
 */
void nope(void);

/**
 * @brief value_fnc_t constructor
 *
 * @return structure that contains info about function
 */
value_t *value_create_fnc();

/**
 * @brief value_var_t constructor
 *
 * @return structure that contains info about variable
 */
value_t *value_create_var();

/**
 * @brief value_null_t constructor
 *
 * @return NULL
 */
value_t *value_create_null();
