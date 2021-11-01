#ifndef _SCANNER_H
#define _SCANNER_H

#include <stdio.h>
#include "Dynamic_string.h"

typedef enum {
    KEYWORD_AND,
    KEYWORD_AS,
    KEYWORD_ASC,
    KEYWORD_BOOLEN,
    KEYWORD_CONTINUE,
    KEYWORD_DECLARE,
    KEYWORD_DIM,
    KEYWORD_DO,
    KEYWORD_DOUBLE,
    KEYWORD_ELSE,
    KEYWORD_ELSEIF,
    KEYWORD_END,
    KEYWORD_EXIT,
    KEYWORD_FALSE,
    KEYWORD_FOR,
    KEYWORD_CHR,
    KEYWORD_FUNCTION,
    KEYWORD_IF,
    KEYWORD_INPUT,
    KEYWORD_INTEGER,
    KEYWORD_LENGTH,
    KEYWORD_LOOP,
    KEYWORD_NEXT,
    KEYWORD_NOT,
    KEYWORD_OR,
    KEYWORD_PRINT,
    KEYWORD_RETURN,
    KEYWORD_SCOPE,
    KEYWORD_SHARED,
    KEYWORD_STRING,
    KEYWORD_STATIC,
    KEYWORD_SUBSTR,
    KEYWORD_THEN,
    KEYWORD_TRUE,
    KEYWORD_WHILE,
} Keyword;

typedef enum {
    TOKEN_ID_KEYWORD,
    TOKEN_ID_EOL,
    TOKEN_ID_CMA,
    TOKEN_ID_SPACE,
    TOKEN_ID_RBR,
    TOKEN_ID_LBR,
    TOKEN_ID_ADD,
    TOKEN_ID_MUL,
    TOKEN_ID_SUB,
    TOKEN_ID_LCMT2,
    TOKEN_ID_BCMT4,
    TOKEN_ID_DIV2,
    TOKEN_ID_DIV,
    TOKEN_ID_NEQ2,
    TOKEN_ID_LTE,
    TOKEN_ID_LT,
    TOKEN_ID_GTE,
    TOKEN_ID_GT,
    TOKEN_ID_EQ,
    TOKEN_ID_ASSIGN,
    TOKEN_ID_STR5,
    TOKEN_ID_INT0,
    TOKEN_ID_HEX2,
    TOKEN_ID_DHEX2,
    TOKEN_ID_HEXP3,
    TOKEN_ID_EXP3,
    TOKEN_ID_DBL2,
    TOKEN_ID_INT,
    TOKEN_ID_ID,
    TOKEN_ID_EOF
} Token_ID;

typedef union {
    Dynamic_string *string;
    int Integer;
    Keyword keyword;
    double Double;
}Token_Value;

typedef struct {
    Token_ID ID;
    Token_Value Value;
} Token;


//int get_token(Token *token);

#endif