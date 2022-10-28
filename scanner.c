#include "scanner.h"

static int line;
static char lastc;
static int indexE;
static int indexH;

static char escapeNumber[3];

static char *kwords[] = {
    "else\0", "float\0",  "function\0", "if\0",   "int\0",
    "null\0", "return\0", "string\0",   "void\0", "while\0",
};

int get_token(FILE *file, token *tk) {
    state state = state_START;
    // free_str(&tk->val);
    init_str(&tk->val);
    char c;
    while (1) {
        c = getc(file);
        switch (state) {
            case state_START:
                if (isspace(c) != 0) {
                    state = state_START;
                } else if (isalpha(c)) {
                    add_char(&tk->val, c);
                    state = state_IDENTIFIER_KEYWORD;
                } else if (c == '_') {
                    state = state_IDENTIFIER;
                } else if (isdigit(c)) {
                    add_char(&tk->val, c);
                    state = state_INT;
                } else if (c == '+') {
                    add_char(&tk->val, c);
                    tk->type = state_PLUS;
                    return 0;
                } else if (c == '-') {
                    add_char(&tk->val, c);
                    tk->type = state_MINUS;
                    return 0;
                } else if (c == '/') {
                    add_char(&tk->val, c);
                    state = state_INTDIVIDE;
                } else if (c == '*') {
                    add_char(&tk->val, c);
                    tk->type = state_TIMES;
                    return 0;
                } else if (c == '=') {
                    add_char(&tk->val, c);
                    state = state_EQUAL;
                } else if (c == '!') {
                    add_char(&tk->val, c);
                    state = state_EXCLAMATION;
                } else if (c == '>') {
                    add_char(&tk->val, c);
                    tk->type = state_MORETHAN;
                    return 0;
                } else if (c == '<') {
                    add_char(&tk->val, c);
                    state = state_LESSTHAN;
                } else if (c == '?') {
                    add_char(&tk->val, c);
                    state = state_QUESTIONMARK;
                } else if (c == '(') {
                    add_char(&tk->val, c);
                    tk->type = state_LEFTPARENT;
                    return 0;
                } else if (c == ')') {
                    add_char(&tk->val, c);
                    tk->type = state_RIGHTPARENT;
                    return 0;
                } else if (c == '$') {
                    add_char(&tk->val, c);
                    state = state_DOLLAR;
                } else if (c == ',') {
                    add_char( &tk->val, c);
                    state = state_COMMA;
                    return 0;
                } else if (c == '.') {
                    add_char(&tk->val, c);
                    tk->type = state_CONCANT;
                    return 0;
                } else if (c == ';') {
                    add_char(&tk->val, c);
                    tk->type = state_SEMICOLON;
                    return 0;
                } else if (c == '{') {
                    add_char(&tk->val, c);
                    tk->type = state_CLEFTPARENT;
                    return 0;
                } else if (c == '}') {
                    add_char(&tk->val, c);
                    tk->type = state_CRIGHTPARENT;
                    return 0;
                } else if (c == EOF) {
                    tk->type = state_EOF;
                    return 0;
                } else if (c == EOL) {
                    tk->type = state_EOL;
                    tk->line++;
                    return 0;
                } else if (c == '"') {
                    add_char(&tk->val, c);
                    state = state_STRING;
                } else if ( c == ':') {
                    add_char(&tk->val, c);
                    tk->type = state_COLON;
                    return 0;
                } else {
                    ungetc(c, file);
                    tk->type = state_ERROR;
                    return 1;
                }
                break;

            case state_LESSTHAN:
                if (c == '?') {
                    add_char(&tk->val, c);
                    state = state_PROLOG;

                } else {
                    ungetc(c, file);
                    tk->type = state_LESSTHAN;
                    return 0;
                }
                break;

            case state_PROLOG:
                if (islower(c) != 0) {
                    add_char(&tk->val, c);
                } else {
                    ungetc(c, file);
                    tk->type = state_PROLOG;
                    return 0;
                }
                break;
            case state_EOL:
                ungetc(c, file);
                line++;
                state = state_START;
                break;
            case state_IDENTIFIER:
                if (c == '_' || isalpha(c) || isdigit(c)) {
                    add_char(&tk->val, c);
                } else {
                    ungetc(c, file);
                    if (tk->type == state_VARIABLE) {
                        return 0;
                    } else {
                        tk->type = state_IDENTIFIER;
                        return 0;
                    }
                }
                break;
            case state_IDENTIFIER_KEYWORD:
                if (isalpha(c)) {
                    add_char(&tk->val, c);
                } else if (c == '_' || isdigit(c)) {
                    if (lastc == '?') {
                        ungetc(c, file);
                        tk->type = state_TYPE;
                        return 0;
                    }

                    add_char(&tk->val, c);
                    printf("%c", c);
                    state = state_IDENTIFIER;
                } else {
                    ungetc(c, file);
                    for (int i = 0; i < 9; i++) {
                        if (!strcmp(tk->val.str, kwords[i])) {
                            tk->type = state_KEYWORD;
                            return 0;
                        }
                    }

                    state = state_IDENTIFIER;
                }
                break;
            case state_DOLLAR:
                if (c == '_' || isalpha(c)) {
                    add_char(&tk->val, c);
                    tk->type = state_VARIABLE;
                    state = state_IDENTIFIER;
                } else {
                    ungetc(c, file);
                    tk->type = state_ERROR;
                    return 1;
                }
                break;
            case state_QUESTIONMARK:
                if (isalpha(c)) {
                    add_char(&tk->val, c);
                    state = state_IDENTIFIER_KEYWORD;
                    lastc = '?';
                } else if (c == '>') {
                    add_char(&tk->val, c);
                    tk->type = state_END;
                    return 0;
                } else {
                    ungetc(c, file);
                    tk->type = state_ERROR;
                    return 1;
                }
                break;
            case state_EQUAL:
                if (c == '=') {
                    add_char(&tk->val, c);
                    state = state_OPEQUAL;
                } else {
                    ungetc(c, file);
                    tk->type = state_EQUAL;
                    return 0;
                }
                break;
            case state_OPEQUAL:
                if (c != '=') {
                    ungetc(c, file);
                    tk->type = state_ERROR;
                    return 1;
                }
                add_char(&tk->val, c);
                tk->type = state_OPEQUAL;
                return 0;
            case state_INT:
                if (isdigit(c)) {
                    add_char(&tk->val, c);

                } else if (c == '.') {
                    add_char(&tk->val, c);
                    state = state_DDOT;
                } else if (c == 'e' || c == 'E') {
                    add_char(&tk->val, c);
                    state = state_EXPONENT;
                } else {
                    ungetc(c, file);
                    tk->type = state_INT;
                    return 0;
                }
                break;
            case state_DDOT:
                if (isdigit(c)) {
                    add_char(&tk->val, c);
                    state = state_DOUBLE_N;
                } else {
                    add_char(&tk->val, c);
                    tk->type = state_ERROR;
                    return 1;
                }
                break;
            case state_DOUBLE_N:
                if (isdigit(c)) {
                    add_char(&tk->val, c);
                    state = state_DOUBLE;

                } else if (c == 'e' || c == 'E') {
                    add_char(&tk->val, c);
                    state = state_EXPONENT;
                } else {
                    add_char(&tk->val, c);
                }

                break;
            case state_EXPONENT:
                if (isdigit(c)) {
                    add_char(&tk->val, c);
                    state = state_DOUBLE;

                } else if (c == '+' || c == '-') {
                    add_char(&tk->val, c);
                    state = state_DOUBLE;
                } else {
                    add_char(&tk->val, c);
                    tk->type = state_ERROR;
                    return 1;
                }
                break;
            case state_DOUBLE:
                if (isdigit(c)) {
                    add_char(&tk->val, c);

                } else {
                    ungetc(c, file);
                    tk->type = state_DOUBLE;
                    return 0;
                }
                break;
            case state_STRING:
                if (c == '"') {
                    add_char(&tk->val, c);
                    tk->type = state_STRING;
                    return 0;
                } else if (c == '\\') {
                    add_char(&tk->val, c);
                    state = state_ESCAPE;

                } else if (c > 31) {
                    add_char(&tk->val, c);
                } else {
                    ungetc(c, file);
                    tk->type = state_ERROR;
                    return 1;
                }
                break;
            case state_ESCAPE:
                if (c == '\\') {
                    add_char(&tk->val, c);
                    state = state_STRING;
                } else if (c == 't') {
                    add_char(&tk->val, c);
                    state = state_STRING;
                } else if (c == 'n') {
                    add_char(&tk->val, c);
                    state = state_STRING;
                } else if (c == '"') {
                    add_char(&tk->val, c);
                    state = state_STRING;
                } else if (isdigit(c)) {
                    add_char(&tk->val, c);
                    state = state_ESCAPE_NUMBER;
                } else if (c == 'x') {
                    add_char(&tk->val, c);
                    state = state_ESCAPE_HEX;
                } else {
                    ungetc(c, file);
                    tk->type = state_ERROR;
                    return 1;
                }
                break;
            case state_ESCAPE_NUMBER:
                if (isdigit(c)) {
                    indexE++;
                    add_char(&tk->val, c);
                    escapeNumber[indexE] = c;
                    if (atoi(escapeNumber) > 0 && atoi(escapeNumber) <= 255 &&
                        (int)(escapeNumber[2]) > 0) {
                        state = state_STRING;
                    }

                } else {
                    ungetc(c, file);
                    tk->type = state_ERROR;
                    return 1;
                }
                break;
            case state_ESCAPE_HEX:
                if (indexH == 2) {
                    ungetc(c, file);
                    tk->type = state_ERROR;
                    return 1;
                }
                if (isxdigit(c)) {
                    indexH++;
                    add_char(&tk->val, c);

                } else {
                    ungetc(c, file);
                    tk->type = state_ERROR;
                    return 1;
                }
                break;
            case state_INTDIVIDE:
                if (c == '/') {
                    ungetc(c, file);
                    del_last(&tk->val);
                    state = state_COMMENT;
                } else if (c == '*') {
                    state = state_BLOCK_COMMENT;
                } else {   
                    tk->type = state_INTDIVIDE;
                    return 0;
                }
                break;
            case state_COMMENT:
                if ( c == EOL || c == EOF) {
                    ungetc(c, file);
                    state = state_START;
                }
                break;
            case state_BLOCK_COMMENT:
                if (c == '*') {
                    state = state_BLOCK_COMMENT_END;
                } else if (c == EOF) {
                    tk->type = state_EOF;
                    return 0;
                } else {
                    state = state_BLOCK_COMMENT;
                }
                break;
            case state_BLOCK_COMMENT_END:
                if (c != '/') {
                    state = state_BLOCK_COMMENT;
                } else {
                    state = state_START;
                }
                break;
            case state_ERROR:
            case state_KEYWORD:
            case state_VARIABLE:
            case state_EOF:
            case state_TYPE:
            case state_END:
            case state_PLUS:
            case state_MINUS:
            case state_TIMES:
            case state_EXCLAMATION:
            case state_MORETHAN:
            case state_LEFTPARENT:
            case state_RIGHTPARENT:
            case state_CLEFTPARENT:
            case state_CRIGHTPARENT:
            case state_CONCANT:
            case state_COMMA:
            case state_COLON:
            case state_SEMICOLON:
                return 0;
        }
    }
}
