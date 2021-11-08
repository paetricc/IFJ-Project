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
        default:
            return NULL;
    }
}

/**
 * Prelozi enum hodnotu Token_ID na string obsahujici jeho nazev.
 *
 * @param keyword Hodnota udavajici o jake ID tokenu se jedna
 * @return String literal odpovidajici id tokenu
*/
char *Token_ID_to_string(Token_ID id) {
    switch(id) {
        case TOKEN_ID_KEYWORD:
            return "TOKEN_ID_KEYWORD";
        case TOKEN_ID_EOL:
            return "TOKEN_ID_EOL";
        case TOKEN_ID_CMA:
            return "TOKEN_ID_CMA";
        case TOKEN_ID_SPACE:
            return "TOKEN_ID_SPACE";
        case TOKEN_ID_RBR:
            return "TOKEN_ID_RBR";
        case TOKEN_ID_LBR:
            return "TOKEN_ID_LBR";
        case TOKEN_ID_ADD:
            return "TOKEN_ID_ADD";
        case TOKEN_ID_MUL:
            return "TOKEN_ID_MUL";
        case TOKEN_ID_SUB:
            return "TOKEN_ID_SUB";
        case TOKEN_ID_LCMT2:
            return "TOKEN_ID_LCMT2";
        case TOKEN_ID_BCMT4:
            return "TOKEN_ID_BCMT4";
        case TOKEN_ID_DIV2:
            return "TOKEN_ID_DIV2";
        case TOKEN_ID_DIV:
            return "TOKEN_ID_DIV";
        case TOKEN_ID_NEQ2:
            return "TOKEN_ID_NEQ2";
        case TOKEN_ID_LTE:
            return "TOKEN_ID_LTE";
        case TOKEN_ID_LT:
            return "TOKEN_ID_LT";
        case TOKEN_ID_GTE:
            return "TOKEN_ID_GTE";
        case TOKEN_ID_GT:
            return "TOKEN_ID_GT";
        case TOKEN_ID_EQ:
            return "TOKEN_ID_EQ";
        case TOKEN_ID_ASSIGN:
            return "TOKEN_ID_ASSIGN";
        case TOKEN_ID_FSTR:
            return "TOKEN_ID_FSTR";
        case TOKEN_ID_STR5:
            return "TOKEN_ID_STR5";
        case TOKEN_ID_INT0:
            return "TOKEN_ID_INT0";
        case TOKEN_ID_HEX2:
            return "TOKEN_ID_HEX2";
        case TOKEN_ID_DHEX2:
            return "TOKEN_ID_DHEX2";
        case TOKEN_ID_HEXP3:
            return "TOKEN_ID_HEXP3";
        case TOKEN_ID_EXP3:
            return "TOKEN_ID_EXP3";
        case TOKEN_ID_DBL2:
            return "TOKEN_ID_DBL2";
        case TOKEN_ID_INT:
            return "TOKEN_ID_INT";
        case TOKEN_ID_ID:
            return "TOKEN_ID_ID";
        case TOKEN_ID_EOF:
            return "TOKEN_ID_EOF";
    }
}

/**
 * Vypise obsah struktury token.
 *
 * @param *token Ukazatel na strukturu tokenu
*/
void print_token(Token *token) {
    fprintf(stdout, "Token ID: %s\n", Token_ID_to_string(token->ID));
    if(token->ID == TOKEN_ID_KEYWORD)
        fprintf(stdout, "Keyword: %s\n", keyword_to_string(token->Value.keyword));
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
