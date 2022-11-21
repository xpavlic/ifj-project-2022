
#ifndef __HTAB_STRUCTS__
#define __HTAB_STRUCTS__

#include <stdlib.h>

#define MIN_TABLE_SIZE 10
#define AVG_LEN_MAX 10
#define AVG_LEN_MIN 5

enum tree_node_type {
    BODY,          // null
    ASSIGN,        // null
    FIRST_ASSIGN,  // null
    IF,            // null
    WHILE,         // null
    ARGUMENTS,     // null
    TYPE,          // string
    NAME,          // string

    RETURN,     // null
    FUNC_DEC,   // null
    FUNC_CALL,  // null
    RETURN_TYPE,

    OPERATOR,  // string

    EXPRESSION,  // null

    // OPERATORS
    PLUS_OPERATOR,            // +
    MINUS_OPERATOR,           // -
    DIVISION_OPERATOR,        // /
    MULTIPLICATION_OPERATOR,  // *
    CONCATENATION_OPERATOR,   // .
    EQUAL_OPERATOR,           // ===
    NOT_EQUAL_OPERATOR,       // !==
    SMALLER_OPERATOR,         // <
    BIGGER_OPERATOR,          // >
    SMALLER_EQUAL_OPERATOR,   // <=
    BIGGER_EQUAL_OPERATOR,    // >=

    // TERMINALS
    T_STRING,     // string
    T_INT,        // string
    T_FLOAT,      // string
    T_NULL,       // null/string
    VAR_OPERAND,  // string

    // PARAMETERS
    PARAMETERS,       // null
    STR_PARAMETER,    // string
    NULL_PARAMETER,   // string
    INT_PARAMETER,    // string
    FLOAT_PARAMETER,  // string

    STR_NULL_PARAMETER,
    INT_NULL_PARAMETER,
    FLOAT_NULL_PARAMETER,

    ARGUMENT,  // TO DELETE, instead use 'terminal'/VAR_OPERAND

};

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
} symtable_item_t;

typedef struct symtable {
    struct symtable_item* head;
    size_t count;
    void (*free_func)(void*);
} symtable_t;

#endif
