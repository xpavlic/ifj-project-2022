/**
 * @project Compiler IFJ22
 * @file scanner.h
 * @authors Martin Soukup <xsouku15>
*/
#ifndef _SCANNER_H_
#define _SCANNER_H_

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strings.h"
#include "token_stack.h"


#define EOL '\n'

/**
 * @brief Function goes through file and build token structure
 * @param FILE Source file
 * @param Token Token structure
 */
int get_token(FILE *file, Token *tk);


typedef enum {

    state_ERROR, //Error state (end)
    state_START, //Start state
    state_EOF, //End of file (end)
    state_EOL, //End of line

    // IDENTIFIERS
    state_IDENTIFIER, //END
    state_IDENTIFIER_KEYWORD,
    state_KEYWORD, //END

    // VARIABLE
    state_DOLLAR,
    state_VARIABLE, //END

    // TYPE
    state_QUESTIONMARK,
    state_TYPE, //END

    //PROLOG
    state_PROLOG,

    //END
    state_END,

    // NUMERALS
    state_INT, //END
    state_FLOAT, //END
    state_FDOT, 
    state_FLOAT_N,
    state_FLOAT_EN,
    state_EXPONENT,

    //OPERATORS
    state_PLUS, //END
    state_MINUS, //END
    state_INTDIVIDE,   //POSSIBLEEND
    state_TIMES, //END
    state_EQUAL, //END
    state_OPEQUAL, //END
    state_NOTEQUAL, //END

    state_EXCLAMATION,
    state_MORETHAN, //END
    state_MOREEQUAL, //END
    state_LESSEQUAL, //END
    state_LESSTHAN, //END

    state_LEFTPARENT, //END
    state_RIGHTPARENT, //END
    state_CLEFTPARENT, //END
    state_CRIGHTPARENT, //END

    state_COMMA, //END
    state_COLON, //END

    // STRINGS
    state_STRING, //END
    state_ESCAPE,
    state_CONCANT, //END

    //Comments
    state_COMMENT,
    state_BLOCK_COMMENT,
    state_BLOCK_COMMENT_END,

    state_SEMICOLON, //END


}state;
#endif
