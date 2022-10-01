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
 * @param dyn_str
 * @return int
*/
int init_str(dyn_str *string);

/**
 * @brief Free memory of given string
 * @param dyn_str
 * @return void
 */
void free_str(dyn_str *string);

/**
 * @brief Reallocates memory of string
 * @param dyn_str
 * @return int
 */
int realloc_str(dyn_str *string, int length);

/**
 * @brief Adds char to the end of the string
 * @param dyn_str
 * @param int
 */
void add_char(dyn_str *string, char letter);
#endif
