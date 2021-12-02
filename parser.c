/**
 * Projekt: IFJ2021
 * 
 * @brief Implementace top-down syntakticke anylzy.
 *
 * @author Šimon Vacek xvacek10@stud.fit.vutbr.cz
*/

#include <stdlib.h>
#include <string.h>

#include "scanner.h"
#include "parser.h"
#include "error.h"
#include "symtable.h"
#include "expression.h"


/**
 * @brief Tabulka symbolu
 *
 * Implemenotvana hybridem seznamu a zasobniku.
 */
SLList_Frame *symTable;


/**
 * @brief Nacita tokeny dokud je v nich nektery bily znak, nebo az se narazi na EOF.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 *
 * @return Typ erroru generovany analyzou
*/
int get_non_white_token(Token *token, FILE *sourceFile) {
    int error;
    do {
        // nacteni noveho tokenu a kontrola vysupu scanneru
        if ((error = get_token(token, sourceFile)) != ERROR_PASSED)
            // lexikalni nebo kompilatorova chyba
            return error;
    } while (token->ID != TOKEN_ID_EOF && (
            token->ID == TOKEN_ID_SPACE || token->ID == TOKEN_ID_EOL ||
            token->ID == TOKEN_ID_TAB || token->ID == TOKEN_ID_LCMT2 ||
            token->ID == TOKEN_ID_BCMT4));
    return error;
}


/**
 * @brief Neterminal start.
 *
 * Implementuje pravidlo 1.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int start(Token *token, FILE *sourceFile) {
    int error;
    // require
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_KEYWORD)
        return ERROR_SYNTAX;
    else if (token->Value.keyword != KEYWORD_REQUIRE)
        return ERROR_SYNTAX;

    // "ifj21"
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_FSTR)
        return ERROR_SYNTAX;
    else if (strcmp(token->Value.string->str, "\"ifj21\""))
        return ERROR_SYNTAX;

    // nainicializovani symtable
    symTable = (SLList_Frame *) malloc(sizeof(SLList_Frame));
    SLL_Frame_Init(symTable);

    if (symTable == NULL)
        return ERROR_COMPILER;
    // vytvoreni globalniho ramce
    SLL_Frame_Insert(symTable);

    // vse korektni - uplatnuju pravidlo a rozsiruju dalsi neterminal
    error = program(token, sourceFile); // aplikace pravidla 1

    // uvoleni symTable
    SLL_Frame_Dispose(symTable);
    free(symTable);
    return error;
}

/**
 * @brief Neterminal program.
 *
 * Implementuje pravidla 2, 3, 4, 5.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int program(Token *token, FILE *sourceFile) {
    int error;



    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);

    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    // rozvetveni na ruzna pravidla podle hodnoty tokenu
    switch (token->ID) {
        case TOKEN_ID_KEYWORD: // global nebo function
            if (token->Value.keyword == KEYWORD_GLOBAL) { // global
                error = fnc_dec(token, sourceFile); // aplikace pravidla 2
                if (error)
                    return error;
            } else if (token->Value.keyword == KEYWORD_FUNCTION) { // function
                error = fnc_def(token, sourceFile); // aplikace pravidla 4
                if (error)
                    return error;
            } else { // private keyword nelze pouzit - pravidlo neexistuje
                return ERROR_SYNTAX;
            }
            break;

        case TOKEN_ID_ID: // id_fnc
            // nastaveni cteni pred identifikator, aby si to precetl volany
            fsetpos(sourceFile, &lastReadPos);
            if ((error = fnc_call(token, sourceFile))) // aplikace pravidla 3
                return error;
            break;

        case TOKEN_ID_EOF: // konec souboru - syntaxe je korektni, muze se ukoncit
            return ERROR_PASSED; // aplikace pravidla 5

        default: // pro token zadne pravidlo neexistuje
            return ERROR_SYNTAX;
    } // switch

    // rekurzivni volani neterminalu program
    return program(token, sourceFile);
} //program


/**
 * @brief Neterminal fnc_dec.
 *
 * Implementuje pravidlo 6.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int fnc_dec(Token *token, FILE *sourceFile) {
    // token global byl prijaty o uroven vyse => pokracuju dale
    // aplikuju pravidlo 6
    int error;

    // id_fnc
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_ID)
        return ERROR_SYNTAX;

    // overeni, zda nebyla funkce uz deklarovana nebo definovana
    if (search_Iden(token->Value.string, symTable) != NULL)
        return ERROR_SEM_UNDEFINED;
    // pridani id_fnc do symtable
    bst_insert(&(symTable->globalElement->node), token->Value.string, true);


    // ':'
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_CLN)
        return ERROR_SYNTAX;


    // function
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_KEYWORD)
        return ERROR_SYNTAX;
    else if (token->Value.keyword != KEYWORD_FUNCTION)
        return ERROR_SYNTAX;


    // '('
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_LBR)
        return ERROR_SYNTAX;


    // rozvinuti neterminalu params_dec
    if ((error = params_dec(token, sourceFile)))
        return error;


    // ')'
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_RBR)
        return ERROR_SYNTAX;


    // rozvinuti neterminalu return_type
    if ((error = return_type(token, sourceFile)))
        return error;

    return ERROR_PASSED;
} // fnc_dec


/**
 * @brief Neterminal params_dec.
 *
 * Implementuje pravidla 7,8.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int params_dec(Token *token, FILE *sourceFile) {
    int error;

    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    // rozvetveni na ruzna pravidla podle hodnoty tokenu
    switch (token->ID) {
        case TOKEN_ID_RBR: // )
            // vratim cteni pred zavorku, aby ji mohl precist volajici
            fsetpos(sourceFile, &lastReadPos);
            return ERROR_PASSED; // aplikace pravidla 7

        case TOKEN_ID_KEYWORD: // integer, number nebo string
            if (token->Value.keyword == KEYWORD_INTEGER || token->Value.keyword == KEYWORD_NUMBER ||
                token->Value.keyword == KEYWORD_STRING) {

                // vratim cteni pred keyword, aby ji mohl precist volany
                fsetpos(sourceFile, &lastReadPos);

                // rozvinuti neterminalu data_type
                if ((error = data_type(token, sourceFile))) // aplikace pravidla 8
                    return error;
            } else // prijate keyword nelze pouzit - pravidlo neexistuje
                return ERROR_SYNTAX;
            break;

        default: // pro token zadne pravidlo neexistuje
            return ERROR_SYNTAX;
    }

    return params_dec2(token, sourceFile);
} // params_dec


/**
 * @brief Neterminal params_dec2.
 *
 * Implementuje pravidla 9, 10.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int params_dec2(Token *token, FILE *sourceFile) {
    int error;

    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID == TOKEN_ID_RBR) { // ')'
        // vratim cteni pred zavorku, aby ji mohl precist volajici
        fsetpos(sourceFile, &lastReadPos);
        return ERROR_PASSED; // aplikace pravidla 9
    } else if (token->ID == TOKEN_ID_CMA) { // ','
        if ((error = data_type(token, sourceFile))) // aplikace pravidla 10
            return error;
    } else {
        return ERROR_SYNTAX;
    }

    return params_dec2(token, sourceFile);
}

/**
 * @brief Neterminal return_type.
 *
 * Implementuje pravidla 11, 12.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int return_type(Token *token, FILE *sourceFile) {
    int error;

    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    // rozvetveni na ruzna pravidla podle hodnoty tokenu
    switch (token->ID) {
        case TOKEN_ID_CLN: // ':'
            break;

        case TOKEN_ID_KEYWORD: // global nebo function
            if (token->Value.keyword == KEYWORD_GLOBAL || token->Value.keyword == KEYWORD_FUNCTION) {
                // vratim cteni pred keyword, aby ho mohl znovu precist volajici
                fsetpos(sourceFile, &lastReadPos);
                return ERROR_PASSED; // aplikace pravidla 12
            } else { // prijate keyword nelze pouzit - pravidlo neexistuje
                return ERROR_SYNTAX;
            }

        case TOKEN_ID_EOF: // konec souboru
            // vratim cteni pred keyword, aby ho mohl znovu precist volajici
            fsetpos(sourceFile, &lastReadPos);
            return ERROR_PASSED; // aplikace pravidla 12

        case TOKEN_ID_ID: // id_var
            // vratim cteni pred keyword, aby ho mohl znovu precist volajici
            fsetpos(sourceFile, &lastReadPos);
            return ERROR_PASSED; // aplikace pravidla 12

        default: // pro token zadne pravidlo neexistuje
            return ERROR_SYNTAX;
    }

    return data_type(token, sourceFile); // aplikace pravidla 11
} // return_type


/**
 * @brief Neterminal data_type.
 *
 * Implementuje pravidel 13, 14, 15.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int data_type(Token *token, FILE *sourceFile) {
    int error;
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_KEYWORD) // nevalidni token
        return ERROR_SYNTAX;

    // rozvetveni na ruzna pravidla podle hodnoty tokenu
    switch (token->Value.keyword) {
        case KEYWORD_INTEGER: // aplikace pravidla 13
        case KEYWORD_NUMBER: // aplikace pravidla 14
        case KEYWORD_STRING: // aplikace pravidla 15
            break;

        default:
            return ERROR_SYNTAX; // nevalidni klicove slovo
    }

    return ERROR_PASSED;
}


/**
 * @brief Neterminal fnc_call.
 *
 * Implementuje pravidlo 16.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int fnc_call(Token *token, FILE *sourceFile) {
    // aplikuju pravidlo 16
    int error;

    // id_fnc
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_ID)
        return ERROR_SYNTAX;

    // vyhledani id_fnc v symtable
    if (search_Iden(token->Value.string, symTable) == NULL)
        return ERROR_SEM_UNDEFINED;
    //TODO nize se musi do symtable ulozit i parametry a dalsi srandy
    // k tomu budu ale potrebovat znat bud id_fnc, nebo mit ukazatel na jeji uzel

    // '('
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_LBR)
        return ERROR_SYNTAX;


    // rozvinuti neterminalu value
    if ((error = value(token, sourceFile)))
        return error;


    // ')'
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_RBR)
        return ERROR_SYNTAX;

    return ERROR_PASSED;
}


/**
 * @brief Neterminal value.
 *
 * Implementuje pravidla 17 a 18.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int value(Token *token, FILE *sourceFile) {
    int error;
    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);

    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    switch (token->ID) {
        case TOKEN_ID_RBR: // ')'
            // nasteveni cteni pred zavorku, aby si ji precetl volajici
            fsetpos(sourceFile, &lastReadPos);
            return ERROR_PASSED; // aplikace pravidla 17

        case TOKEN_ID_KEYWORD: // nil
            if (token->Value.keyword == KEYWORD_NIL) {
                // nasteveni cteni pred nil, aby si ji precetl volany
                fsetpos(sourceFile, &lastReadPos);
                value_last(token, sourceFile); // aplikace pravidla 18
            }
            break;


        case TOKEN_ID_ID: // id_var
        case TOKEN_ID_INT0:
        case TOKEN_ID_HEX2:
        case TOKEN_ID_INT:
        case TOKEN_ID_ZERO: // int_value
        case TOKEN_ID_DHEX2:
        case TOKEN_ID_HEXP3:
        case TOKEN_ID_DBL2:
        case TOKEN_ID_EXP3: // num_value
        case TOKEN_ID_FSTR: // str_value
            // nasteveni cteni pred hodnotu, aby si ji precetl volany
            fsetpos(sourceFile, &lastReadPos);
            value_last(token, sourceFile); // aplikace pravidla 18
            break;

        default:
            return ERROR_SYNTAX;
    }

    return value2(token, sourceFile);
} // value


/**
 * @brief Neterminal value2.
 *
 * Implementuje pravidla 19 a 20.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int value2(Token *token, FILE *sourceFile) {
    int error;
    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);

    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID == TOKEN_ID_RBR) { // ')'
        // vratim cteni pred zavorku, aby si ji precetl volajici
        fsetpos(sourceFile, &lastReadPos);
        return ERROR_PASSED; // aplikace pravidla 20
    } else if (token->ID == TOKEN_ID_CMA) { // ','
        // aplikace pravidla 19
    } else { // pro token zadne pravidlo neexistuje
        return ERROR_SYNTAX;
    }

    // rozvinuti neterminalu value_last
    if ((error = value_last(token, sourceFile)))
        return error;

    return value2(token, sourceFile);
}


/**
 * @brief Neterminal value_last.
 *
 * Implementuje pravidla  21, 22, 23, 24 a 25.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int value_last(Token *token, FILE *sourceFile) {
    int error;
    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);

    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    switch (token->ID) {
        case TOKEN_ID_ID: // id_var
            // TODO aplikace pravidla 21
            // overim, ze jde o id_fnc a ne id_var
            if (isFnc(search_Iden(token->Value.string, symTable)))
                return ERROR_SYNTAX;
            break;

        case TOKEN_ID_INT0:
        case TOKEN_ID_HEX2:
        case TOKEN_ID_INT:
        case TOKEN_ID_ZERO: // int_value
            // TODO aplikace pravidla 22
            break;

        case TOKEN_ID_DHEX2:
        case TOKEN_ID_HEXP3:
        case TOKEN_ID_DBL2:
        case TOKEN_ID_EXP3: // num_value
            // TODO aplikace pravidla 23
            break;

        case TOKEN_ID_FSTR: // str_value
            // TODO aplikace pravidla 24
            break;

        case TOKEN_ID_KEYWORD: // nil
            if (token->Value.keyword == KEYWORD_NIL) {
                // TODO aplikace pravidal 25
            } else // pro keyword neexistuje pravidlo
                return ERROR_SYNTAX;
            break;

        default: // pro token zadne pravidlo neexistuje
            return ERROR_SYNTAX;
    }

    return ERROR_PASSED;
} // value_last


/**
 * @brief Neterminal fnc_def.
 *
 * Implementuje pravidlo 26.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int fnc_def(Token *token, FILE *sourceFile) {
    // token function byl prijaty o uroven vyse => pokracuju dale
    // aplikuju pravidlo 26
    int error;

    // rozvinuti neterminalu fnc_head
    if ((error = fnc_head(token, sourceFile)))
        return error;

    // rozvinuti neterminalu fnc_def2
    if ((error = fnc_def2(token, sourceFile)))
        return error;

    // end
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_KEYWORD)
        return ERROR_SYNTAX;
    else if (token->Value.keyword != KEYWORD_END)
        return ERROR_SYNTAX;

    // odeberu ramec funkce
    SLL_Frame_Delete(symTable);

    return ERROR_PASSED;
}


/**
 * @brief Neterminal fnc_head.
 *
 * Implementuje pravidlo 27.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int fnc_head(Token *token, FILE *sourceFile) {
    // token function byl prijaty o uroven vyse => pokracuju dale
    // aplikuju pravidlo 27
    int error;

    // id_fnc
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;
    if (token->ID != TOKEN_ID_ID)
        return ERROR_SYNTAX;

    // overeni, zda nebyla funkce uz deklarovana nebo definovana
    bst_node_t *idFnc = search_Iden(token->Value.string, symTable);

    if (idFnc == NULL)
        bst_insert(&(symTable->globalElement->node), token->Value.string, true);
    else if (!isDefFnc(idFnc)) {
        setDataF(idFnc, true);
    } else {
        return ERROR_SEM_UNDEFINED;
    }

    // '('
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;
    if (token->ID != TOKEN_ID_LBR)
        return ERROR_SYNTAX;

    // vytvoreni noveho ramce pro funkci
    SLL_Frame_Insert(symTable);

    // rozvinu neterminal params_def
    if ((error = params_def(token, sourceFile)))
        return error;


    // ')'
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;
    if (token->ID != TOKEN_ID_RBR)
        return ERROR_SYNTAX;

    return ERROR_PASSED;
} // fnc_head


/**
 * @brief Neterminal fnc_def2.
 *
 * Implementuje pravidlo 28 a 29.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int fnc_def2(Token *token, FILE *sourceFile) {
    int error;

    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);

    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    // ':'
    if (token->ID == TOKEN_ID_CLN) { // aplikace pravidla 29
        // rozvinuti neterminalu data_type
        if ((error = data_type(token, sourceFile)))
            return error;

        // rozvinuti neterminalu fnc_body
        if ((error = fnc_body(token, sourceFile)))
            return error;

        // rozvinuti neterminalu return
        if ((error = return_(token, sourceFile)))
            return error;
    } else { // id_fce nebo id_var
        // aplikace pravidla 28

        if (token->ID == TOKEN_ID_ID) {
            // korektni zadani
        } else if (token->ID == TOKEN_ID_KEYWORD) { // local, if, while, return nebo end
            switch (token->Value.keyword) {
                case KEYWORD_LOCAL:
                case KEYWORD_IF:
                case KEYWORD_WHILE:
                case KEYWORD_RETURN:
                case KEYWORD_END:
                    // korektni zadani
                    break;

                default: // pro tento keyword neexistuje pravidlo
                    return ERROR_SYNTAX;
            }
        } else { // pro tento token neexistuje pravidlo
            return ERROR_SYNTAX;
        }

        // vraceni cteni pred identifikator/ keyword, aby si to precetl volajici
        fsetpos(sourceFile, &lastReadPos);

        // rozvinuti neterminalu fnc_body
        if ((error = fnc_body(token, sourceFile)))
            return error;

        // rozvinuti neterminalu return_void
        if ((error = return_void(token, sourceFile)))
            return error;
    }

    return ERROR_PASSED;
} // fnc_def2


/**
 * @brief Neterminal params_def.
 *
 * Implementuje pravidlo 30 a 31.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int params_def(Token *token, FILE *sourceFile) {
    int error;

    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);

    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID == TOKEN_ID_RBR) { // ')'
        // nastavim cteni pred zavorku, aby si ji precetl volajici
        fsetpos(sourceFile, &lastReadPos);
        return ERROR_PASSED; // aplikace pravidla 30
    } else if (token->ID == TOKEN_ID_ID) { // id_var
        // nastavim cteni pred identifikator, aby si ho precetl volany
        fsetpos(sourceFile, &lastReadPos);
    } else
        return ERROR_SYNTAX; // pro tento token neexistuje pravidlo

    // rozsireni neterminalu var_def
    if ((error = var_def(token, sourceFile))) // aplikace pravidla 31
        return error;

    // rozsireni neterminalu params_def2
    return params_def2(token, sourceFile);
}


/**
 * @brief Neterminal params_def2.
 *
 * Implementuje pravidlo 32 a 33.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int params_def2(Token *token, FILE *sourceFile) {
    int error;

    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);

    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID == TOKEN_ID_RBR) { // ')'
        // nastavim cteni pred zavorku, aby si ji precetl volajici
        fsetpos(sourceFile, &lastReadPos);
        return ERROR_PASSED; // aplikace pravidla 32
    } else if (token->ID == TOKEN_ID_CMA) { // ','
        // aplikace pravidla 33
        // (hned za koncem podminky)
    } else { // pro tento token neexistuje pravidlo
        return ERROR_SYNTAX;
    }

    // rozvinuti neterminalu var_def
    if ((error = var_def(token, sourceFile)))
        return error;

    // znovu rozvinuti neterminalu params_def2
    return params_def2(token, sourceFile);
}


/**
 * @brief Neterminal var_def.
 *
 * Implementuje pravidlo 34.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int var_def(Token *token, FILE *sourceFile) {
    int error;
    // aplikace pravidla 34

    // id_var
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_ID)
        return ERROR_SYNTAX;

    // overeni, ze identifikator zatim neexistuje
    if (bst_search(symTable->TopLocalElement->node, token->Value.string) != NULL)
        return ERROR_SEM_UNDEFINED;
    // vlozeni identifikatoru do symtable
    bst_insert(&(symTable->TopLocalElement->node), token->Value.string, false);


    // ':'
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_CLN)
        return ERROR_SYNTAX;

    // rozvinuti neterminalu data_type
    return data_type(token, sourceFile);
}


/**
 * @brief Neterminal return.
 *
 * Implementuje pravidlo 35.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int return_(Token *token, FILE *sourceFile) {
    int error;
    // aplikace pravidla 35

    // return
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_KEYWORD)
        return ERROR_SYNTAX;
    else if (token->Value.keyword != KEYWORD_RETURN)
        return ERROR_SYNTAX;

    // volani bottom-up SA (rozsireni neterminalu expr)
    if ((error = exprSyntaxCheck(token, sourceFile, symTable)))
        return error;

    return ERROR_PASSED;
}


/**
 * @brief Neterminal return_void.
 *
 * Implementuje pravidlo 36 a 37.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int return_void(Token *token, FILE *sourceFile) {
    int error;

    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);

    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_KEYWORD) // pro token neexistuje pravidlo
        return ERROR_SYNTAX;

    if (token->Value.keyword == KEYWORD_END) {    // end
        // nastavim cteni pred end, aby si ji precetl volajici
        fsetpos(sourceFile, &lastReadPos);
        return ERROR_PASSED; // aplikace pravidla 37
    } else if (token->Value.keyword == KEYWORD_RETURN) {
        // aplikace pravidla 36
    } else { // pro prijaty token neexistuje pravidlo
        return ERROR_SYNTAX;
    }

    return ERROR_PASSED;
}


/**
 * @brief Neterminal fnc_body.
 *
 * Implementuje pravidlo 38, 39 a 40 a 41.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int fnc_body(Token *token, FILE *sourceFile) {
    int error;

    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);

    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID == TOKEN_ID_KEYWORD) { // local, if, while, return, nebo end
        switch (token->Value.keyword) {
            case KEYWORD_LOCAL: // local
                // vratim cteni pred local, aby si ho precetl volany
                fsetpos(sourceFile, &lastReadPos);

                // rozvinu neterminal statement
                if ((error = statement(token, sourceFile))) // aplikace pravidla 40
                    return error;

                break;

            case KEYWORD_IF: // if
                // rozvinu neterminal if
                if ((error = if_(token, sourceFile))) // aplikace pravidla 38
                    return error;
                break;


            case KEYWORD_WHILE: // while
                // rozvinu neterminal while
                if ((error = loop(token, sourceFile))) // aplikace pravidla 39
                    return error;
                break;

            case KEYWORD_END: // end
            case KEYWORD_RETURN: // return
                // vratim cteni pred keyword, aby si ho precetl volajici
                fsetpos(sourceFile, &lastReadPos);

                return ERROR_PASSED; // aplikace pravidla 41
                break;

            default: // pro keyword neexistuje pravidlo
                return ERROR_SYNTAX;
        }
    } else if (token->ID == TOKEN_ID_ID) {
        // vratim cteni pred identifikator, aby si ho precetl volany
        fsetpos(sourceFile, &lastReadPos);

        // rozvinu neterminal statement
        if ((error = statement(token, sourceFile))) // aplikace pravidla 40
            return error;
    } else { // pro prijaty token neexistuje pravidlo
        return ERROR_SYNTAX;
    }

    return fnc_body(token, sourceFile);
} // fnc_body


/**
 * @brief Neterminal statement.
 *
 * Implementuje pravidlo 42, 43 a 44.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int statement(Token *token, FILE *sourceFile) {
    int error;

    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);

    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID == TOKEN_ID_KEYWORD) // local
        // rozvinuti neterminalu var_dec
        return var_dec(token, sourceFile); // aplikace pravidla 42
    else if (token->ID == TOKEN_ID_ID) { // id_var nebo id_fnc
        // najdu uzel identifikatoru
        bst_node_t *id_node = search_Iden(token->Value.string, symTable);

        // zjistim, jestli existuje
        if (id_node == NULL)
            return ERROR_SEM_UNDEFINED;
        if (isFnc(id_node)) { // id_fnc
            // vratim cteni pred identifikator, aby si ho precetl volany
            fsetpos(sourceFile, &lastReadPos);
            // rozsirim neterminal fnc_call
            return fnc_call(token, sourceFile); // aplikace pravidla 44
        } else { // id_var
            // aplikace pravidla 42
        }
    } else // pro prijaty token neexistuje pravidlo
        return ERROR_SYNTAX;

    // z podminek pokracuje dal uz jen pravidlo 43
    // '='
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;
    else if (token->ID != TOKEN_ID_ASSIGN)
        return ERROR_SYNTAX;

    // rozvinu neterminal var_assign
    return var_assign(token, sourceFile);
}


/**
 * @brief Neterminal var_dec.
 *
 * Implementuje pravidlo 45.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int var_dec(Token *token, FILE *sourceFile) {
    // aplikace pravidla 45

    int error;
    // local bylo uz precteno volajicim

    // rozvinuti neterminalu var_def
    if ((error = var_def(token, sourceFile)))
        return error;

    // rozvinuti neterminalu var_dec_init
    return var_dec_init(token, sourceFile);
}


/**
 * @brief Neterminal var_dec_init.
 *
 * Implementuje pravidlo 46 a 47.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int var_dec_init(Token *token, FILE *sourceFile) {
    int error;

    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);

    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID == TOKEN_ID_ASSIGN) { // '='
        // aplikace pravidla 46

        // rozvinuti neteminalu var_assign
        return var_assign(token, sourceFile);
    } else if (token->ID == TOKEN_ID_KEYWORD) { // local, if, while, return nebo end
        switch (token->Value.keyword) {
            case KEYWORD_LOCAL:
            case KEYWORD_IF:
            case KEYWORD_ELSE:
            case KEYWORD_WHILE:
            case KEYWORD_RETURN:
            case KEYWORD_END:
                // aplikace pravidla 47
                break;

            default: // prot tento keyword neexisuje pravidlo
                return ERROR_SYNTAX;
        }
    } else if (token->ID == TOKEN_ID_ID) { // id_var nebo id_fce
        // aplikace pravidla 47
    } else {
        // pro tento token neexistuje pravidlo
        return ERROR_SYNTAX;
    }

    // vratim cteni pred identifikator, aby si ho precetl volany
    fsetpos(sourceFile, &lastReadPos);
    return ERROR_PASSED;
}


/**
 * @brief Neterminal var_assign.
 *
 * Implementuje pravidlo 48 a 49.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int var_assign(Token *token, FILE *sourceFile) {
    int error;

    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);

    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    // uzel s pripadnym identifikatorem
    bst_node_t *id;
    switch (token->ID) {
        case TOKEN_ID_ID: // id_var nebo var_fce
            // vyhledam identifikator v tabulce
            id = search_Iden(token->Value.string, symTable);
            // nastaveni cteni pred identifikator, aby si to precetl volany
            fsetpos(sourceFile, &lastReadPos);

            if (id == NULL) // identifikator neexistuje
                return ERROR_SEM_UNDEFINED;
            else if (isFnc(id)) // id_fnc
                // rozvinuti neterminalu fnc_call
                error = fnc_call(token, sourceFile); // aplikace pravidla 49
            else // id_var
                // zavolani bottomup SA pro neterminal expr
                error = exprSyntaxCheck(token, sourceFile, symTable); // aplikace pravidla 48
            break;

        case TOKEN_ID_RBR: // ')'
        case TOKEN_ID_LEN: // '#'
        case TOKEN_ID_INT0:
        case TOKEN_ID_HEX2:
        case TOKEN_ID_INT:
        case TOKEN_ID_ZERO: // integer
        case TOKEN_ID_DHEX2:
        case TOKEN_ID_HEXP3:
        case TOKEN_ID_DBL2:
        case TOKEN_ID_EXP3: // number
        case TOKEN_ID_FSTR: // string
            // vratim cteni pred token, aby si ho precetl volany
            fsetpos(sourceFile, &lastReadPos);

            //aplikace pravidla 48
            // volani bottom-up SA (rozsireni neterminalu expr)
            error = exprSyntaxCheck(token, sourceFile, symTable);
            break;

        case TOKEN_ID_KEYWORD:
            if (token->Value.keyword == KEYWORD_NIL) { // nil
                // vratim cteni pred nil, aby si ho precetl volany
                fsetpos(sourceFile, &lastReadPos);

                //aplikace pravidla 48
                // volani bottom-up SA (rozsireni neterminalu expr)
                error = exprSyntaxCheck(token, sourceFile, symTable);
            } else // pro tento keyword neexistuje pravidlo
                return ERROR_SYNTAX;
            break;

        default: // pro tento token neexistuje pravidlo
            return ERROR_SYNTAX;
    }

    return error;
} // var_assign


/**
 * @brief Neterminal if
 *
 * Implementuje pravidlo 50.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int if_(Token *token, FILE *sourceFile) {
    int error;
    // token if byl precten o uroven vyse => pokracuju dal
    // aplikace pravidla 50

    // volani bottom-up SA (rozsireni neterminalu expr)
    if ((error = exprSyntaxCheck(token, sourceFile, symTable)))
        return error;

    // then
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_KEYWORD)
        return ERROR_SYNTAX;
    else if (token->Value.keyword != KEYWORD_THEN)
        return ERROR_SYNTAX;

    // vytvorim ramec pro blok if
    SLL_Frame_Insert(symTable);

    // rozvinu neterminal statements
    if ((error = statements(token, sourceFile)))
        return error;

    // odstranim ramec pro blok if
    SLL_Frame_Delete(symTable);

    // else
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_KEYWORD)
        return ERROR_SYNTAX;
    else if (token->Value.keyword != KEYWORD_ELSE)
        return ERROR_SYNTAX;

    // vytvorim ramec pro blok else
    SLL_Frame_Insert(symTable);

    // rozvinu neterminal statements
    if ((error = statements(token, sourceFile)))
        return error;

    // odstranim ramec pro blok else
    SLL_Frame_Delete(symTable);

    // end
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_KEYWORD)
        return ERROR_SYNTAX;
    else if (token->Value.keyword != KEYWORD_END)
        return ERROR_SYNTAX;

    return ERROR_PASSED;
} // if_


/**
 * @brief Neterminal loop
 *
 * Implementuje pravidlo 51.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int loop(Token *token, FILE *sourceFile) {
    int error;
    // token while byl precten o uroven vyse => pokracuju dal
    // aplikace pravidla 51

    // volani bottom-up SA (rozsireni neterminalu expr)
    if ((error = exprSyntaxCheck(token, sourceFile, symTable)))
        return error;

    // do
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_KEYWORD)
        return ERROR_SYNTAX;
    else if (token->Value.keyword != KEYWORD_DO)
        return ERROR_SYNTAX;

    // vytvorim ramec pro blok do
    SLL_Frame_Insert(symTable);

    // rozvinu neterminal statements
    if ((error = statements(token, sourceFile)))
        return error;

    // vytvorim ramec pro blok do
    SLL_Frame_Delete(symTable);

    // end
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_KEYWORD)
        return ERROR_SYNTAX;
    else if (token->Value.keyword != KEYWORD_END)
        return ERROR_SYNTAX;

    return ERROR_PASSED;
} // loop


/**
 * @brief Neterminal statements
 *
 * Implementuje pravidlo 52 a 53.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int statements(Token *token, FILE *sourceFile) {
    int error;

    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);

    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID == TOKEN_ID_ID) { // id_var nebo id_fce
        // nastavim cteni pred identifikator, aby si ho precetl volany
        fsetpos(sourceFile, &lastReadPos);
    } else if (token->ID == TOKEN_ID_KEYWORD) {
        switch (token->Value.keyword) { // end, else nebo local
            case KEYWORD_ELSE:
            case KEYWORD_END:
                // nastavim cteni pred keyword, aby si to precetl volajici
                fsetpos(sourceFile, &lastReadPos);
                return ERROR_PASSED; // aplikace pravidla 52

            case KEYWORD_LOCAL:
                // nastavim cteni pred keyword, aby si to precetl volajici
                fsetpos(sourceFile, &lastReadPos);
                break;

            default: // pro prijaty token neexistuje pravidlo
                return ERROR_SYNTAX;
        }
    } else // pro prijaty token neexistuje zadne pravidlo
        return ERROR_SYNTAX;

    // rozvinuti neterminalu statement
    if ((error = statement(token, sourceFile))) // aplikace pravidla 53
        return error;

    return statements(token, sourceFile);
}


/**
 * @brief Parser
 *
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int parser(FILE *sourceFile) {
    // vytvoreni struktury token, kterou bude scanner plnit
    Token *token = (Token *) malloc(sizeof(Token));
    if (token == NULL)
        return ERROR_COMPILER;

    // volani prvniho pravidla a nahrazovani prvniho neterminalu
    int error = start(token, sourceFile);

    // TODO free(token->Value.string);
    free(token);
    return error;
} 
