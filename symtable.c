/**
 * Projekt: IFJ2021
 *
 * @brief Implementace Tabulky symbolu.
 *
 * @author Vít Janeček xjanec30@stud.fit.vutbr.cz
*/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "symtable.h"

/**
 * Funkce pro inicializaci seznamu parametru.
 *
 * @param listParam Ukazatel na seznam parametru.
 */
void SLL_Param_Init(SLList_Param *listParam){
    listParam->firstElement = NULL;
}

/**
 * Funkce pro pridani prvku do seznamu parametru.
 *
 * @param type Datovy typ pridavaneho parametru.
 * @param stringName Ukazatel na DS, kde je jmeno pridavaneho parametru.
 * @param tree Ukazatel na dany uzel, ve kterem je seznam parametru, do ktereho chceme pridavat prvek.
 * @return Vraci ERROR_PASSED, jestlize se pridani povedlo, jinak ERROR_COMPILER.
 */
int SLL_Param_Insert(Data_type type, Dynamic_string *stringName, bst_node_t *tree){
    SLLElementPtr_Param nElemPtr = (SLLElementPtr_Param)malloc(sizeof(struct SLLElement_Param));  //naalokuje prostor pro pridavany prvek
    Dynamic_string *string1 = (Dynamic_string *)malloc(sizeof(Dynamic_string));     //naalokuje DS
    if (nElemPtr == NULL || string1 == NULL){
        return ERROR_COMPILER;          //pokud se malloc nepovedl
    }
    nElemPtr->type = type;                      //nahraje do pomocne
    nElemPtr->name = string1;
    nElemPtr->name->str = stringName->str;
    if (tree->funcData->paramList->firstElement == NULL) {                      //pokud je to prvni prvek
        nElemPtr->nextElement = NULL;
        tree->funcData->paramList->firstElement = nElemPtr;
    } else {
        SLLElementPtr_Param elemPtr = tree->funcData->paramList->firstElement;
        while (elemPtr->nextElement != NULL){                                   //jinak se dostane az na konec seznamu
            elemPtr = elemPtr->nextElement;
        }
        nElemPtr->nextElement = NULL;
        elemPtr->nextElement = nElemPtr;                        //prida prvek na konec seznamu
    }
    return ERROR_PASSED;
}

/**
 * Funkce pro celkove smazani seznamu parametru.
 *
 * @param listParam Ukazatel na seznam parametru.
 */
void SLL_Param_Dispose(SLList_Param *listParam){
    if (listParam != NULL){
        while (listParam->firstElement != NULL){                     //dokud prvni element neni NULL
            SLLElementPtr_Param elemPtr;
            free(listParam->firstElement->name);
            elemPtr = listParam->firstElement;                       //pomocny ukazatel ukazuje na 1. prvek
            listParam->firstElement = listParam->firstElement->nextElement;       //prvni se posune na dalsi
            free(elemPtr);          //smaze prvni prvek
        }
        listParam->firstElement = NULL;
    }
}

/**
 * Funkce pro inicializaci seznamu navratovych typu.
 *
 * @param listReturn Ukazatel na seznam navratovych typu.
 */
void SLL_Return_Init(SLList_Return *listReturn){
    listReturn->firstElement = NULL;
}

/**
 * Funkce pro pridani prvku do seznamu navratovych typu.
 * @param type Datovy typ pridavaneho navratoveho typu.
 * @param tree Ukazatel na dany uzel, ve kterem je seznam navratovych typu, do ktereho chceme pridavat prvek.
 * @return Vraci ERROR_PASSED, jestlize se pridani povedlo, jinak ERROR_COMPILER.
 */
int SLL_Return_Insert(Data_type type, bst_node_t *tree){
    SLLElementPtr_Return nElemPtr = (SLLElementPtr_Return)malloc(sizeof(struct SLLElement_Return));  //naalokuje prostor pro pridavany prvek
    if (nElemPtr == NULL){
        return ERROR_COMPILER;      //pokud se malloc nepovedl
    }
    nElemPtr->type = type;
    if (tree->funcData->returnList->firstElement == NULL) {         //pokud je to prvni prvek
        nElemPtr->nextElement = NULL;
        tree->funcData->returnList->firstElement = nElemPtr;
    } else{
        SLLElementPtr_Return elemPtr = tree->funcData->returnList->firstElement;
        while (elemPtr->nextElement != NULL){                                       //jinak se dostane az na konec seznamu
            elemPtr = elemPtr->nextElement;
        }
        nElemPtr->nextElement = NULL;
        elemPtr->nextElement = nElemPtr;                    //prida prvek na konec seznamu
    }
    return ERROR_PASSED;
}

/**
 * Funkce pro celkove smazani seznamu navratovych typu.
 *
 * @param listReturn Ukazatel na seznam navratovych typu.
 */
void SLL_Return_Dispose(SLList_Return *listReturn){
    if (listReturn != NULL) {
        while (listReturn->firstElement != NULL) {                     //dokud prvni element neni NULL
            SLLElementPtr_Return elemPtr;
            elemPtr = listReturn->firstElement;                         //pomocny ukazatel ukazuje na 1. prvek
            listReturn->firstElement = listReturn->firstElement->nextElement;       //prvni se posune na dalsi
            free(elemPtr);          //smaze prvni prvek
        }
        listReturn->firstElement = NULL;
    }
}

/**
 * Funkce pro nahrani dat do struktury pro data promenne
 * .
 *promenna
 * @param tree Ukazatel na uzel, do ktereho se nahravaji hodnoty.
 * @param type Hodnota typu promenne
 * .
 * @param init Hodnota, zdali byla promenna inicializovana.
 * @param used Hodnota, zdali byla promenna pouzita.
 */
void setVarType(bst_node_t *tree, Data_type type){
    tree->varData->type = type;
}

void setVarInit(bst_node_t *tree, bool init){
    tree->varData->init = init;
}

void setVarUsed(bst_node_t *tree, bool used){
    tree->varData->used = used;
}

/**
 * Funkce pro nahrani dat do struktury pro data funkce.
 *
 * @param tree Ukazatel na uzel, do ktereho se nahravaji hodnoty.
 * @param dec Hodnota, zdali byla funkce deklarovana.
 */
void setFncDec(bst_node_t *tree, bool dec){
    tree->funcData->dec = dec;
}

/**
 * Funkce pro nahrani dat do struktury pro data funkce.
 *
 * @param tree Ukazatel na uzel, do ktereho se nahravaji hodnoty.
 * @param def Hodnota, zdali byla funkce definovana.
 */
void setFncDef(bst_node_t *tree, bool def){
    tree->funcData->def = def;
}

/**
 * Funkce pro inicializaci Binarniho vyhledavaciho stromu.
 *
 * @param tree Ukazatel na koren BVS.
 */
void bst_init(bst_node_t **tree) {
    *tree = NULL;
}

/**
 * Funkce pro vyhledani uzlu v BVS podle zadaneho jmena.
 *
 * @param tree Ukazatel na koren Binarniho vyhledavaciho stromu.
 * @param string Ukazatel na DS, kde je jmeno hledaneho uzlu.
 * @return Vraci ukazatel na nalezeny ukazatel, jinak NULL.
 */
bst_node_t *bst_search(bst_node_t *tree, Dynamic_string *string) {
    bst_node_t *findNode = NULL;
    if (tree != NULL){                                               //dokud nedosel nakonec
        if ((strcmp(tree->name->str, string->str)) == 0){       //pokud jsou jmena stejne, vrati ukazatel
            findNode = tree;
            return findNode;
        } else{
            if ((strcmp(tree->name->str, string->str)) > 0){        //pokud je ASCII hodnota jmena mensi, rekurzivne se zavola pro levy podstrom
                findNode = bst_search(tree->left, string);
                return findNode;
            } else{
                findNode = bst_search(tree->right, string);         //pokud ASCII hodnota jmena vetsi, rekurzivne se zavola pro pravy podstrom
                return findNode;
            }
        }
    } else{
        findNode = NULL;
        return findNode;                                               // jestli nenasel, vrati NULL
    }
}

/**
 * Funkce pro pridani uzlu v BVS podle zadaneho jmena.
 *
 * @param tree Ukazatel na koren Binarniho vyhledavaciho stromu.
 * @param string Ukazatel na DS, kde je jmeno noveho uzlu.
 * @param isFnc Hodnota, zdali je to uzel pro funkci nebo pro promennou.
 * @return Vraci ERROR_PASSED, jestlize se pridani povedlo, jinak ERROR_COMPILER
 */
 int bst_insert(bst_node_t **tree, Dynamic_string *string, bool isFnc) {
    if (*tree != NULL){                                         //dokud nedosel nakonec
        if ((strcmp((*tree)->name->str, string->str)) == 0){        //pokud jsou jmena stejne, vrati error
            return ERROR_SEM_UNDEFINED;
        } else{
            int error;
            if ((strcmp((*tree)->name->str, string->str)) > 0){
                error = bst_insert(&((*tree)->left), string, isFnc);                //pokud je ASCII hodnota jmena mensi, rekurzivne se zavola pro levy podstrom
                return (error == ERROR_PASSED) ? ERROR_PASSED : ERROR_COMPILER;
            } else{
                error = bst_insert(&((*tree)->right), string, isFnc);               //pokud ASCII hodnota jmena vetsi, rekurzivne se zavola pro pravy podstrom
                return (error == ERROR_PASSED) ? ERROR_PASSED : ERROR_COMPILER;
            }
        }
    } else{                                                     //pokud je strom prazdny
        *tree = malloc(sizeof(bst_node_t));                         //vytvoreni misto pro novy uzel
        if (*tree == NULL){
            return ERROR_COMPILER;      //pokud se malloc nepovedl
        } else{
            Dynamic_string *string1 = (Dynamic_string *)malloc(sizeof(Dynamic_string)); //naalokuje DS
            if (string1 == NULL) return ERROR_COMPILER;
            (*tree)->name = string1;                //nahraj hodnoty
            (*tree)->name->str = string->str;
            (*tree)->isFnc = isFnc;
            if (isFnc){                         //pokud je strom pro funkce
                Table_data_func_t *dataFunc = malloc(sizeof(Table_data_func_t));    //naalokuje strukturu s daty pro funkci
                if (dataFunc == NULL){              //kdyz se nenalokuje
                    free((*tree)->name);        //smaze DS se jmenem
                    free(*tree);            //smaze uzel
                    *tree = NULL;
                    return ERROR_COMPILER;
                }
                (*tree)->funcData = dataFunc;                   //nastavi ukazatel na structuru s daty pro funkci
                SLList_Param *listParam = (SLList_Param *)malloc(sizeof(SLList_Param));             //naalokuje seznam paranetru
                if (listParam == NULL) return ERROR_COMPILER;
                SLL_Param_Init(listParam);                                          //inicializuje seznam
                (*tree)->funcData->paramList = listParam;                           //nastavi ukazatele na seznamy
                SLList_Return *listReturn = (SLList_Return *)malloc(sizeof(SLList_Return));         //naalokuje seznam navratovych typu
                if (listReturn == NULL) return ERROR_COMPILER;
                SLL_Return_Init(listReturn);                                        //inicializuje seznam
                (*tree)->funcData->returnList = listReturn;
                (*tree)->varData = NULL;                                            //ukazatel na nepotrebnou strukturu nastavi na null
            } else{
                Table_data_var_t *dataVar = malloc(sizeof(Table_data_var_t));   //naalokuje strukturu s daty pro promenne
                if (dataVar == NULL) {              //kdyz se nenalokuje
                    free((*tree)->name);        //smaze DS se jmenem
                    free(*tree);            //smaze uzel
                    *tree = NULL;
                    return ERROR_COMPILER;
                }
                (*tree)->varData = dataVar;                 //nastavi ukazatel na strukturu s daty pro promenne
                (*tree)->funcData = NULL;                                       //ukazatel na nepotrebnou strukturu nastavi na null
            }
            (*tree)->left = NULL;
            (*tree)->right = NULL;
            return ERROR_PASSED;
        }
    }
}

/**
 * Funkce pro celkove smazani Binarniho vyhledavaciho stromu.
 *
 * @param tree Ukazatel na koren BVS.
 */
void bst_dispose(bst_node_t **tree){
    if (*tree != NULL){                         //dokud strom neni prazdny
        bst_dispose(&((*tree)->left));                  //rekurzivne se zavola pro levy podstrom
        bst_dispose(&((*tree)->right));                 //rekurzivne se zavola pro pravy podstrom
        if ((*tree)->isFnc){                                    //pokud je strom pro funkce
            SLL_Param_Dispose((*tree)->funcData->paramList);      //smaze seznam paranetru
            SLL_Return_Dispose((*tree)->funcData->returnList);      //smaze seznam navratovych typu
            free((*tree)->funcData->paramList);                 //uvolni ukazatele
            free((*tree)->funcData->returnList);
            (*tree)->funcData->returnList = NULL;
            (*tree)->funcData->paramList = NULL;
            free((*tree)->funcData);            //smaze strukturu s daty pro funkci
            (*tree)->funcData = NULL;
        } else{
            free((*tree)->varData);             //smaze strukturu s daty pro promennou
            (*tree)->varData = NULL;
        }
        free((*tree)->name);        //smaze DS se jmenem
        free(*tree);            //smaze uzel
        *tree = NULL;       //inicializuje strom
    } else{
        return;
    }
}

/**
 * Funkce pro prochazeni Binarniho vyhledavaciho stromu.
 *
 * @param tree Ukazatel na koren BVS.
 */
void bst_preorder(bst_node_t *tree, DLList_Instruct *listInstruct) {
    if (tree != NULL){
        DLL_Instruct_InsertFirst(listInstruct, tree->name);
        bst_preorder(tree->left, listInstruct);
        bst_preorder(tree->right, listInstruct);      //vytiskne uzel, rekurzivne se zavola pro levy a pravy podstrom
    } else{
        return;
    }
}

/**
 * Funkce pro inicializaci Tabulky symbolu.
 *
 * @param listFrame Ukazatel na Tabulku symbolu.
 */
void SLL_Frame_Init(SLList_Frame *listFrame){
    listFrame->TopLocalElement = NULL;
    listFrame->globalElement = NULL;
}

/**
 * Funkce pro pridani Framu do Tabulky symbolu.
 *
 * @param listFrame Ukazatel na Tabulku symbolu.
 * @return Vraci ERROR_PASSED, jestlize se pridani povedlo, jinak ERROR_COMPILER.
 */
int SLL_Frame_Insert(SLList_Frame *listFrame){
    SLLElementPtr_Frame nElemPtr = (SLLElementPtr_Frame)malloc(sizeof(struct SLLElement_Frame));  //naalokuje prostor pro pridavany frame
    if (nElemPtr == NULL){
        return ERROR_COMPILER;      //pokud se malloc nepovedl
    }
    bst_init(&(nElemPtr->node));        //inicializuje BVS
    if (listFrame->globalElement == NULL){          //pokud jeste neexistuje globalni frame
        nElemPtr->previousElement = NULL;
        listFrame->globalElement = nElemPtr;        //tak ho vytvori
    } else{
        if (listFrame->TopLocalElement == NULL) {                   //pokud jeste neexistuje localni frame
            nElemPtr->previousElement = listFrame->globalElement;
            listFrame->TopLocalElement = nElemPtr;                  //tak ho vytvori
        } else{
            nElemPtr->previousElement = listFrame->TopLocalElement;    //pokud uz existuje localni frame
            listFrame->TopLocalElement = nElemPtr;                  //prida ho na konec
        }
    }
    return ERROR_PASSED;
}

/**
 * Funkce pro smazani posledniho lokalniho Framu Tabulky symbolu.
 * Nesmaze globalni Frame.
 *
 * @param listFrame Ukazatel na Tabulku symbolu.
 */
 void SLL_Frame_Delete(SLList_Frame *listFrame){
     //pomocna promenna pro zjisteni posledniho framu
    bool isfrstLastElement = false;

    if (listFrame->TopLocalElement != NULL){     //jestli existuje localni prvek
        bst_dispose(&(listFrame->TopLocalElement->node));       //smaze BVS
        SLLElementPtr_Frame elemPtr;
        elemPtr = listFrame->TopLocalElement;           //pomocny ukazatel ukazuje na posledni
        if (listFrame->TopLocalElement->previousElement == listFrame->globalElement){   //jestlize je posledni localni frame
            isfrstLastElement = true;
        }
        listFrame->TopLocalElement = listFrame->TopLocalElement->previousElement;       //prvni se posune na dalsi
        free(elemPtr);                              //smaze posledni prvek
        if (isfrstLastElement){                     //jestlize je posledni localni frame
            listFrame->TopLocalElement = NULL;      //nastavy ukazatel na null
        }
    }
}

/**
 * Funkce pro smazani globalniho Framu Tabulky symbolu.
 * Jestlize existuje jeste nejaky lokalni Frame, nic neprovede.
 *
 * @param listFrame Ukazatel na Tabulku symbolu.
 */
void SLL_Frame_DeleteGlobal(SLList_Frame *listFrame){
    if (listFrame->TopLocalElement == NULL && listFrame->globalElement != NULL){     //jestli neexistuje localni frame a globalni ano
        bst_dispose(&(listFrame->globalElement->node));     //smaze BVS
        free(listFrame->globalElement);          //smaze globalni frame
        listFrame->globalElement = NULL;
        listFrame->TopLocalElement = NULL;
    }
}
/**
 * Funkce pro celkove smazani Tabulky symbolu.
 *
 * @param listFrame Ukazatel na Tabulku symbolu.
 */
void SLL_Frame_Dispose(SLList_Frame *listFrame){
    if (listFrame->TopLocalElement != NULL){    //pokud jsou nejake local framy
        while (listFrame->TopLocalElement->previousElement != listFrame->globalElement) {//dokud nezustal posledni local frame, tak maze framy
            bst_dispose(&(listFrame->TopLocalElement->node));           //smaze BVS
            SLLElementPtr_Frame elemPtr;
            elemPtr = listFrame->TopLocalElement;                       //pomocny ukazatel ukazuje na posledni
            listFrame->TopLocalElement = listFrame->TopLocalElement->previousElement;       //posledni se posune na predchozi
            free(elemPtr);          //smaze posledni local frame
        }
        if (listFrame->TopLocalElement->previousElement == listFrame->globalElement) { //smaze posledni local frame
            bst_dispose(&(listFrame->TopLocalElement->node));       //smaze BVS
            free(listFrame->TopLocalElement);       //smaze posledni local frame
            listFrame->TopLocalElement = NULL;
        }
    }
    if (listFrame->globalElement != NULL) {
        bst_dispose(&(listFrame->globalElement->node));     //smaze BVS
        free(listFrame->globalElement);             //smaze global frame
        listFrame->globalElement = NULL;
        listFrame->TopLocalElement = NULL;
    }
}

/**
 * Funkce pro vyhledani uzlu v cele Tabulce symbolu podle zadaneho jmena.
 * Pouziva funkci bst_search().
 *
 * @param string Ukazatel na DS, kde je jmeno hledaneho uzlu.
 * @param listFrame Ukazatel na Tabulku symbolu.
 * @return Vraci ukazatel na nalezeny ukazatel, jinak NULL.
 */
bst_node_t *search_Iden(Dynamic_string *string, SLList_Frame *listFrame){
    //pomocne ukazatele
    SLLElementPtr_Frame elemPtr = NULL;
    elemPtr = listFrame->TopLocalElement;
    SLLElementPtr_Frame pElemPtr = NULL;
    bst_node_t *treePtr;

    if (listFrame->TopLocalElement != NULL){
        while (elemPtr->previousElement != listFrame->globalElement){ //hleda v lokalnich framech
            pElemPtr = elemPtr->previousElement;
            if((treePtr = bst_search(elemPtr->node, string))){      //prohleda jejich BVS
                return treePtr;
            } else{
                elemPtr = pElemPtr;
            }
        }
        if (elemPtr->previousElement == listFrame->globalElement){ //hleda v prvnim lokalnim framu
            if ((treePtr = bst_search(elemPtr->node, string))){  //prohleda jeho BVS
                return treePtr;
            }
        }
    }
    if(listFrame->globalElement != NULL){                                           //prohleda globalni frame
        if ((treePtr = bst_search(listFrame->globalElement->node, string))){          //prohleda jeho BVS
            return treePtr;
        }
    }
    return NULL;            //pokud nenasel ani jeden uzel
}

void getAllVar(DLList_Instruct *listInstruct, SLList_Frame *listFrame){
    SLList_Frame *elemPtr;
    elemPtr = listFrame;
    SLList_Frame *pElemPtr = (SLList_Frame *) malloc(sizeof(SLList_Frame));
    pElemPtr->globalElement = listFrame->globalElement;
    while (elemPtr->TopLocalElement->previousElement != listFrame->globalElement){
        pElemPtr->TopLocalElement = elemPtr->TopLocalElement->previousElement;
        bst_preorder(elemPtr->TopLocalElement->node, listInstruct);
        elemPtr = pElemPtr;
    }
    bst_preorder(elemPtr->TopLocalElement->node, listInstruct);
    free(pElemPtr);
}
/**
 * Funkce pro ziskani dat, jestli je uzel pro funkci nebo pro promenne
 * .
 *
 * @param tree Ukazatel na uzel, ze ktereho je chteno ziskat hodnoty.
 * @return Vraci hodnotu true, jestlize je uzel pro funkci, jinak false.
 */
bool isFnc(bst_node_t *tree){
    bool isFnc = tree->isFnc;
    return isFnc;
}

/**
 * Funkce pro ziskani dat, jestli je funkce definovana.
 *
 * @param tree Ukazatel na uzel, ze ktereho je chteno ziskat hodnoty.
 * @return Vraci hodnotu true, jestlize je funkce definovana, jinak false.
 */
bool isDecFnc(bst_node_t *tree){
    bool isDecFnc = tree->funcData->dec;
    return isDecFnc;
}

/**
 * Funkce pro ziskani dat, jestli je funkce definovana.
 *
 * @param tree Ukazatel na uzel, ze ktereho je chteno ziskat hodnoty.
 * @return Vraci hodnotu true, jestlize je funkce definovana, jinak false.
 */
bool isDefFnc(bst_node_t *tree){
    bool isDefFnc = tree->funcData->def;
    return isDefFnc;
}

/**
 * Funkce pro ziskani dat, jestli je promenna inicializovana.
 *
 * @param tree Ukazatel na uzel, ze ktereho je chteno ziskat hodnoty.
 * @return Vraci hodnotu true, jestlize je promenna inicializovana, jinak false.
 */
bool isInitVar(bst_node_t *tree){
    bool isInitVar = tree->varData->init;
    return isInitVar;
}

/**
 * Funkce pro ziskani dat, jestli je promenna pouzita.
 *
 * @param tree Ukazatel na uzel, ze ktereho je chteno ziskat hodnoty.
 * @return Vraci hodnotu true, jestlize je promenna pouzita, jinak false.
 */
bool isUsedVar(bst_node_t *tree) {
    bool isUsedVar = tree->varData->used;
    return isUsedVar;
}

/**
 * Funkce pro ziskani datoveho typu promenne
 * .
 *
 * @param tree Ukazatel na uzel, ze ktereho je chteno ziskat hodnoty.
 * @return Vraci hodnotu datoveho typu promenne
 * .
 */
Data_type typeVar(bst_node_t *tree) {
    Data_type typeVar = tree->varData->type;
    return typeVar;
}

/**
 * Funkce pro pridani vestavenych funkci do global Framu.
 *
 * @param listFrame Ukazatel na Tabulku symbolu.
 * @return Vraci ERROR_PASSED, jestlize se pridani povedlo, jinak ERROR_COMPILER.
 */
int setBuildInFun(SLList_Frame *listFrame, Dynamic_string *string, Data_type param1, Data_type param2, Data_type param3, Data_type return1){
    int error = 0;
    bst_node_t *tree;
    if ((bst_insert(&(listFrame->globalElement->node), string, true)) == ERROR_COMPILER){       //prida uzel s vestavenou funkci
        return ERROR_COMPILER;
    }
    tree = search_Iden(string, listFrame);
    setFncDef(tree, true);                       //nastavi ji na definovanou
    if (param1 != TYPE_UNDEFINED){
        error = SLL_Param_Insert(param1, string, tree);
    }
    if (!error && (param2 != TYPE_UNDEFINED)){
        error = SLL_Param_Insert(param2, string, tree);
    }
    if (!error && (param3 != TYPE_UNDEFINED)){
        error = SLL_Param_Insert(param3, string, tree);
    }
    if (!error && (return1 != TYPE_UNDEFINED)){
        error = SLL_Return_Insert(return1, tree);
    }
    return error;
}