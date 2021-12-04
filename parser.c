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
#include "expression.h"

#define VAR_TYPE 0
#define PARAM_TYPE 1
#define RET_TYPE 2


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
    /* nacteni vestavenych funkci */
    // dynamic string pro jmena vestavenych funkci
    Dynamic_string *str = (Dynamic_string *) malloc(sizeof(Dynamic_string));

    // write()
    str->str = "write";
    error = bst_insert(&(symTable->globalElement->node), str, true);

    // reads()
    if(!error) {
        str->str = "reads";
        error = setBuildInFun(symTable, str, TYPE_UNDEFINED, TYPE_UNDEFINED, TYPE_UNDEFINED, TYPE_STRING);
    }

    // readi()
    if(!error) {
        str->str = "readi";
        error = setBuildInFun(symTable, str, TYPE_UNDEFINED, TYPE_UNDEFINED, TYPE_UNDEFINED, TYPE_INTEGER);
    }

    // readn()
    if(!error) {
        str->str = "readn";
        error = setBuildInFun(symTable, str, TYPE_UNDEFINED, TYPE_UNDEFINED, TYPE_UNDEFINED, TYPE_NUMBER);
    }

    // tointeger()
    if(!error) {
        str->str = "tointeger";
        error = setBuildInFun(symTable, str, TYPE_NUMBER, TYPE_UNDEFINED, TYPE_UNDEFINED, TYPE_INTEGER);
    }

    // substr()
    if(!error) {
        str->str = "substr";
        error = setBuildInFun(symTable, str, TYPE_STRING, TYPE_NUMBER, TYPE_NUMBER, TYPE_STRING);
    }

    // ord()
    if(!error) {
        str->str = "ord";
        error = setBuildInFun(symTable, str, TYPE_STRING, TYPE_INTEGER, TYPE_UNDEFINED, TYPE_INTEGER);
    }

    // chr()
    if(!error) {
        str->str = "chr";
        error = setBuildInFun(symTable, str, TYPE_INTEGER, TYPE_UNDEFINED, TYPE_UNDEFINED, TYPE_STRING);
    }

    free(str);
    if(error) { // doslo k chybe prirazeni pameti
        SLL_Frame_Dispose(symTable);
        free(symTable);
        return error;
    }

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

    bst_node_t *node_idFnc = NULL;

    // overeni, zda nebyla funkce uz deklarovana nebo definovana
    if (search_Iden(token->Value.string, symTable) != NULL)
        return ERROR_SEM_UNDEFINED;
    // pridani id_fnc do symtable
    if((error = bst_insert(&(symTable->globalElement->node), token->Value.string, true)))
        return error;

    // ulozim si uzel id_fnc, abych vedel komu pridavat parametry a navratovou hodnotu
    node_idFnc = bst_search(symTable->globalElement->node, token->Value.string);
    setFncDec(node_idFnc, false);
    setFncDef(node_idFnc, false);

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
    if ((error = params_dec(token, sourceFile, node_idFnc))) {
        return error;
    }

    // ')'
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_RBR)
        return ERROR_SYNTAX;


    // rozvinuti neterminalu return_type
    if ((error = return_type(token, sourceFile, node_idFnc)))
        return error;

    setFncDec(node_idFnc, true);

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
int params_dec(Token *token, FILE *sourceFile, bst_node_t *node_idFnc) {
    int error;

    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    // rozvetveni na ruzna pravidla podle hodnoty tokenu
    switch (token->ID) {
        case TOKEN_ID_RBR: // ')'
            // vratim cteni pred zavorku, aby ji mohl precist volajici
            fsetpos(sourceFile, &lastReadPos);
            return ERROR_PASSED; // aplikace pravidla 7

        case TOKEN_ID_KEYWORD: // integer, number nebo string
            if (token->Value.keyword == KEYWORD_INTEGER || token->Value.keyword == KEYWORD_NUMBER ||
                token->Value.keyword == KEYWORD_STRING) {

                // vratim cteni pred keyword, aby ji mohl precist volany
                fsetpos(sourceFile, &lastReadPos);

                // rozvinuti neterminalu data_type
                // TODO pridat jeden parametr
                if ((error = data_type(token, sourceFile, node_idFnc, NULL, PARAM_TYPE))) // aplikace pravidla 8
                    return error;
            } else // prijate keyword nelze pouzit - pravidlo neexistuje
                return ERROR_SYNTAX;
            break;

        default: // pro token zadne pravidlo neexistuje
            return ERROR_SYNTAX;
    }

    return params_dec2(token, sourceFile, node_idFnc);
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
int params_dec2(Token *token, FILE *sourceFile, bst_node_t *node_idFnc) {
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
        // TODO pridat jeden parametr
        if ((error = data_type(token, sourceFile, node_idFnc, NULL, PARAM_TYPE))) // aplikace pravidla 10
            return error;
    } else {
        return ERROR_SYNTAX;
    }

    return params_dec2(token, sourceFile, node_idFnc);
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
int return_type(Token *token, FILE *sourceFile, bst_node_t *node_idFnc) {
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
            // aplikace pravidla 11
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

    return data_type(token, sourceFile, node_idFnc, NULL, RET_TYPE); // aplikace pravidla 11
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
int data_type(Token *token, FILE *sourceFile, bst_node_t *node_id, SLLElementPtr_Param param, int dataSwitch) {
    int error;
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_KEYWORD) // nevalidni token
        return ERROR_SYNTAX;

    // rozvetveni na ruzna pravidla podle hodnoty tokenu
    switch (token->Value.keyword) {
        case KEYWORD_INTEGER: // aplikace pravidla 13
            switch (dataSwitch) {
                case VAR_TYPE:
                    setVarType(node_id, TYPE_INTEGER);
                    break;

                case PARAM_TYPE:
                    if(!isDecFnc(node_id))
                        error = SLL_Param_Insert(TYPE_INTEGER, node_id->name, node_id);
                    else // fce byla deklarovana => musim zkontrolovat dat. typy deklarace a definice
                        // TODO vratit odpovidajici chybu
                        error = (param->type == TYPE_INTEGER ? ERROR_PASSED : ERROR_SEM_OTHERS);
                    break;

                case RET_TYPE:
                    if(!isDecFnc(node_id))
                        error = SLL_Return_Insert(TYPE_INTEGER, node_id);
                    else // fce byla deklarovana => musim zkontrolovat dat. typy deklarace a definice
                        error = (node_id->funcData->returnList->firstElement->type == TYPE_INTEGER ? ERROR_PASSED : ERROR_SEM_OTHERS);
                    break;
            }
            break; // KEYWORD_INTEGER

        case KEYWORD_NUMBER: // aplikace pravidla 14
            switch (dataSwitch) {
                case VAR_TYPE:
                    setVarType(node_id, TYPE_NUMBER);
                    break;

                case PARAM_TYPE:
                    if(!isDecFnc(node_id))
                        error = SLL_Param_Insert(TYPE_NUMBER, node_id->name, node_id);
                    else // fce byla deklarovana => musim zkontrolovat dat. typy deklarace a definice
                        error = (param->type == TYPE_NUMBER ? ERROR_PASSED : ERROR_SEM_OTHERS);
                    break;

                case RET_TYPE:
                    if(!isDecFnc(node_id))
                        error = SLL_Return_Insert(TYPE_NUMBER, node_id);
                    else // fce byla deklarovana => musim zkontrolovat dat. typy deklarace a definice
                        error = (node_id->funcData->returnList->firstElement->type == TYPE_NUMBER ? ERROR_PASSED : ERROR_SEM_OTHERS);
                    break;
            } // KEYWORD_NUMBER
            break;

        case KEYWORD_STRING: // aplikace pravidla 15
            switch (dataSwitch) {
                case VAR_TYPE:
                    setVarType(node_id, TYPE_STRING);
                    break;

                case PARAM_TYPE:
                    if(!isDecFnc(node_id))
                        error = SLL_Param_Insert(TYPE_STRING, node_id->name, node_id);
                    else // fce byla deklarovana => musim zkontrolovat dat. typy deklarace a definice
                        error = (param->type == TYPE_STRING ? ERROR_PASSED : ERROR_SEM_OTHERS);
                    break;

                case RET_TYPE:
                    if(!isDecFnc(node_id))
                        error = SLL_Return_Insert(TYPE_STRING, node_id);
                    else // fce byla deklarovana => musim zkontrolovat dat. typy deklarace a definice
                        error = (node_id->funcData->returnList->firstElement->type != TYPE_STRING ? ERROR_PASSED : ERROR_SEM_OTHERS);
                    break;
            } // KEYWORD_STRING
            break;

        default:
            return ERROR_SYNTAX; // nevalidni klicove slovo
    } // switch

    return error;
} // data_type


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

    bst_node_t *node_idFnc = NULL;

    // vyhledani id_fnc v symtable
    node_idFnc = search_Iden(token->Value.string, symTable);
    if(node_idFnc == NULL) // funkce nebyla deklarovana ani definovana
        return ERROR_SEM_UNDEFINED;

    // '('
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_LBR)
        return ERROR_SYNTAX;


    // rozvinuti neterminalu value
    // TODO seznam parametru jako ukazatel na parametr, nebo ukazatel na ukazatel na parametr
    if ((error = value(token, sourceFile, node_idFnc, node_idFnc->funcData->paramList->firstElement)))
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
int value(Token *token, FILE *sourceFile, bst_node_t *node_idFnc, SLLElementPtr_Param param) {
    int error;
    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);

    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    switch (token->ID) {
        case TOKEN_ID_RBR: // ')'
            if(param != NULL) // funkce ocekava parametr, ktery na vstup neprisel
                return ERROR_SEM_TYPE_COUNT;

            // nasteveni cteni pred zavorku, aby si ji precetl volajici
            fsetpos(sourceFile, &lastReadPos);
            return ERROR_PASSED; // aplikace pravidla 17

        case TOKEN_ID_KEYWORD: // nil
            if (token->Value.keyword == KEYWORD_NIL) {
                // nasteveni cteni pred nil, aby si ji precetl volany
                fsetpos(sourceFile, &lastReadPos);
                if(param == NULL) // funkce neocekava dalsi parametr
                    return ERROR_SEM_TYPE_COUNT;
                else // prechod na dalsi parametr v seznamu
                    param = param->nextElement;
                value_last(token, sourceFile, node_idFnc, param); // aplikace pravidla 18
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
            // nastaveni cteni pred hodnotu, aby si ji precetl volany
            fsetpos(sourceFile, &lastReadPos);
            if((error = value_last(token, sourceFile, node_idFnc, param))) // aplikace pravidla 18
                return error;
            break;

        default:
            return ERROR_SYNTAX;
    }

    return value2(token, sourceFile, node_idFnc, param);
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
int value2(Token *token, FILE *sourceFile, bst_node_t *node_idFnc, SLLElementPtr_Param param) {
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
    if ((error = value_last(token, sourceFile, node_idFnc, param)))
        return error;

    return value2(token, sourceFile, node_idFnc, param);
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
int value_last(Token *token, FILE *sourceFile, bst_node_t *node_idFnc, SLLElementPtr_Param param) {
    // overeni, zda funkce jeste hleda parametry
    if(param == NULL)
        return ERROR_SEM_TYPE_COUNT;

    int error;
    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);

    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    bst_node_t *node_id;
    switch (token->ID) {
        case TOKEN_ID_ID: // id_var
            // aplikace pravidla 21
            node_id = search_Iden(token->Value.string, symTable);
            if(isFnc(node_id)) // id_fnc
                return ERROR_SYNTAX;
            setVarUsed(node_id, true);
            // overeni datovych typu
            if(typeVar(node_id) != param->type) {
                if(param->type == TYPE_NUMBER && typeVar(node_id) == TYPE_INTEGER)
                    // pretypovani integer na number
                    node_id->varData->type = TYPE_NUMBER;
                else // promenne nemaji kompatibilni typy
                    return ERROR_SEM_TYPE_COUNT;
            }
            break;

        case TOKEN_ID_INT0:
        case TOKEN_ID_HEX2:
        case TOKEN_ID_INT:
        case TOKEN_ID_ZERO: // int_value
            // aplikace pravidla 22
            if(param->type != TYPE_INTEGER) {
                if(param->type == TYPE_NUMBER) { // ocekavam number, davam mu integer
                    // TODO generovani kodu - int2float
                }
                else // datove typy nejsou kompatibilni
                    return ERROR_SEM_TYPE_COUNT;
            }
            // TODO generovani kodu pro integer
            break;

        case TOKEN_ID_DHEX2:
        case TOKEN_ID_HEXP3:
        case TOKEN_ID_DBL2:
        case TOKEN_ID_EXP3: // num_value
            // aplikace pravidla 23
            if(param->type != TYPE_NUMBER) // datove typy nejsou kompatibilni
                return ERROR_SEM_TYPE_COUNT;
            // TODO generace kodu pro number
            break;

        case TOKEN_ID_FSTR: // str_value
            // aplikace pravidla 24
            if(param->type != TYPE_STRING) // datove typy nejsou kompatibilni
                return ERROR_SEM_TYPE_COUNT;
            // TODO generace kodu pro string
            break;

        case TOKEN_ID_KEYWORD: // nil
            if (token->Value.keyword == KEYWORD_NIL) {
                // aplikace pravidal 25
                // TODO generovani kodu pro nil
            } else // pro keyword neexistuje pravidlo
                return ERROR_SYNTAX;
            break;

        default: // pro token zadne pravidlo neexistuje
            return ERROR_SYNTAX;
    }

    param = param->nextElement;
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

    // ukazatel na uzel s funkci
    bst_node_t *node_idFnc = NULL;

    // rozvinuti neterminalu fnc_head
    if ((error = fnc_head(token, sourceFile, node_idFnc)))
        return error;

    // rozvinuti neterminalu fnc_def2
    if ((error = fnc_def2(token, sourceFile, node_idFnc)))
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

    // oznacim funkci jako definovanou
    // TODO kdy tohle provest?
    // TODO potrebuju promennou isDec? nebo poznam jestli byla fnc deklarovana z isDef?
    // TODO     <- kvuli volani data_type
    setFncDef(node_idFnc, true);

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
int fnc_head(Token *token, FILE *sourceFile, bst_node_t *node_idFnc) {
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
    node_idFnc = search_Iden(token->Value.string, symTable);

    if (node_idFnc == NULL)
        bst_insert(&(symTable->globalElement->node), token->Value.string, true);
    else if (isDecFnc(node_idFnc) && !isDefFnc(node_idFnc)) { // funkce byla uz deklarovana

    } else { // funkce byla uz i definovanna - pokus o redefinici
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
    // TODO seznam parametru jako ukazatel na parametr, nebo ukazatel na ukazatel na parametr
    if(isDecFnc(node_idFnc)) {
        // u deklarovane funkce musim kontrolovat pocet a typ paremtru jeji definice
        if ((error = params_def(token, sourceFile, node_idFnc, node_idFnc->funcData->paramList->firstElement)))
            return error;
    }
    else {
        // nedeklarovana funkce ma prazdny seznam parametru
        if ((error = params_def(token, sourceFile, node_idFnc, NULL)))
            return error;
    }





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
int fnc_def2(Token *token, FILE *sourceFile, bst_node_t *node_idFnc) {
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
        if(isDecFnc(node_idFnc))
            error = data_type(token, sourceFile, node_idFnc, NULL, RET_TYPE);
        else
            error = data_type(token, sourceFile, node_idFnc, NULL, RET_TYPE);

        if (error)
            return error;

        // rozvinuti neterminalu fnc_body
        if ((error = fnc_body(token, sourceFile)))
            return error;

        // rozvinuti neterminalu return
        if ((error = return_(token, sourceFile, node_idFnc->funcData->returnList->firstElement->type)))
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
int params_def(Token *token, FILE *sourceFile, bst_node_t *node_idFnc, SLLElementPtr_Param param) {
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
    } else // pro tento token neexistuje pravidlo
        return ERROR_SYNTAX;

    // rozsireni neterminalu var_def
    if ((error = var_defParam(token, sourceFile, node_idFnc, param))) // aplikace pravidla 31
        return error;

    // rozsireni neterminalu params_def2
    return params_def2(token, sourceFile, node_idFnc, param);
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
int params_def2(Token *token, FILE *sourceFile, bst_node_t *node_idFnc, SLLElementPtr_Param param) {
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
        // aplikace pravidla 33 (hned za koncem podminky)
    } else { // pro tento token neexistuje pravidlo
        return ERROR_SYNTAX;
    }

    // rozvinuti neterminalu var_def
    // TODO jak rozlisit var_def pro definici parametru funkce a pro deklaraci promenne
    if ((error = var_defParam(token, sourceFile, node_idFnc, param)))
        return error;

    // znovu rozvinuti neterminalu params_def2
    return params_def2(token, sourceFile, node_idFnc, param);
}

/**
 * @brief Neterminal var_def.
 *
 * Totozna s var_def, ale kontroluje semantiku pro promenne v seznamu parametru
 *
 * Implementuje pravidlo 34.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int var_defParam(Token *token, FILE *sourceFile, bst_node_t *node_idFnc, SLLElementPtr_Param param) {
    int error;
    // aplikace pravidla 34

    // id_var
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_ID)
        return ERROR_SYNTAX;

    // kontrola poctu parametru u deklarace x definice
    if (isDecFnc(node_idFnc) && param == NULL) // funkce neocekava dalsi parametry
        return ERROR_SEM_TYPE_COUNT;

    // uzel s id pridavaneho parametru
    bst_node_t *node_idVar = NULL;

    // overeni, ze identifikator zatim neexistuje
    if (search_Iden(token->Value.string, symTable) != NULL)
        return ERROR_SEM_UNDEFINED;
    // vlozeni identifikatoru do symtable
    bst_insert(&(symTable->TopLocalElement->node), token->Value.string, false);

    node_idVar = bst_search(symTable->TopLocalElement->node, token->Value.string);

    // ':'
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_CLN)
        return ERROR_SYNTAX;


    if (isDecFnc(node_idFnc)) {
        // funkce je deklarovana => kontrola dat. typu parametru v deklaraci a definici
        if((error = data_type(token, sourceFile, node_idFnc, param, PARAM_TYPE)))
            return error;
        param = param->nextElement;
    }
    else {
        // funkce neni deklarovana => ulozeni dat.typu parametru k id funkce
        if((error = data_type(token, sourceFile, node_idFnc, NULL, PARAM_TYPE)))
            return error;
    }
    // ulozeni datoveho typu k id promenne predstavujici parametr
    return data_type(token, sourceFile, node_idVar, NULL, VAR_TYPE);
} // var_defParam


/**
 * @brief Neterminal var_def.
 *
 * Implementuje pravidlo 34.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int var_def(Token *token, FILE *sourceFile, bst_node_t *node_idFnc) {
    // TODO jak rozlisit var_def pro definici parametru funkce a pro deklaraci promenne

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

    node_idFnc = bst_search(symTable->TopLocalElement->node, token->Value.string);

    // ':'
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_CLN)
        return ERROR_SYNTAX;

    // rozvinuti neterminalu data_type
    return data_type(token, sourceFile, node_idFnc, NULL, VAR_TYPE);
} // var_def


/**
 * @brief Neterminal return.
 *
 * Implementuje pravidlo 35.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int return_(Token *token, FILE *sourceFile, Data_type var_dataType) {
    int error;
    // aplikace pravidla 35

    // return
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_KEYWORD)
        return ERROR_SYNTAX;

    if(token->Value.keyword == KEYWORD_RETURN) { // return
        // volani bottom-up SA (rozsireni neterminalu expr)
        return exprSyntaxCheck(token, sourceFile, symTable, var_dataType);
    }
    else if (token->Value.keyword == KEYWORD_END) {
        // funkce ma vratit hodnotu. token return neprisel, ale prisel end => vrati hodnotu nil
        // TODO generovani kodu - vratit hodnotu nil
        return ERROR_PASSED;
    }
    else // pro keyword neexistuje pravidlo
        return ERROR_SYNTAX;
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
 * Implementuje pravidlo 38 a 39.
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
            case KEYWORD_IF: // if
            case KEYWORD_WHILE: // while
                // vratim cteni pred keyword, aby si ho precetl volany
                fsetpos(sourceFile, &lastReadPos);

                // rozvinu neterminal statement
                if ((error = statement(token, sourceFile))) // aplikace pravidla 38
                    return error;

                break;

            case KEYWORD_END: // end
            case KEYWORD_RETURN: // return
                // vratim cteni pred keyword, aby si ho precetl volajici
                fsetpos(sourceFile, &lastReadPos);

                return ERROR_PASSED; // aplikace pravidla 39
                break;

            default: // pro keyword neexistuje pravidlo
                return ERROR_SYNTAX;
        }
    } else if (token->ID == TOKEN_ID_ID) {
        // vratim cteni pred identifikator, aby si ho precetl volany
        fsetpos(sourceFile, &lastReadPos);

        // rozvinu neterminal statement
        if ((error = statement(token, sourceFile))) // aplikace pravidla 38
            return error;
    } else { // pro prijaty token neexistuje pravidlo
        return ERROR_SYNTAX;
    }

    return fnc_body(token, sourceFile);
} // fnc_body


/**
 * @brief Neterminal statement.
 *
 * Implementuje pravidlo 40, 41, 42, 43 a 44.
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

    // ukazatel na uzel s pripadnym identifikatorem
    bst_node_t *id_node = NULL;

    if (token->ID == TOKEN_ID_KEYWORD) { // local, if nebo while
        switch (token->Value.keyword) {
            case KEYWORD_LOCAL: // local
                // rozvinuti neterminalu var_dec
                error = var_dec(token, sourceFile); // aplikace pravidla 40
                break;

            case KEYWORD_IF: // if
                // rozvinuti neterminalu if
                error = if_(token, sourceFile); // aplikace pravidla 43
                break;

            case KEYWORD_WHILE: // while
                // rozvinuti neterminalu loop
                error = loop(token, sourceFile); // aplikace pravidla 44
                break;

            default: // pro keyword neexistuje pravidlo
                error = ERROR_SYNTAX;
                break;
        }
        return error;
    }
    else if (token->ID == TOKEN_ID_ID) { // id_var nebo id_fnc
        // najdu uzel identifikatoru
        id_node = search_Iden(token->Value.string, symTable);

        // zjistim, jestli identifikator existuje
        if (id_node == NULL)
            return ERROR_SEM_UNDEFINED;
        if (isFnc(id_node)) { // id_fnc
            // vratim cteni pred identifikator, aby si ho precetl volany
            fsetpos(sourceFile, &lastReadPos);
            // rozsirim neterminal fnc_call
            return fnc_call(token, sourceFile); // aplikace pravidla 42
        } else { // id_var
            // aplikace pravidla 41 //TODO tady jsem skoncil

        }
    } else // pro prijaty token neexistuje pravidlo
        return ERROR_SYNTAX;

    // z podminek pokracuje dal uz jen pravidlo 41
    // '='
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;
    else if (token->ID != TOKEN_ID_ASSIGN)
        return ERROR_SYNTAX;

    // rozvinu neterminal var_assign
    error = var_assign(token, sourceFile, typeVar(id_node));
    setVarInit(id_node, true);

    return error;
} // statement


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

    // ukazatel na uzel s promennou
    bst_node_t *node_idVar = NULL;

    // rozvinuti neterminalu var_def
    if ((error = var_def(token, sourceFile, node_idVar)))
        return error;

    // rozvinuti neterminalu var_dec_init
    return var_dec_init(token, sourceFile, node_idVar);
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
int var_dec_init(Token *token, FILE *sourceFile, bst_node_t *node_idVar) {
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
        error = var_assign(token, sourceFile, node_idVar->varData->type);
        // oznacim promennou z vyssi urovne za inicializovanou
        setVarInit(node_idVar, true);
        return error;
    } else if (token->ID == TOKEN_ID_KEYWORD) { // local, if, while, return nebo end
        switch (token->Value.keyword) {
            case KEYWORD_LOCAL:
            case KEYWORD_IF:
            case KEYWORD_ELSE:
            case KEYWORD_WHILE:
            case KEYWORD_RETURN:
            case KEYWORD_END:
                // aplikace pravidla 47
                setVarInit(node_idVar, false);
                break;

            default: // prot tento keyword neexisuje pravidlo
                return ERROR_SYNTAX;
        }
    } else if (token->ID == TOKEN_ID_ID) // id_var nebo id_fce
        // aplikace pravidla 47
        setVarInit(node_idVar, false);
    else {
        // pro tento token neexistuje pravidlo
        return ERROR_SYNTAX;
    }

    // vratim cteni pred identifikator, aby si ho precetl volany
    fsetpos(sourceFile, &lastReadPos);
    return ERROR_PASSED;
} // var_dec_init


/**
 * @brief Neterminal var_assign.
 *
 * Implementuje pravidlo 48 a 49.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int var_assign(Token *token, FILE *sourceFile, Data_type varType) {
    int error;

    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);

    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    // uzel s pripadnym identifikatorem (za rovnitkem)
    bst_node_t *id;
    switch (token->ID) {
        case TOKEN_ID_ID: // id_var nebo var_fce
            // vyhledam identifikator v tabulce
            id = search_Iden(token->Value.string, symTable);
            // nastaveni cteni pred identifikator, aby si to precetl volany
            fsetpos(sourceFile, &lastReadPos);

            if (id == NULL) // identifikator neexistuje
                return ERROR_SEM_UNDEFINED;
            else if (isFnc(id)) { // id_fnc
                // rozvinuti neterminalu fnc_call
                if((error = fnc_call(token, sourceFile))) // aplikace pravidla 49
                    return error;
                // TODO generovani kodu - prirazeni promenne navratovou hodnotu funkce
            }
            else { // id_var
                // zavolani bottomup SA pro neterminal expr
                if((error = exprSyntaxCheck(token, sourceFile, symTable, varType))) // aplikace pravidla 48
                    return error;
                // TODO generovani kodu - prirazeni promenne vysledek vyrazu
            }
            break;

        case TOKEN_ID_LBR: // '('
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
            if((error = exprSyntaxCheck(token, sourceFile, symTable, varType))) // aplikace pravidla 48
                return error;
            break;

        case TOKEN_ID_KEYWORD:
            if (token->Value.keyword == KEYWORD_NIL) { // nil
                // vratim cteni pred nil, aby si ho precetl volany
                fsetpos(sourceFile, &lastReadPos);

                //aplikace pravidla 48
                // volani bottom-up SA (rozsireni neterminalu expr)
                if((error = exprSyntaxCheck(token, sourceFile, symTable, varType))) // aplikace pravidla 48
                    return error;
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
    if ((error = exprSyntaxCheck(token, sourceFile, symTable, TYPE_UNDEFINED)))
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
    if ((error = exprSyntaxCheck(token, sourceFile, symTable, TYPE_UNDEFINED)))
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
