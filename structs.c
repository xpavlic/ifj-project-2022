
#ifndef __HTAB_STRUCTS__
#define __HTAB_STRUCTS__

#include <stdlib.h>

#define MIN_TABLE_SIZE 10
#define AVG_LEN_MAX 10
#define AVG_LEN_MIN 5

// Typy:
typedef const char * htab_key_t;        // type of key
typedef int htab_value_t;               // type of value

/**
 * @brief pair of key and value
*/
typedef struct htab_pair {
    htab_key_t    key;          // key
    void * value;               // associated value
} htab_pair_t;                  // typedef

typedef struct htab_item{
    htab_pair_t data;
    struct htab_item * next;
} htab_item_t;


struct htab{
    size_t size;
    size_t arr_size;
    htab_item_t ** arr_ptr;
};

typedef struct symtable_item{
    struct htab * hash_table;
    struct symtable_item * next;
} symtable_item_t;


struct symtable{
    struct symtable_item* head;
    size_t count;
    void (*free_func)(void*);
};

#endif
