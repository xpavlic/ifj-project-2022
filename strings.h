#ifndef _STRINGS_H
#define _STRINGS_H

#include <stdlib.h>
#include <stdio.h>


typedef struct
{
    char* str;
    unsigned int length;
    unsigned int size;
}dyn_str;


/**
 * @brief Initialiazion of string, allocating memory
 * @param string dynamic string
*/
int init_str(dyn_str *string);

/**
 * @brief Free memory of given string
 * @param string dynamic string
 */
void free_str(dyn_str *string);

/**
 * @brief Reallocates memory of string
 * @param string dynamic string
 * @param length length that has to be allocated
 */
int reallocStr(dyn_str *string, unsigned length);

/**
 * @brief Adds char to the end of the string
 * @param string dynamic string
 * @param letter character
 */
void add_char(dyn_str *string, char letter);
#endif
