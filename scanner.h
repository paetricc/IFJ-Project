/**
 * Projekt: IFJ2021
 *
 * @brief Implementace scanneru na predavani tokenu.
 *
 * @author Tomáš Bártů xbartu11@stud.fit.vutbr.cz
 * @author Tony Pham xphamt00@stud.fit.vutbr.cz
 * @author Vít Janeček xjanec30@stud.fit.vutbr.cz
 */

#ifndef _SCANNER_H
#define _SCANNER_H

#include <stdio.h>
#include "Dynamic_string.h"
/**
 * Stavy scanneru
 */
#define STATE_START 99
#define STATE_SUB 100
#define STATE_LCMT2 101
#define STATE_BCMT 102
#define STATE_BCMT2 103
#define STATE_BCMT3 104
#define STATE_DIV 105
#define STATE_NEQ 106
#define STATE_LT 107
#define STATE_GT 108
#define STATE_ASSIGN 109
#define STATE_STR 110
#define STATE_STR2 111
#define STATE_STR3 112
#define STATE_STR4 113
#define STATE_STR5 114
#define STATE_STR6 115
#define STATE_STR7 116
#define STATE_STR8 117
#define STATE_INT0 118
#define STATE_HEX 119
#define STATE_HEX2 120
#define STATE_DHEX 121
#define STATE_DHEX2 122
#define STATE_HEXP 123
#define STATE_HEXP2 124
#define STATE_HEXP3 125
#define STATE_DBL 126
#define STATE_DBL2 127
#define STATE_EXP 128
#define STATE_EXP2 129
#define STATE_EXP3 130
#define STATE_INT 131
#define STATE_ID 132
#define STATE_ZERO 133
#define STATE_ERROR 134
#define STATE_DOT 135

/**
 * @enum Klicova slova kodu.
 */

typedef enum {
    KEYWORD_DO,
    KEYWORD_ELSE,
    KEYWORD_END,
    KEYWORD_FUNCTION,
    KEYWORD_GLOBAL,
    KEYWORD_IF,
    KEYWORD_INTEGER,
    KEYWORD_LOCAL,
    KEYWORD_NIL,
    KEYWORD_NUMBER,
    KEYWORD_REQUIRE,
    KEYWORD_RETURN,
    KEYWORD_STRING,
    KEYWORD_THEN,
    KEYWORD_WHILE,
} Keyword;

/**
 * @enum Tokeny, ktery vraci scanner.
 */
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
    TOKEN_ID_FSTR,
    TOKEN_ID_INT0,
    TOKEN_ID_ZERO,
    TOKEN_ID_HEX2,
    TOKEN_ID_DHEX2,
    TOKEN_ID_HEXP3,
    TOKEN_ID_EXP3,
    TOKEN_ID_DBL2,
    TOKEN_ID_INT,
    TOKEN_ID_ID,
    TOKEN_ID_TAB,
    TOKEN_ID_CLN,
    TOKEN_ID_DDOT,
    TOKEN_ID_LEN,
    TOKEN_ID_ERROR,
    TOKEN_ID_EOF
} Token_ID;

/**
 * @union Rozsireni Value->co muze reprezentovat.
 */
typedef union {
    Dynamic_string *string;
    int Integer;
    Keyword keyword;
    double Double;
}Token_Value;

/**
 * @struct Struktura reprezentujici token.
 */
typedef struct {
    Token_ID ID;
    Token_Value Value;
} Token;

/**
 * Funkce prochazi soubor s kodem a skonci, kdyz vraci token.
 *
 * @param token Ukazatel na token do ktereho se ulozi value.
 * @param source_file Ukazatel na soubor s kodem.
 * @return vraci intovou hodnotu, ktera reprezentuje zda nastal error nebo vse probehlo v poradku.
 */
int get_token(Token *, FILE *);

#endif