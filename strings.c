#include "strings.h"

const size_t dyn_str_init_size = 16;

int init_str(dyn_str *str) {

    str->str = (char *)malloc(dyn_str_init_size * sizeof(char));

    if (!str->str) {
        return 1;
    }
    str->str[0] = '\0';
    str->length = 0;
    str->size = dyn_str_init_size;
    return 0;
}

void free_str(dyn_str *str) {
    free(str->str);
    str->str = NULL;
    str->length = 0;
    str->size = 0;
}

int add_char(dyn_str *str, char c) {

    if (str->length >= str->size) {

        str->size *= 2;
        str->str = (char *)realloc(str->str, str->size);
        if(!str->str) {
            return 1;
        }
    }
    str->str[str->length] = c;
    str->length++;
    str->str[str->length] = '\0';
    return 0;
}

int del_last(dyn_str *str) {
    if(str->length == 0)
        return 1;

    str->length--;
    str->str[str->length] = '\0';
    return 0;
}
