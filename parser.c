/**
 * Projekt: IFJ2021
 * 
 * @brief Implementace top-down syntakticke anylzy shora dolu.
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
int ifCounter = 0;
int loopCounter = 0;

/**
 * @brief Nacita tokeny dokud je v nich nektery bily znak, nebo az se narazi na EOF.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 *
 * @return Typ erroru generovany analyzou
*/
int get_non_white_token(Token *token, FILE *sourceFile) {
    int error = ERROR_PASSED;
    do {
        // nacteni noveho tokenu a kontrola vysupu scanneru
        if ((error = get_token(token, sourceFile)) != ERROR_PASSED)
            // lexikalni nebo kompilatorova chyba
            return error;
    } while (token->ID != TOKEN_ID_EOF && (token->ID == TOKEN_ID_SPACE || token->ID == TOKEN_ID_EOL ||
                token->ID == TOKEN_ID_TAB || token->ID == TOKEN_ID_LCMT2 || token->ID == TOKEN_ID_BCMT4));
    return error;
} // get_non_white_token


/**
 * @brief Kontrolue, zda vsechny deklarovane funkce byly definovany.
 *
 * @param func Koren stromu globalniho ramce
 * @return Error, ktery funkce vygeneruje
 */
int checkDeclaredFncs(bst_node_t *func) {
    if(func == NULL)
        return ERROR_PASSED;
    else if(!isDefFnc(func))
        return ERROR_SEM_UNDEFINED;
    else {
        int error = ERROR_PASSED;
        if((error = checkDeclaredFncs(func->left)))
            return error;
        else
            return checkDeclaredFncs(func->right);
    }
} // checkDeclaredFncs


/**
 *  @brief Zpracovava parametry volane vestavene funkce write()
 *
 * @return Typ chyby, ktera nastane
 */
int writeFncCall(Token *token, FILE *sourceFile) {
    int error = ERROR_PASSED;
    error = get_non_white_token(token, sourceFile);
    while (token->ID != TOKEN_ID_RBR) {
        if (token->ID == TOKEN_ID_ID) {
            // vyhledam promennou
            bst_node_t *node_idVar = search_Iden(token->Value.string, symTable);
            if(node_idVar == NULL || isFnc(node_idVar)) // promenna neexistuje, nebo jde o funkci
                return ERROR_SEM_UNDEFINED;

            //printf("CALL $makeWRITE\n");
            makeWriteCall(token->Value.string);
            //printf("WRITE TF@_%s\n", token->Value.string->str);
            //call_write(token->Value.string);
        } else if (token->ID == TOKEN_ID_FSTR) {
            printf("PUSHFRAME\n");
            char *output = converString(token->Value.string->str);
            call_write(output);
            free(output);
            printf("POPFRAME\n");
        }
        error = get_non_white_token(token, sourceFile);
        if (token->ID == TOKEN_ID_RBR)
            break;
        if (token->ID == TOKEN_ID_CMA)
            error = get_non_white_token(token, sourceFile);
        if (token->ID == TOKEN_ID_CMA || token->ID == TOKEN_ID_RBR)
            return ERROR_SYNTAX;
    }
    return error;
} // writeFncCall

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
    int error = ERROR_PASSED;
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
        return ERROR_SEM_OTHERS;

    // nainicializovani symtable
    symTable = (SLList_Frame *) malloc(sizeof(SLList_Frame));
    SLL_Frame_Init(symTable);

    if (symTable == NULL)
        return ERROR_COMPILER;
    // vytvoreni globalniho ramce
    SLL_Frame_Insert(symTable);

    // vlozeni vestavenych funkci do symtable
    error = setBuildInFuns(symTable->globalElement);

    if(error) { // doslo k chybe prirazeni pameti
        SLL_Frame_Dispose(symTable);
        free(symTable);
        return error;
    }

    printf(".IFJcode21\n");
    printf("DEFVAR GF@&varFloat\n");
    printf("DEFVAR GF@&varBool\n");
    printf("DEFVAR GF@&if\n");
    printf("DEFVAR GF@&varType\n");
    printf("DEFVAR GF@*return\n");
    printf("MOVE GF@*return nil@nil\n");
    printf("DEFVAR GF@&desAct\n");
    printf("DEFVAR GF@&desPrev\n");
    make_WRITE_TF();
    //printf("JUMP $$main\n");

    make_write();
    make_reads();
    make_readi();
    make_readn();
    make_substr();
    make_toInteger();
    make_ord();
    make_chr();
    make_NILcompare();
    printf("JUMP $$distributeEnd\n");
    printf("LABEL $distrbute\n");
    printf("JUMPIFEQ !disCheck GF@&desAct GF@&desPrev\n");
    printf("MOVE GF@&desPrev GF@&desAct\n");
    printf("LABEL !disCheck\n");
    printf("RETURN\n");
    printf("LABEL $$distributeEnd\n");

    // vse korektni - uplatnuju pravidlo a rozsiruju dalsi neterminal
    error = program(token, sourceFile); // aplikace pravidla 1

    if(error == ERROR_PASSED)
        // kontrola, zda vsechny deklarovane funkce byly definovany
        error = checkDeclaredFncs(symTable->globalElement->node);


    // uvoleni symTable
    SLL_Frame_Dispose(symTable);
    free(symTable);
    return error;
} // start

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
    int error = ERROR_PASSED;

    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);

    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    // uzel s pripadnou volanou funkci
    bst_node_t *node_idFnc = NULL;

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
            // kontrola, ze funkce nevraci hodnotu
            node_idFnc = bst_search(symTable->globalElement->node, token->Value.string);
            if(node_idFnc != NULL && (isDecFnc(node_idFnc) || isDefFnc(node_idFnc))&& node_idFnc->funcData->returnList->firstElement != NULL)
                return ERROR_SEM_TYPE_COUNT;


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
    int error = ERROR_PASSED;

    // id_fnc
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_ID)
        return ERROR_SYNTAX;

    // uzel nove deklarovane funkce
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
 * @param node_idFnc Uzel funkce, ke ktere budu pridavat datove typy parametru
 * @return Typ erroru generovany analyzou
*/
int params_dec(Token *token, FILE *sourceFile, bst_node_t *node_idFnc) {
    int error = ERROR_PASSED;

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
                // aplikace pravidla 8
                if ((error = data_type(token, sourceFile, node_idFnc, NULL, PARAM_TYPE)))
                    return error;
            } else // prijate keyword nelze pouzit - pravidlo neexistuje
                return ERROR_SYNTAX;
            break;

        default: // pro token zadne pravidlo neexistuje
            return ERROR_SYNTAX;
    } // switch

    return params_dec2(token, sourceFile, node_idFnc);
} // params_dec


/**
 * @brief Neterminal params_dec2.
 *
 * Implementuje pravidla 9, 10.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @param node_idFnc Uzel funkce, ke ktere budu pridavat datove typy parametru
 * @return Typ erroru generovany analyzou
*/
int params_dec2(Token *token, FILE *sourceFile, bst_node_t *node_idFnc) {
    int error = ERROR_PASSED;

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
        // aplikace pravidla 10
        if ((error = data_type(token, sourceFile, node_idFnc, NULL, PARAM_TYPE)))
            return error;
    } else {
        return ERROR_SYNTAX;
    }

    // rozvinuti neterminalu params_dec2
    return params_dec2(token, sourceFile, node_idFnc);
} // params_dec2

/**
 * @brief Neterminal return_type.
 *
 * Implementuje pravidla 11, 12.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @param node_idFnc Uzel funkce, ke ktere budu pridavat datovy typ navratove hodnoty
 * @return Typ erroru generovany analyzou
*/
int return_type(Token *token, FILE *sourceFile, bst_node_t *node_idFnc) {
    int error = ERROR_PASSED;

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
    } // switch

    // rozvinuti netreminalu data_type
    return data_type(token, sourceFile, node_idFnc, NULL, RET_TYPE); // aplikace pravidla 11
} // return_type


/**
 * @brief Neterminal data_type.
 *
 * Implementuje pravidel 13, 14, 15.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @param node_id Uzel funkce, ke ktere budu pridavat datove typy
 * @param param Ukazatel na jeden parametr ze seznamu parametru funkce (NULL pokud nekontroluju datovy typ parametru)
 * @param dataSwitch Urcuje, zda se datovy typ priradi promenne, parametru, nebo navratove hodnote
 * @return Typ erroru generovany analyzou
*/
int data_type(Token *token, FILE *sourceFile, bst_node_t *node_id, SLLElementPtr_Param param, int dataSwitch) {
    int error = ERROR_PASSED;
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
                    if(!isDecFnc(node_id)) {
                        if (param != NULL) // funkce byla deklarovana a prave ji definuju => parametr byl byt vytvoren a pridam mu jmeno
                            param->type = TYPE_INTEGER;
                        else // funkci prave deklaruju => parametr musim vytvorit
                            SLL_Param_Insert(TYPE_STRING, node_id->name, node_id);
                    }
                    else // fce byla deklarovana => musim zkontrolovat dat. typy deklarace a definice
                        error = (param->type == TYPE_INTEGER ? ERROR_PASSED : ERROR_SEM_UNDEFINED);
                    break;

                case RET_TYPE:
                    if(!isDecFnc(node_id))
                        error = SLL_Return_Insert(TYPE_INTEGER, node_id);
                    else // fce byla deklarovana => musim zkontrolovat dat. typy deklarace a definice
                        error = (node_id->funcData->returnList->firstElement->type == TYPE_INTEGER ? ERROR_PASSED : ERROR_SEM_UNDEFINED);
                    break;
            } // switch
            break; // KEYWORD_INTEGER

        case KEYWORD_NUMBER: // aplikace pravidla 14
            switch (dataSwitch) {
                case VAR_TYPE:
                    setVarType(node_id, TYPE_NUMBER);
                    break;

                case PARAM_TYPE:
                    if(!isDecFnc(node_id)) {
                        if(param != NULL) // funkce byla deklarovana a prave ji definuju => parametr byl byt vytvoren a pridam mu jmeno
                            param->type = TYPE_NUMBER;
                        else // funkci prave deklaruju => parametr musim vytvorit
                            SLL_Param_Insert(TYPE_STRING, node_id->name, node_id);
                    }
                    else // fce byla deklarovana => musim zkontrolovat dat. typy deklarace a definice
                        error = (param->type == TYPE_NUMBER ? ERROR_PASSED : ERROR_SEM_UNDEFINED);
                    break;

                case RET_TYPE:
                    if(!isDecFnc(node_id))
                        error = SLL_Return_Insert(TYPE_NUMBER, node_id);
                    else // fce byla deklarovana => musim zkontrolovat dat. typy deklarace a definice
                        error = (node_id->funcData->returnList->firstElement->type == TYPE_NUMBER ? ERROR_PASSED : ERROR_SEM_UNDEFINED);
                    break;
            } // switch
            break; // KEYWORD_NUMBER

        case KEYWORD_STRING: // aplikace pravidla 15
            switch (dataSwitch) {
                case VAR_TYPE:
                    setVarType(node_id, TYPE_STRING);
                    break;

                case PARAM_TYPE:
                    if(!isDecFnc(node_id)) {
                        if (param != NULL) // funkce byla deklarovana a prave ji definuju => parametr byl byt vytvoren a pridam mu jmeno
                            param->type = TYPE_STRING;
                        else // funkci prave deklaruju => parametr musim vytvorit
                            SLL_Param_Insert(TYPE_STRING, node_id->name, node_id);
                    }
                    else // fce byla deklarovana => musim zkontrolovat dat. typy deklarace a definice
                        error = (param->type == TYPE_STRING ? ERROR_PASSED : ERROR_SEM_UNDEFINED);
                    break;

                case RET_TYPE:
                    if(!isDecFnc(node_id))
                        error = SLL_Return_Insert(TYPE_STRING, node_id);
                    else // fce byla deklarovana => musim zkontrolovat dat. typy deklarace a definice
                        error = (node_id->funcData->returnList->firstElement->type == TYPE_STRING ? ERROR_PASSED : ERROR_SEM_UNDEFINED);
                    break;
            } // switch
            break; // KEYWORD_STRING

        case KEYWORD_NIL: // aplikace pravidla 15
            switch (dataSwitch) {
                case VAR_TYPE:
                    setVarType(node_id, TYPE_NIL);
                    break;

                case PARAM_TYPE:
                    if(!isDecFnc(node_id)) {
                        if (param != NULL) // funkce byla deklarovana a prave ji definuju => parametr byl byt vytvoren a pridam mu jmeno
                            param->type = TYPE_NIL;
                        else // funkci prave deklaruju => parametr musim vytvorit
                            SLL_Param_Insert(TYPE_NIL, node_id->name, node_id);
                    }
                    else // fce byla deklarovana => musim zkontrolovat dat. typy deklarace a definice
                        error = (param->type == TYPE_NIL ? ERROR_PASSED : ERROR_SEM_UNDEFINED);
                    break;

                case RET_TYPE:
                    if(!isDecFnc(node_id))
                        error = SLL_Return_Insert(TYPE_STRING, node_id);
                    else // fce byla deklarovana => musim zkontrolovat dat. typy deklarace a definice
                        error = (node_id->funcData->returnList->firstElement->type == TYPE_NIL ? ERROR_PASSED : ERROR_SEM_UNDEFINED);
                    break;
            } // switch
            break; // KEYWORD_STRING

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
    int error = ERROR_PASSED;

    // id_fnc
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_ID)
        return ERROR_SYNTAX;

    // uzel s volanou funkci
    bst_node_t *node_idFnc = NULL;

    // vyhledani id_fnc v symtable
    node_idFnc = search_Iden(token->Value.string, symTable);
    if(node_idFnc == NULL) // funkce nebyla deklarovana ani definovana
        return ERROR_SEM_UNDEFINED;

    // brikule
    char *var = token->Value.string->str;

    // '('
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_LBR)
        return ERROR_SYNTAX;

    // volani funkce write se osetri zvlast (promenny pocet parametru
    if(!strcmp(node_idFnc->name->str, "write")) {
        return writeFncCall(token, sourceFile);
    }
    // ukazatel na ukazatel na polozku v seznamu parametru
    SLLElementPtr_Param *param = (SLLElementPtr_Param *) malloc(sizeof(SLLElementPtr_Param));
    if(param == NULL)
        return ERROR_COMPILER;
    *param = node_idFnc->funcData->paramList->firstElement;

    if(symTable->TopLocalElement != NULL)
        printf("PUSHFRAME\n");

    printf("CREATEFRAME\n");


    // rozvinuti neterminalu value
    if ((error = value(token, sourceFile, node_idFnc, param))) {
        free(param);
        return error;
    }
    free(param);

    // ')'
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_RBR)
        return ERROR_SYNTAX;

    printf("CALL $%s\n", var);

    if(symTable->TopLocalElement != NULL)
        printf("POPFRAME\n");


    return ERROR_PASSED;
} // fnc_call


/**
 * @brief Neterminal value.
 *
 * Implementuje pravidla 17 a 18.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @param node_idFnc Uzel funkce, pro jejiz parametry budu kontrolovat datove typy
 * @param param Ukazatel na jeden parametr ze seznamu parametru funkce
 * @return Typ erroru generovany analyzou
*/
int value(Token *token, FILE *sourceFile, bst_node_t *node_idFnc, SLLElementPtr_Param *param) {
    int error = ERROR_PASSED;
    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);

    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    switch (token->ID) {
        case TOKEN_ID_RBR: // ')'
            // funkce ocekava parametr, ktery na vstup neprisel
            if((isDecFnc(node_idFnc) || isDefFnc(node_idFnc)) && (*param) != NULL)
                return ERROR_SEM_TYPE_COUNT;
            else {
                // nastaveni cteni pred zavorku, aby si ji precetl volajici
                fsetpos(sourceFile, &lastReadPos);
                return ERROR_PASSED; // aplikace pravidla 17
            }

        case TOKEN_ID_KEYWORD: // nil
            if (token->Value.keyword == KEYWORD_NIL) {
                // funkce neocekava dalsi parametr
                if((isDecFnc(node_idFnc) || isDefFnc(node_idFnc)) && (*param) == NULL)
                    return ERROR_SEM_TYPE_COUNT;
                else {
                    // nasteveni cteni pred nil, aby si ji precetl volany
                    fsetpos(sourceFile, &lastReadPos);
                    if((error = value_last(token, sourceFile, node_idFnc, param))) // aplikace pravidla 18
                        return error;
                }
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
            // funkce neocekava dalsi parametr
            if((isDecFnc(node_idFnc) || isDefFnc(node_idFnc)) && (*param) == NULL)
                return ERROR_SEM_TYPE_COUNT;
            else {
                // nastaveni cteni pred hodnotu, aby si ji precetl volany
                fsetpos(sourceFile, &lastReadPos);
                if ((error = value_last(token, sourceFile, node_idFnc, param))) // aplikace pravidla 18
                    return error;
            }
            break;

        default:
            return ERROR_SYNTAX;
    } // switch

    return value2(token, sourceFile, node_idFnc, param);
} // value


/**
 * @brief Neterminal value2.
 *
 * Implementuje pravidla 19 a 20.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @param node_idFnc Uzel funkce, pro jejiz parametry budu kontrolovat datove typy
 * @param param Ukazatel na jeden parametr ze seznamu parametru funkce
 * @return Typ erroru generovany analyzou
*/
int value2(Token *token, FILE *sourceFile, bst_node_t *node_idFnc, SLLElementPtr_Param *param) {
    int error = ERROR_PASSED;
    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);

    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID == TOKEN_ID_RBR) { // ')'
        // funkce ocekava parametr, ktery na vstup neprisel
        if((isDecFnc(node_idFnc) || isDefFnc(node_idFnc)) && (*param) != NULL)
            return ERROR_SEM_TYPE_COUNT;
        else {
            // nastaveni cteni pred zavorku, aby si ji precetl volajici
            fsetpos(sourceFile, &lastReadPos);
            return ERROR_PASSED; // aplikace pravidla 20
        }
    } else if (token->ID == TOKEN_ID_CMA) { // ','
        // aplikace pravidla 19
    } else { // pro token zadne pravidlo neexistuje
        return ERROR_SYNTAX;
    }

    // funkce neocekava dalsi parametr
    if((isDecFnc(node_idFnc) || isDefFnc(node_idFnc)) && (*param) == NULL)
        return ERROR_SEM_TYPE_COUNT;

    // rozvinuti neterminalu value_last
    if ((error = value_last(token, sourceFile, node_idFnc, param)))
        return error;

    return value2(token, sourceFile, node_idFnc, param);
} // value2


/**
 * @brief Neterminal value_last.
 *
 * Implementuje pravidla  21, 22, 23, 24 a 25.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @param node_idFnc Uzel funkce, pro jejiz parametry budu kontrolovat datove typy
 * @param param Ukazatel na jeden parametr ze seznamu parametru funkce
 * @return Typ erroru generovany analyzou
*/
int value_last(Token *token, FILE *sourceFile, bst_node_t *node_idFnc, SLLElementPtr_Param *param) {
    // overeni, zda funkce jeste hleda parametry
    if((*param) == NULL)
        return ERROR_SEM_TYPE_COUNT;

    int error = ERROR_PASSED;
    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);

    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    // uzel s promennou/funkci
    bst_node_t *node_idVar = NULL;

    printf("DEFVAR TF@&%s\n", (*param)->name->str);

    switch (token->ID) {
        case TOKEN_ID_ID: // id_var
            // aplikace pravidla 21
            node_idVar = search_Iden(token->Value.string, symTable);
            if(isFnc(node_idVar)) // id_fnc
                return ERROR_SYNTAX;
            setVarUsed(node_idVar, true);
            // overeni datovych typu
            if(typeVar(node_idVar) != (*param)->type) {
                if((*param)->type == TYPE_NUMBER && typeVar(node_idVar) == TYPE_INTEGER) {
                    printf("MOVE TF@&%s LF@&%s\n", (*param)->name->str, token->Value.string->str);
                    printf("INT2FLOAT TF@&%s TF@&%s\n", (*param)->name->str, (*param)->name->str);
                }
                else // promenne nemaji kompatibilni typy
                    return ERROR_SEM_TYPE_COUNT;
            }
            else { // datove typy jsou kompatibilni
                printf("MOVE TF@&%s LF@&%s\n", (*param)->name->str, token->Value.string->str);
            }
            break;

        case TOKEN_ID_INT0:
        case TOKEN_ID_HEX2:
        case TOKEN_ID_INT:
        case TOKEN_ID_ZERO: // int_value
            // aplikace pravidla 22
            if((*param)->type != TYPE_INTEGER) {
                if((*param)->type == TYPE_NUMBER) { // ocekavam number, davam mu integer
                    // TODO generovani kodu - int2float
                    printf("MOVE TF@&%s float@%lld\n", (*param)->name->str, token->Value.Integer);
                }
                else // datove typy nejsou kompatibilni
                    return ERROR_SEM_TYPE_COUNT;
            }
            else {
                // TODO generovani kodu pro integer
                printf("MOVE TF@&%s int@%lld\n", (*param)->name->str, token->Value.Integer);
            }
            break;

        case TOKEN_ID_DHEX2:
        case TOKEN_ID_HEXP3:
        case TOKEN_ID_DBL2:
        case TOKEN_ID_EXP3: // num_value
            // aplikace pravidla 23
            if((*param)->type != TYPE_NUMBER) // datove typy nejsou kompatibilni
                return ERROR_SEM_TYPE_COUNT;
            // TODO generace kodu pro number
            printf("MOVE TF@&%s float@%a\n", (*param)->name->str, token->Value.Double);
            break;

        case TOKEN_ID_FSTR: // str_value
            // aplikace pravidla 24
            if((*param)->type != TYPE_STRING) // datove typy nejsou kompatibilni
                return ERROR_SEM_TYPE_COUNT;
            // TODO generace kodu pro string
            printf("MOVE TF@&%s string@%s\n", (*param)->name->str, converString(token->Value.string->str));
            break;

        case TOKEN_ID_KEYWORD: // nil
            if (token->Value.keyword == KEYWORD_NIL) {
                // aplikace pravidal 25
                // TODO generovani kodu pro nil
                printf("MOVE TF@&%s nil@nil\n", (*param)->name->str);
            } else // pro keyword neexistuje pravidlo
                return ERROR_SYNTAX;
            break;

        default: // pro token zadne pravidlo neexistuje
            return ERROR_SYNTAX;
    } // switch

    // prejdu na dalsi parametr funkce v seznamu
    (*param) = (*param)->nextElement;
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
    int error = ERROR_PASSED;

    // ukazatel na uzel s funkci
    bst_node_t **node_idFnc = (bst_node_t **) malloc(sizeof(bst_node_t *));
    if (node_idFnc == NULL)
        return ERROR_COMPILER;

    // brikule
    fpos_t last_read_pos;
    fgetpos(sourceFile, &last_read_pos);
    if((error = get_non_white_token(token, sourceFile)))
        return error;
    if(token->ID != TOKEN_ID_ID)
        return ERROR_SYNTAX;

    char *var = token->Value.string->str;
    fsetpos(sourceFile, &last_read_pos);
    // brikule

    // rozvinuti neterminalu fnc_head
    if ((error = fnc_head(token, sourceFile, node_idFnc))) {
        free(node_idFnc);
        return error;
    }


    // rozvinuti neterminalu fnc_def2
    printf("PUSHFRAME\n");
    make_CREATEFRAME_TMP();

    // prekopirovani parametru z LF do TF
    DLList_Instruct *dll_istInstruct = (DLList_Instruct *) malloc(sizeof(DLList_Instruct));
    if (dll_istInstruct == NULL)
        return ERROR_COMPILER;
    DLL_Instruct_Init(dll_istInstruct);
    getAllVar(dll_istInstruct, symTable);
    movePrevious(dll_istInstruct);
    DLL_Instruct_Dispose(dll_istInstruct);
    free(dll_istInstruct);

    if ((error = fnc_def2(token, sourceFile, node_idFnc))) {
        free(node_idFnc);
        return error;
    }

    // end
    if ((error = get_non_white_token(token, sourceFile))) {
        free(node_idFnc);
        // lexikalni nebo kompilatorova chyba
        return error;
    }

    if (token->ID != TOKEN_ID_KEYWORD) {
        free(node_idFnc);
        return ERROR_SYNTAX;
    }
    else if (token->Value.keyword != KEYWORD_END) {
        free(node_idFnc);
        return ERROR_SYNTAX;
    }

    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $$end_fnc_%s\n", var);
    // odeberu ramec funkce
    SLL_Frame_Delete(symTable);

    free(node_idFnc);
    return ERROR_PASSED;
} // fnc_def


/**
 * @brief Neterminal fnc_head.
 *
 * Implementuje pravidlo 27.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @param node_idFnc Ukazatel na uzel funkce, ktere budu pridavat parametry
 * @return Typ erroru generovany analyzou
*/
int fnc_head(Token *token, FILE *sourceFile, bst_node_t **node_idFnc) {
    // token function byl prijaty o uroven vyse => pokracuju dale
    // aplikuju pravidlo 27
    int error = ERROR_PASSED;

    // id_fnc
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;
    if (token->ID != TOKEN_ID_ID)
        return ERROR_SYNTAX;

    // overeni, zda nebyla funkce uz deklarovana nebo definovana
    *node_idFnc = search_Iden(token->Value.string, symTable);

    if (*node_idFnc == NULL) {
        bst_insert(&(symTable->globalElement->node), token->Value.string, true);
        *node_idFnc = bst_search(symTable->globalElement->node, token->Value.string);
    } else if (isDecFnc(*node_idFnc) && !isDefFnc(*node_idFnc)) { // funkce byla uz deklarovana

    } else { // funkce byla uz i definovanna - pokus o redefinici
        return ERROR_SEM_UNDEFINED;
    }

    printf("JUMP $$end_fnc_%s\n", token->Value.string->str);
    printf("LABEL $%s\n", token->Value.string->str);

    // '('
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;
    if (token->ID != TOKEN_ID_LBR)
        return ERROR_SYNTAX;

    // vytvoreni noveho ramce pro funkci
    SLL_Frame_Insert(symTable);

    // ukazatel na prvni parametr
    SLLElementPtr_Param *param = (SLLElementPtr_Param *) malloc(sizeof(SLLElementPtr_Param));
    if (param == NULL)
        return ERROR_COMPILER;
    *param = (*node_idFnc)->funcData->paramList->firstElement;

    // rozvinu neterminal params_def
    if (isDecFnc(*node_idFnc)) {
        // u deklarovane funkce musim kontrolovat pocet a typ paremtru jeji definice
        if ((error = params_def(token, sourceFile, *node_idFnc, param))) {
            free(param);
            return error;
        }
    } else {
        // nedeklarovana funkce ma prazdny seznam parametru
        if ((error = params_def(token, sourceFile, *node_idFnc, param))) {
            free(param);
            return error;
        }
    }

    free(param);

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
 * @param node_idFnc Ukazatel na uzel funkce, ktere budu pridavat navratovou hodnotu
 * @return Typ erroru generovany analyzou
*/
int fnc_def2(Token *token, FILE *sourceFile, bst_node_t **node_idFnc) {
    int error = ERROR_PASSED;

    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);

    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    // ':'
    if (token->ID == TOKEN_ID_CLN) { // aplikace pravidla 29
        // deklarovana funkce navratovy typ nema, ktery jsem tady dostal
        if(isDecFnc(*node_idFnc) && (*node_idFnc)->funcData->returnList->firstElement == NULL)
            return ERROR_SEM_UNDEFINED;

        // rozvinuti neterminalu data_type
        if(isDecFnc(*node_idFnc))
            error = data_type(token, sourceFile, *node_idFnc, NULL, RET_TYPE);
        else
            error = data_type(token, sourceFile, *node_idFnc, NULL, RET_TYPE);

        if (error)
            return error;

        setFncDef(*node_idFnc, true);
        // true, pokud funkce vratila hodnotu
        bool returned = false;

        // rozvinuti neterminalu fnc_body
        if ((error = fnc_body(token, sourceFile, (*node_idFnc)->funcData->returnList->firstElement->type, &returned)))
            return error;

        if(!returned) { // funkce nevratila zadnou hodnoty
            printf("MOVE GF@*return nil@nil\n");
        }

    } else { // id_fce, id_var, local, if, while, return nebo end
        // aplikace pravidla 28

        // deklarovana funkce ma navratovy typ, ktery jsem nedostal
        if(isDecFnc(*node_idFnc) && (*node_idFnc)->funcData->returnList->firstElement != NULL)
            return ERROR_SEM_UNDEFINED;

        if (token->ID == TOKEN_ID_ID) { // id_fce nebo id_var
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

        // oznacim funkci jako definovanou
        setFncDef(*node_idFnc, true);

        // rozvinuti neterminalu fnc_body
        bool noNeedToReturn;
        if ((error = fnc_body(token, sourceFile, TYPE_UNDEFINED, &noNeedToReturn)))
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
 * @param node_idFnc Uzel funkce, ktere budu pridavat parametry
 * @param param Ukazatel na jeden parametr ze seznamu parametru funkce
 * @return Typ erroru generovany analyzou
*/
int params_def(Token *token, FILE *sourceFile, bst_node_t *node_idFnc, SLLElementPtr_Param *param) {
    int error = ERROR_PASSED;

    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);

    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID == TOKEN_ID_RBR) { // ')'
        // deklarovana funkce ocekava parametr a zadny nedostala
        if(isDecFnc(node_idFnc) && (*param) != NULL)
            return ERROR_SEM_UNDEFINED;
        else {
            // nastavim cteni pred zavorku, aby si ji precetl volajici
            fsetpos(sourceFile, &lastReadPos);
            return ERROR_PASSED; // aplikace pravidla 30
        }
    }
    else if (token->ID == TOKEN_ID_ID) { // id_var
        // nastavim cteni pred identifikator, aby si ho precetl volany
        fsetpos(sourceFile, &lastReadPos);
    }
    else // pro tento token neexistuje pravidlo
        return ERROR_SYNTAX;

    // deklarovana funkce nema parametr, ale dostala ho
    if(isDecFnc(node_idFnc) && (*param) == NULL)
        return ERROR_SEM_UNDEFINED;

    // rozsireni neterminalu var_def
    if ((error = var_defParam(token, sourceFile, node_idFnc, param))) // aplikace pravidla 31
        return error;

    // rozsireni neterminalu params_def2
    return params_def2(token, sourceFile, node_idFnc, param);
} // params_def


/**
 * @brief Neterminal params_def2.
 *
 * Implementuje pravidlo 32 a 33.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @param node_idFnc Uzel funkce, ktere budu pridavat parametry
 * @param param Ukazatel na jeden parametr ze seznamu parametru funkce
 * @return Typ erroru generovany analyzou
*/
int params_def2(Token *token, FILE *sourceFile, bst_node_t *node_idFnc, SLLElementPtr_Param *param) {
    int error = ERROR_PASSED;

    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);

    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID == TOKEN_ID_RBR) { // ')'
        // deklarovana funkce ocekava parametr a zadny nedostala
        if(isDecFnc(node_idFnc) && (*param) != NULL)
            return ERROR_SEM_UNDEFINED;
        else {
            // nastavim cteni pred zavorku, aby si ji precetl volajici
            fsetpos(sourceFile, &lastReadPos);
            return ERROR_PASSED; // aplikace pravidla 32
        }
    } else if (token->ID == TOKEN_ID_CMA) { // ','
        // aplikace pravidla 33 (hned za koncem podminky)
    } else { // pro tento token neexistuje pravidlo
        return ERROR_SYNTAX;
    }

    // deklarovana funkce neocekava parametr, ale nejaky dostala
    if(isDecFnc(node_idFnc) && (*param) == NULL)
        return ERROR_SEM_UNDEFINED;

    // rozvinuti neterminalu var_def
    if ((error = var_defParam(token, sourceFile, node_idFnc, param)))
        return error;

    // znovu rozvinuti neterminalu params_def2
    return params_def2(token, sourceFile, node_idFnc, param);
} // params_def2

/**
 * @brief Neterminal var_def.
 *
 * Totozna s var_def, ale kontroluje semantiku pro promenne v seznamu parametru
 *
 * Implementuje pravidlo 34.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @param node_idFnc Uzel funkce, ktere budu pridavat parametr
 * @param param Ukazatel na jeden parametr ze seznamu parametru funkce
 * @return Typ erroru generovany analyzou
*/
int var_defParam(Token *token, FILE *sourceFile, bst_node_t *node_idFnc, SLLElementPtr_Param *param) {
    int error = ERROR_PASSED;
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
    // nastaveni parametru jako inicializovany
    setVarInit(node_idVar, true);


    if(!isDecFnc(node_idFnc)) { // pokud funkce nebyla deklarovana, vytvorim ji hned parametr a priradim mu jeho jmeno
        if((error = SLL_Param_Insert(TYPE_UNDEFINED, token->Value.string, node_idFnc)))
            return error;
        // ulozim si prave pridany parametr
        (*param) = node_idFnc->funcData->paramList->firstElement;
        while((*param)->nextElement != NULL)
            (*param) = (*param)->nextElement;
    }
    else { // funkce byla deklarovana => priradim parametru jen jmeno
        (*param)->name = token->Value.string;
    }

    // ':'
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_CLN)
        return ERROR_SYNTAX;

    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);

    if (isDecFnc(node_idFnc)) {
        // funkce je deklarovana => kontrola dat. typu parametru v deklaraci a definici
        if((error = data_type(token, sourceFile, node_idFnc, *param, PARAM_TYPE)))
            return error;
        *param = (*param)->nextElement;
    }
    else {
        // funkce neni deklarovana => ulozeni dat.typu parametru k id funkce
        if((error = data_type(token, sourceFile, node_idFnc, *param, PARAM_TYPE)))
            return error;
    }
    fsetpos(sourceFile, &lastReadPos);
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
 * @param node_idFnc Ukazatel na uzel promenne, ktere budu pridavat datovy typ
 * @return Typ erroru generovany analyzou
*/
int var_def(Token *token, FILE *sourceFile, bst_node_t **node_idVar) {

    int error = ERROR_PASSED;
    // aplikace pravidla 34

    // id_var
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_ID)
        return ERROR_SYNTAX;

    // overeni, ze identifikator zatim neexistuje
    if (bst_search(symTable->TopLocalElement->node, token->Value.string) != NULL || bst_search(symTable->globalElement->node, token->Value.string) != NULL)
        return ERROR_SEM_UNDEFINED;
    // vlozeni identifikatoru do symtable
    bst_insert(&(symTable->TopLocalElement->node), token->Value.string, false);

    *node_idVar = bst_search(symTable->TopLocalElement->node, token->Value.string);

    // ':'
    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID != TOKEN_ID_CLN)
        return ERROR_SYNTAX;

    // rozvinuti neterminalu data_type
    return data_type(token, sourceFile, *node_idVar, NULL, VAR_TYPE);
} // var_def


/**
 * @brief Neterminal return.
 *
 * Implementuje pravidlo 35.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @param fnc_ret_dataType Datovy typ, ktera ma funkce vracet
 * @return Typ erroru generovany analyzou
*/
int return_(Token *token, FILE *sourceFile, Data_type fncRetType, bool *returned) {
    int error = ERROR_PASSED;

    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);

    // aplikace pravidla 35

    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    bst_node_t *node_idVar = NULL;

    printf("DEFVAR TF@&return\n");

    switch(token->ID) {
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
            if(fncRetType == TYPE_UNDEFINED)
                return ERROR_SEM_TYPE_COUNT;
            else {
                // aplikace pravidla 37
                fsetpos(sourceFile, &lastReadPos);
                if(token->ID == TOKEN_ID_ID) {
                    if ((error = exprSyntaxCheck(token, sourceFile, symTable, fncRetType, token->Value.string->str, NULL)))
                        return error;
                }
                else {
                    if ((error = exprSyntaxCheck(token, sourceFile, symTable, fncRetType, "return", NULL)))
                        return error;
                }
                *returned = true;
            }
            break;

        case TOKEN_ID_KEYWORD:
            if(token->Value.keyword == KEYWORD_NIL) {
                if(fncRetType == TYPE_UNDEFINED)
                    return ERROR_SEM_TYPE_COUNT;
                else {
                    // aplikace pravidla 37
                    fsetpos(sourceFile, &lastReadPos);
                    if ((error = exprSyntaxCheck(token, sourceFile, symTable, fncRetType, "return", NULL)))
                        return error;
                    *returned = true;
                }
            }
            else if(token->Value.keyword == KEYWORD_END || token->Value.keyword == KEYWORD_ELSE) {
                // aplikace pravidla 36
                if(fncRetType == TYPE_UNDEFINED) {
                    fsetpos(sourceFile, &lastReadPos);
                    return ERROR_PASSED;
                }
                else // funkce ma vracet hodnotu a nevraci
                    return ERROR_SEM_TYPE_COUNT;
            }
            else
                return ERROR_SYNTAX;
            break;
            break;

        case TOKEN_ID_ID:
            // vyhledam identifikator v tabulce
            node_idVar = search_Iden(token->Value.string, symTable);
            // nastaveni cteni pred identifikator, aby si to precetl volany
            fsetpos(sourceFile, &lastReadPos);

            if (node_idVar == NULL) // identifikator neexistuje
                return ERROR_SEM_UNDEFINED;
            else if (isFnc(node_idVar)) { // id_fnc
                return ERROR_SEM_OTHERS;
            }
            else { // id_var
                fsetpos(sourceFile, &lastReadPos);
                // zavolani bottomup SA pro neterminal expr
                if((error = exprSyntaxCheck(token, sourceFile, symTable, fncRetType, "return", NULL))) // aplikace pravidla 48
                    return error;
                *returned = true;
                // TODO generovani kodu - vraceni hodnoty vyrazu
            }
            break;

        default:
            return ERROR_SYNTAX;
    }
    printf("MOVE GF@*return TF@&return\n");
    return error;
} // return_


/**
 * @brief Neterminal fnc_body.
 *
 * Implementuje pravidlo 38 a 39.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int fnc_body(Token *token, FILE *sourceFile, Data_type fncRetType, bool *returned) {
    int error = ERROR_PASSED;

    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);

    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID == TOKEN_ID_KEYWORD) { // local, if, while, return, nebo end
        bool returnInBody = false;
        switch (token->Value.keyword) {
            case KEYWORD_LOCAL: // local
            case KEYWORD_IF: // if
            case KEYWORD_WHILE: // while
                // vratim cteni pred keyword, aby si ho precetl volany
                fsetpos(sourceFile, &lastReadPos);

                // rozvinu neterminal statement
                if ((error = statement(token, sourceFile, fncRetType, returned))) // aplikace pravidla 38
                    return error;

                break;

            case KEYWORD_RETURN: // return
                error = return_(token, sourceFile, fncRetType, &returnInBody);
                *returned = (*returned) || returnInBody;
                return error;
                break;

            case KEYWORD_END: // end
                // vratim cteni pred end, aby si ho precetl volajici
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
        if ((error = statement(token, sourceFile, fncRetType, returned))) // aplikace pravidla 38
            return error;
    } else { // pro prijaty token neexistuje pravidlo
        return ERROR_SYNTAX;
    }

    return fnc_body(token, sourceFile, fncRetType, returned);
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
int statement(Token *token, FILE *sourceFile, Data_type fncRetType, bool *returned) {
    int error = ERROR_PASSED;

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
                error = if_(token, sourceFile, fncRetType, returned); // aplikace pravidla 43
                break;

            case KEYWORD_WHILE: // while
                // rozvinuti neterminalu loop
                error = loop(token, sourceFile, fncRetType, returned); // aplikace pravidla 44
                break;

            case KEYWORD_RETURN: // return
                // rozvinuti neterminalu return
                error = return_(token, sourceFile, fncRetType, returned); // aplikace pravidla 44
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
            // aplikace pravidla 41

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
    error = var_assign(token, sourceFile, id_node, token->Value.string->str);
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

    int error = ERROR_PASSED;
    // local bylo uz precteno volajicim

    // ukazatel na uzel s promennou
    bst_node_t **node_idVar = (bst_node_t **) malloc(sizeof(bst_node_t *));
    if(node_idVar == NULL)
        return ERROR_COMPILER;

    //zjisti ID tokenu
    fpos_t last_read_pos;
    fgetpos(sourceFile, &last_read_pos);
    if((error = get_non_white_token(token, sourceFile)))
        return error;
    if(token->ID != TOKEN_ID_ID)
        return ERROR_SYNTAX;

    char *var = token->Value.string->str;
    make_DEFVAR_TF(token->Value.string);

    fsetpos(sourceFile, &last_read_pos);
    // rozvinuti neterminalu var_def
    if ((error = var_def(token, sourceFile, node_idVar))) {
        free(node_idVar);
        return error;
    }

    // rozvinuti neterminalu var_dec_init
    error = var_dec_init(token, sourceFile, *node_idVar, var);
    free(node_idVar);
    return error;
} // var_dec


/**
 * @brief Neterminal var_dec_init.
 *
 * Implementuje pravidlo 46 a 47.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @param node_idVar Uzel promenne
 * @return Typ erroru generovany analyzou
*/
int var_dec_init(Token *token, FILE *sourceFile, bst_node_t *node_idVar, char *var) {
    int error = ERROR_PASSED;

    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);

    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    if (token->ID == TOKEN_ID_ASSIGN) { // '='
        // aplikace pravidla 46

        // rozvinuti neteminalu var_assign
        error = var_assign(token, sourceFile, node_idVar, var);
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
 * @param varType Datovy typ promenne
 * @return Typ erroru generovany analyzou
*/
int var_assign(Token *token, FILE *sourceFile, bst_node_t *node_idVar, char *var) {
    int error = ERROR_PASSED;

    // promenne pro pripadne vraceni cteni
    fpos_t lastReadPos;
    fgetpos(sourceFile, &lastReadPos);

    if ((error = get_non_white_token(token, sourceFile)))
        // lexikalni nebo kompilatorova chyba
        return error;

    // uzel s pripadnym identifikatorem (za rovnitkem)
    bst_node_t *id = NULL;
    switch (token->ID) {
        case TOKEN_ID_ID: // id_var nebo var_fce
            // vyhledam identifikator v tabulce
            id = search_Iden(token->Value.string, symTable);
            // nastaveni cteni pred identifikator, aby si to precetl volany
            fsetpos(sourceFile, &lastReadPos);

            if (id == NULL) // identifikator neexistuje
                return ERROR_SEM_UNDEFINED;
            else if (isFnc(id)) { // id_fnc
                // test, zda vraci funkce spravny datovy typ
                if(id->funcData->returnList->firstElement == NULL) { // funkce nevraci hodnotu
                    printf("MOVE TF@&%s nil@nil\n", var);
                }
                else if(id->funcData->returnList->firstElement->type != node_idVar->varData->type) {
                    if(node_idVar->varData->type == TYPE_NUMBER && id->funcData->returnList->firstElement->type == TYPE_INTEGER) {
                    }
                    else // navratova hodnota fce neni kompatibilni s typem promenne
                        return ERROR_SEM_ASSIGN;
                }
                else { // navratova hodnota fce je kompatibilni s typem promenne

                }

                // rozvinuti neterminalu fnc_call
                if((error = fnc_call(token, sourceFile))) // aplikace pravidla 49
                    return error;
                printf("MOVE TF@&%s GF@*return\n", var);
            }
            else { // id_var
                // zavolani bottomup SA pro neterminal expr
                if((error = exprSyntaxCheck(token, sourceFile, symTable, typeVar(node_idVar), var, NULL))) // aplikace pravidla 48
                    return error;
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
            if((error = exprSyntaxCheck(token, sourceFile, symTable, typeVar(node_idVar), var, NULL))) // aplikace pravidla 48
                return error;
            break;

        case TOKEN_ID_KEYWORD:
            if (token->Value.keyword == KEYWORD_NIL) { // nil
                // vratim cteni pred nil, aby si ho precetl volany
                fsetpos(sourceFile, &lastReadPos);

                //aplikace pravidla 48
                // volani bottom-up SA (rozsireni neterminalu expr)
                if((error = exprSyntaxCheck(token, sourceFile, symTable, typeVar(node_idVar), var, NULL))) // aplikace pravidla 48
                    return error;
            } else // pro tento keyword neexistuje pravidlo
                return ERROR_SYNTAX;
            break;

        default: // pro tento token neexistuje pravidlo
            return ERROR_SYNTAX;
    } // switch

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
int if_(Token *token, FILE *sourceFile, Data_type fncRetType, bool *returned) {
    int error = ERROR_PASSED;
    // token if byl precten o uroven vyse => pokracuju dal
    // aplikace pravidla 50

    // volani bottom-up SA (rozsireni neterminalu expr)
    if ((error = exprSyntaxCheck(token, sourceFile, symTable, TYPE_UNDEFINED, "if", NULL)))
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

    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");

    DLList_Instruct *dll_instruct = (DLList_Instruct*) malloc(sizeof (DLList_Instruct));
    DLL_Instruct_Init(dll_instruct);
    getAllVar(dll_instruct, symTable);
    movePrevious(dll_instruct);


    // rozvinu neterminal statements
    bool returnedInIf = false;
    if ((error = statements(token, sourceFile, fncRetType, &returnedInIf)))
        return error;

    moveAfter(dll_instruct);
    DLL_Instruct_Dispose(dll_instruct);
    free(dll_instruct);
    printf("POPFRAME\n");

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
    printf("JUMP !endif%d\n", ifCounter);
    printf("LABEL !else%d\n", ifCounter);
    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");
    dll_instruct = (DLList_Instruct*) malloc(sizeof (DLList_Instruct));
    DLL_Instruct_Init(dll_instruct);
    getAllVar(dll_instruct, symTable);
    movePrevious(dll_instruct);
    DLL_Instruct_Dispose(dll_instruct);
    free(dll_instruct);

    bool returnedInElse = false;

    // rozvinu neterminal statements
    if ((error = statements(token, sourceFile, fncRetType, &returnedInElse)))
        return error;

    (*returned) = returnedInIf && returnedInElse;

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

    printf("POPFRAME\n");
    printf("LABEL !endif%d\n", ifCounter++);

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
int loop(Token *token, FILE *sourceFile, Data_type fncRetType, bool *returned) {
    int error = ERROR_PASSED;
    // token while byl precten o uroven vyse => pokracuju dal
    // aplikace pravidla 51

    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");
    DLList_Instruct *dll_instruct = (DLList_Instruct*) malloc(sizeof (DLList_Instruct));
    DLL_Instruct_Init(dll_instruct);
    getAllVar(dll_instruct, symTable);
    movePrevious(dll_instruct);
    printf("#VYTVORENI DOCASNYCH PROMENNYCH\n");
    printf("DEFVAR TF@&tmp\n");
    printf("DEFVAR TF@&tmp1\n");
    printf("DEFVAR TF@&tmp2\n");
    printf("#----------------\n");
    printf("LABEL !loop%d\n", loopCounter);

    // volani bottom-up SA (rozsireni neterminalu expr)
    if ((error = exprSyntaxCheck(token, sourceFile, symTable, TYPE_UNDEFINED, "loop", &loopCounter)))
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

    bool dontCheckReturnInLoop;
    // rozvinu neterminal statements
    if ((error = statements(token, sourceFile, fncRetType, &dontCheckReturnInLoop)))
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
    moveAfter(dll_instruct);
    DLL_Instruct_Dispose(dll_instruct);
    free(dll_instruct);
    printf("JUMP !loop%d\n", --loopCounter);
    printf("LABEL !endLoop%d\n", loopCounter);
    printf("POPFRAME\n");

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
int statements(Token *token, FILE *sourceFile, Data_type fncRetType, bool *returned) {
    int error = ERROR_PASSED;

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
            case KEYWORD_IF:
            case KEYWORD_WHILE:
                // nastavim cteni pred keyword, aby si to precetl volajici
                fsetpos(sourceFile, &lastReadPos);
                break;

            case KEYWORD_RETURN: // return
                return return_(token, sourceFile, fncRetType, returned);
                break;

            default: // pro prijaty token neexistuje pravidlo
                return ERROR_SYNTAX;
        }
    } else // pro prijaty token neexistuje zadne pravidlo
        return ERROR_SYNTAX;

    // rozvinuti neterminalu statement
    if ((error = statement(token, sourceFile, fncRetType, returned))) // aplikace pravidla 53
        return error;

    return statements(token, sourceFile, fncRetType, returned);
} // statements


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
    int error = ERROR_PASSED;
		error = start(token, sourceFile);

    // TODO hazi segfault
    //DS_Free(token->Value.string);
    free(token);
    return error;
}

/** @endcode */