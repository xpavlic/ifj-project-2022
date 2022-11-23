
#ifndef __HTAB_STRUCTS__
#define __HTAB_STRUCTS__

#include <stdlib.h>

#include "syntax_tree.h"

#define MIN_TABLE_SIZE 10
#define AVG_LEN_MAX 10
#define AVG_LEN_MIN 5

// Typy:
typedef const char* htab_key_t;  // type of key

typedef struct value_info {
    enum tree_node_type arg_var_type;
    enum tree_node_type return_type;
    int number_of_parameters;
    enum tree_node_type* array;
} value_t;
/**
 * @brief pair of key and value
 */
typedef struct htab_pair {
    htab_key_t key;  // name
    void* value;     // pointer to a content structure
} htab_pair_t;       // typedef

typedef struct htab_item {
    htab_pair_t data;
    struct htab_item* next;
} htab_item_t;

struct htab {
    size_t size;
    size_t arr_size;
    htab_item_t** arr_ptr;
};

typedef struct symtable_item {
    struct htab* hash_table;
    struct symtable_item* next;
    char* name;
} symtable_item_t;

typedef struct symtable {
    struct symtable_item* head;
    size_t count;
    void (*free_func)(void*);
} symtable_t;

#endif
