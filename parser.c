#include "scanner.h"
#include "parser.h"
#include <stdlib.h>

void parse(FILE *source_file) {
    Token *test = (Token *) malloc(sizeof(Token));
    do {
        get_token(test, source_file);
        switch (test->ID)
        {
        case TOKEN_ID_KEYWORD:
            printf("TOKEN_ID_KEYWORD: %s\n", test->Value.string->str);
            break;
        case TOKEN_ID_EOF:
        break;
        case TOKEN_ID_ID:
            printf("TOKEN_ID_ID: %s\n", test->Value.string->str);
            break;
        case TOKEN_ID_INT:
            printf("TOKEN_ID_INT: %d\n", test->Value.Integer);
            break;
        // par casu -> po otestovani jednoho bych pak pridal dalsi a ten opet otestoval
        // a asi bude chyba pri TOKEN_ID_ID v (file: Dynamic_string.c, row 32, s_new->str => saha nikam)
        default:
            break;
        }
    //printf("Token:{ Double -> %f, Integer -> %d, Keyword -> %d, String -> %s \n", test->Value.Double, test->Value.Integer, test->Value.keyword, test->Value.string->str);
    } while(test->ID != TOKEN_ID_EOF);
}   