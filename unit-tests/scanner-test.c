#include <stdio.h>
#include <stdlib.h>
#include "scanner-test.h"
#include "../scanner.c"
#include "../Dynamic_string.c"

// TODO include scanner-test.h nestaci - musim jak opicka includovat oba .c soubory

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
        case TOKEN_ID_CLN:
            return "TOKEN_ID_CLN";
        case TOKEN_ID_TAB:
            return "TOKEN_ID_TAB";
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
        case TOKEN_ID_ZERO:
            return "TOKEN_ID_ZERO";
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
    // vytisknu id tokenu
    fprintf(stdout, "Token ID: %s\n", Token_ID_to_string(token->ID));

    if(token->ID == TOKEN_ID_KEYWORD) // token je klicove slovo
        fprintf(stdout, "Keyword: %s\n", keyword_to_string(token->Value.keyword));

    else if(token->ID == TOKEN_ID_FSTR || token->ID == TOKEN_ID_ID) // token je string
        fprintf(stdout, "String value: %s\n", token->Value.string->str);

    else if(token->ID == TOKEN_ID_INT0 || token->ID == TOKEN_ID_HEX2 ||
                token->ID == TOKEN_ID_INT || token->ID == TOKEN_ID_ZERO) // token je integer
        fprintf(stdout, "Integer value: %d\n", token->Value.Integer);

    else if(token->ID == TOKEN_ID_DHEX2 || token->ID == TOKEN_ID_HEXP3 ||
                token->ID == TOKEN_ID_DBL2 || token->ID == TOKEN_ID_EXP3) // token je double
        fprintf(stdout, "Double value: %f\n", token->Value.Double);
    //else if(token->ID == TOKEN_ID_EXP3) Value je Integer nebo Double - nelze rozhodnout
    else{}
}

/**
 * Test funguji nasledovne:

 * Vstupni soubor odeslu scanneru, ktery z nej vyzobne lexem a vygeneruje token.
 * Na vystup vytisknu jednak lexem, ktery scanner precetl, tak i token, ktery
 * scanner vygeneroval.
 * 
 * Pro praci se vstupnim souboren pouzijeme 4 pomocne promenne, krere slouzi jen
 * a pouze k tomu, abychom mohli zpetne zjistit, jaky lexem byl scannerem precten.
 *
*/
int main() {
    Token *token = malloc(sizeof(token));
    if(token == NULL) {
        fprintf(stderr, "Nepodařilo se přiřadit paměť pro testovací token");
        return 1;
    }

    fprintf(stdout, "Testy na lexikální analyzátor - scanner.c\n");
    fprintf(stdout, "-----------------------------------------\n\n");

    // vstupni soubor
    FILE *read_file = stdin;
    // indexy urcujici pozici v ctenem souboru
    long new_read_index;
    long last_read_index;
    // jezdci udavajici skutecnou pozici v ctenem souboru
    fpos_t new_read_pos;
    fpos_t last_read_pos;
    // ulozim si jezdec na zacatku cteneho souboru
    fgetpos(read_file, &last_read_pos);
    
    do { 
        // ziskam token -> ze vstupu je uzobnut lexem
        get_token(token, read_file);
        // ulozim si, kde scanner skoncil s ctenim
        fgetpos(read_file, &new_read_pos);
        new_read_index = ftell(read_file);

        // budu cist na pozici, kde zacal puvodne scanner
        fsetpos(read_file, &last_read_pos);
        last_read_index = ftell(read_file);
        // a vsechno, co scanner precetl si ulozim do stringu
        char read_lex[new_read_index - last_read_index + 1];
        int i = 0;
        
        //budu cist tak dlouho, dokud nenarazim na pozici, kde skoncil scanner
        while(new_read_index != last_read_index) {
            // jednotlive znaky prectene scannerem ukladam do stringu
            read_lex[i] = fgetc(read_file);
            // a ulozim si index, na kterem ctu
            last_read_index = ftell(read_file);
            i++;
        }
        // string ukoncim a ulozim si, kde scanner (a taky ted ja) prestal cist
        read_lex[i] = '\0';
        fgetpos(read_file, &last_read_pos);

        fprintf(stdout, "Scanner přečetl lexém:[%s]\n", read_lex);
        fprintf(stdout, "a vrátil token: \n");
        print_token(token);
        fprintf(stdout, "\n\n");
    } while(token->ID != TOKEN_ID_EOF);

    free(token);
    return 0;
}
