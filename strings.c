#include "strings.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int init_str(dyn_str *str)
{
    if(!str)
    {
        return 1;

    }

    str->str = (char*)malloc(12*sizeof(char));

    if(!str->str)
    {
        return 1;
    }
    str->str = "\0";
    str->length = 0;
    str->size = 12;
    return 0;
}

void free_str(dyn_str *str)
{
    free(str->str);
    str->str = NULL;
    str->length = 0;
    str->size = 0;
}

int reallocStr(dyn_str *str, int length)
{
    str->str = (char*)realloc(str->str, str->size + length * 2);
    if(!str->str)
    {
        return 1;
    }
    else
    {
        str->size += length * 2;
        return 0;
    }
}

void add_char(dyn_str *str, char c)
{
    if( str->length + 1 >= str->size )
    {
        reallocStr(str, 6);
    }
    str->str[str->length] = c;
    str->length++;
    str->str[str->length] = '\0';
}
