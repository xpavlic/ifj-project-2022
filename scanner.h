#ifndef _SCANNER_H_
#define _SCANNER_H_

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strings.h"


#define EOL '\n'

typedef struct
{
    dyn_str val;
    int type;
    int line;

}token;


int get_token(FILE *file, token *tk);


typedef enum {

    state_ERROR, //Error state (end)
    state_START, //Start state
    state_EOF, //End of file (end)
    state_EOL, //End of line

    // IDENTIFIERS
    state_IDENTIFIER, //END
    state_IDENTIFIER_KEYWORD, //POSSIBLE
    state_KEYWORD, //END

    // VARIABLE
    state_DOLLAR, //NONEND
    state_VARIABLE, //END

    // TYPE
    state_QUESTIONMARK,
    state_TYPE, //END

    //PROLOG
    state_PROLOG,

    //END
    state_END,

    // NUMERALS
    state_INT,
    state_FLOAT,
    state_FDOT,
    state_FLOAT_N,
    state_FLOAT_EN,
    state_EXPONENT,

    state_PLUS, //END
    state_MINUS, //END
    state_INTDIVIDE,   //POSSIBLEEND
    state_TIMES, //END
    state_EQUAL, //NONEND
    state_OPEQUAL,

    state_EXCLAMATION, //NONEND
    state_MORETHAN, //END
    state_LESSTHAN, //END

    state_LEFTPARENT, //END
    state_RIGHTPARENT, //END
    state_CLEFTPARENT, //END
    state_CRIGHTPARENT, //END

    state_COMMA,
    state_COLON,

    // STRINGS
    state_STRING,
    state_ESCAPE,
    //state_ESCAPE_NUMBER,
    //state_ESCAPE_HEX,
    state_CONCANT, //END

    //Comments
    state_COMMENT,
    state_BLOCK_COMMENT,
    state_BLOCK_COMMENT_END,

    state_SEMICOLON, //END


}state;
#endif
