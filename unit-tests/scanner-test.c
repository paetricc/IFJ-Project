#include <stdio.h>
#include <stdlib.h>
#include "../scanner.h"


/**
 * Prelozi enum hodnotu keyword na string obsahujici jeho nazev.
 *
 * @param keyword Hodnota udavajici o jake klicove slovo se jedna
 * @return String literal odpovidajici klicovemu slovu
*/
char *keyword_to_string(Keyword keyword) {
    switch(keyword) {
        case KEYWORD_DO:
            return "KEY_WORD_DO";
        case KEYWORD_ELSE:
            return "KEYWORD_ELSE";
        case KEYWORD_END:
            return "KEYWORD_END";
        case KEYWORD_FUNCTION:
            return "KEYWORD_FUNCTION";
        case KEYWORD_GLOBAL:
            return "KEYWORD_GLOBAL";
        case KEYWORD_IF:
            return "KEYWORD_IF";
        case KEYWORD_INTEGER:
            return "KEYWORD_INTEGER";
        case KEYWORD_LOCAL:
            return "KEYWORD_LOCAL";
        case KEYWORD_NIL:
            return "KEYWORD_NIL";
        case KEYWORD_NUMBER:
            return "KEYWORD_NUMBER";
        case KEYWORD_REQUIRE:
            return "KEYWORD_REQUIRE";
        case KEYWORD_RETURN:
            return "KEYWORD_RETURN";
        case KEYWORD_STRING:
            return "KEYWORD_STRING";
        case KEYWORD_THEN:
            return "KEYWORD_THEN";
        case KEYWORD_WHILE:
            return "KEYWORD_WHILE";
        case default:
            return NULL;
    }
}

/**
 * Vypise obsah struktury token.
 *
 * @param *token Ukazatel na strukturu tokenu
*/
void print_token(Token *token) {
    printf("%s\n", keyword_to_string(token->Value.keyword));
}

int main() {
    Token *token = malloc(sizeof(token));
    if(token == NULL) {
        fprintf(stderr, "Nepodařilo se přiřadit paměť pro testovací token");
    }
        
    printf ("Testy na lexikální analyzátor - scanner.c\n");
    printf ("-----------------------------------------\n");  
    

    free(token);
    return 0;
}
