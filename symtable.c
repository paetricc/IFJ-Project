#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "symtable.h"

void SLL_Param_Init(SLList_Param *listParam){
    listParam->lastElement = NULL;
}

void SLL_Param_Insert(SLList_Param *listParam, Data_type_local type, Dynamic_string *stringName, bst_node_t *tree){
    SLLElementPtr_Param nElemPtr = (SLLElementPtr_Param)malloc(sizeof(struct SLLElement_Param));  //naalokuje prostor pro pridavany prvek
    if (nElemPtr == NULL){
        return;
    }
    if (tree->funcData->paramList->lastElement == NULL) {
        nElemPtr->type = type;
        if (!(DS_Copy(stringName, nElemPtr->name))){
            free(stringName);
        }
        nElemPtr->previousElement = NULL;
        tree->funcData->paramList->lastElement = nElemPtr;
    }
    if (listParam->lastElement != NULL) {
        nElemPtr->type = type;
        if (!(DS_Copy(stringName, nElemPtr->name))){
            free(stringName);
        }
        nElemPtr->previousElement = tree->funcData->paramList->lastElement;          //prenastavi ukazatele
        tree->funcData->paramList->lastElement = nElemPtr;
    }
}

void SLL_Param_Dispose(SLList_Param *listParam){
    while (listParam->lastElement != NULL){                     //dokud prvni element neni NULL
        SLLElementPtr_Param elemPtr;
        free(&(listParam->lastElement->name));
        elemPtr = listParam->lastElement;                       //pomocny ukazatel ukazuje na 1. prvek
        listParam->lastElement = listParam->lastElement->previousElement;       //prvni se posune na dalsi
        free(elemPtr);          //smaze prvni prvek
    }
    listParam->lastElement = NULL;
}

void SLL_Return_Init(SLList_Return *listReturn){
    listReturn->lastElement = NULL;
}

void SLL_Return_Insert(SLList_Return *listReturn, Data_type_local type){
    SLLElementPtr_Return nElemPtr = (SLLElementPtr_Return)malloc(sizeof(struct SLLElement_Return));  //naalokuje prostor pro pridavany prvek
    if (nElemPtr == NULL){
        return;
    }
    if (listReturn->lastElement == NULL) {
        nElemPtr->type = type;
        nElemPtr->previousElement = NULL;
        listReturn->lastElement = nElemPtr;
    }
    if (listReturn->lastElement != NULL) {
        nElemPtr->type = type;
        nElemPtr->previousElement = listReturn->lastElement;          //prenastavi ukazatele
        listReturn->lastElement = nElemPtr;
    }
}

void SLL_Return_Dispose(SLList_Return *listReturn){
    while (listReturn->lastElement != NULL){                     //dokud prvni element neni NULL
        SLLElementPtr_Return elemPtr;
        elemPtr = listReturn->lastElement;                       //pomocny ukazatel ukazuje na 1. prvek
        listReturn->lastElement = listReturn->lastElement->previousElement;       //prvni se posune na dalsi
        free(elemPtr);          //smaze prvni prvek
    }
    listReturn->lastElement = NULL;
}

void setDataV(bst_node_t *tree, Data_type_local type, bool init, bool used){
    tree->varData->type = type;
    tree->varData->init = init;
    tree->varData->used = used;
}

void setDataF(bst_node_t *tree, SLList_Param *listParam, SLList_Return *listReturn, bool def){
    SLL_Param_Init(listParam);
    tree->funcData->paramList = listParam;
    SLL_Return_Init(listReturn);
    tree->funcData->returnList = listReturn;
    tree->funcData->def = def;
}

void bst_init(bst_node_t **tree) {
    *tree = NULL;
}

bool bst_search(bst_node_t *tree, bst_node_t *findNode, Dynamic_string *string) {
    if (tree != NULL){                                               //dokud nedosel nakonec
        if ((strcmp(tree->name->str, string->str)) == 0){//pokud jsou klice stejne, nahraje hodnotu a vrati true
            findNode = tree;
            return true;
        } else{
            if ((strcmp(tree->name->str, string->str)) > 0){
                return bst_search(tree->left, findNode,string);          //pokud je klic mensi, rekurzivne se zavola pro levy podstrom
            } else{
                return bst_search(tree->right, findNode,string);         //pokud je klic vetsi, rekurzivne se zavola pro pravy podstrom
            }
        }
    } else{
        findNode = NULL;
        return false;                                               // jestli nenasel
    }
}

void bst_insert(bst_node_t **tree, Dynamic_string *string, bool isFnc) {
    if (*tree != NULL){                                         //dokud nedosel nakonec
        if ((strcmp((*tree)->name->str, string->str)) == 0){//pokud jsou klice stejne, nahraje hodnotu a vrati true
            //error
            return;
        } else{
            if ((strcmp((*tree)->name->str, string->str)) > 0){
                bst_insert(&((*tree)->left), string, isFnc);       //pokud je klic mensi, rekurzivne se zavola pro levy podstrom
                return;
            } else{
                bst_insert(&((*tree)->right), string, isFnc);      //pokud je klic vetsi, rekurzivne se zavola pro pravy podstrom
                return;
            }
        }
    } else{                                                     //pokud je strom prazdny
        *tree = malloc(sizeof(bst_node_t));                     //vytvoreni mista pro novy uzel
        if (*tree == NULL){
            return;
        } else{
            (*tree)->name->str = string->str;//inicializuje uzel
            (*tree)->isFnc = isFnc;
            if (isFnc){
                (*tree)->funcData = malloc(sizeof(Table_data_func_t));
                (*tree)->varData = NULL;
            } else{
                (*tree)->varData = malloc(sizeof(Table_data_var_t));
                (*tree)->funcData = NULL;
            }
            (*tree)->left = NULL;
            (*tree)->right = NULL;
            return;
        }
    }
}

void bst_dispose(bst_node_t **tree){
    if (*tree != NULL){                                 //dokud srom neni prazdny
        bst_dispose(&((*tree)->left));                  //rekurzivne se zavola pro levy podstrom
        bst_dispose(&((*tree)->right));                 //rekurzivne se zavola pro pravy podstrom
        free(*tree);                                    //smaze uzel
        *tree = NULL;                                   //inicializuje strom
    } else{
        return;
    }
}

void SLL_Frame_Init(SLList_Frame *listFrame){
    listFrame->lastElement = NULL;
    listFrame->globalElement = NULL;
}

void SLL_Frame_Insert(SLList_Frame *listFrame, bst_node_t *tree){
    SLLElementPtr_Frame nElemPtr = (SLLElementPtr_Frame)malloc(sizeof(struct SLLElement_Frame));  //naalokuje prostor pro pridavany prvek
    if (nElemPtr == NULL){
        return;
    }
    bst_init(&tree);
    nElemPtr->node = tree;         //nahraje data
    if (listFrame->globalElement == NULL){
        nElemPtr->previousElement = NULL;
        listFrame->globalElement = nElemPtr;
    }
    if (listFrame->lastElement == NULL) {
        nElemPtr->previousElement = listFrame->globalElement;
        listFrame->lastElement = nElemPtr;
    }
    if (listFrame->lastElement != NULL) {
        nElemPtr->previousElement = listFrame->lastElement;          //prenastavi ukazatele
        listFrame->lastElement = nElemPtr;
    }
}

void SLL_Frame_Delete(SLList_Frame *listFrame){
    if (listFrame->lastElement != NULL){     //jestli existuje posledni prvek
        SLLElementPtr_Frame elemPtr;
        elemPtr = listFrame->lastElement;            //pomocny ukazatel ukazuje na posledni prvek
        if (listFrame->globalElement == listFrame->lastElement){
            listFrame->lastElement = NULL;         //ztraci aktivitu
        } else{
            bst_dispose(&(listFrame->lastElement->node));
            listFrame->lastElement = listFrame->lastElement->previousElement; //posune posledni prvek na predchozi
            free(elemPtr);          //uvolni predesli posledni prvek
        }
    }
    return;
}
void SLL_Frame_DeleteGlobal(SLList_Frame *listFrame){
    if (listFrame->lastElement == NULL && listFrame->globalElement != NULL){     //jestli existuje posledni prvek
        bst_dispose(&(listFrame->globalElement->node));
        free(listFrame->globalElement);          //uvolni global
        listFrame->globalElement = NULL;
    }
    return;
}

void SLL_Frame_Dispose(SLList_Frame *listFrame){
    while (listFrame->lastElement->previousElement != listFrame->globalElement){//dokud prvni element neni NULL
        bst_dispose(&(listFrame->lastElement->node));
        SLLElementPtr_Frame elemPtr;
        elemPtr = listFrame->lastElement;                       //pomocny ukazatel ukazuje na 1. prvek
        listFrame->lastElement = listFrame->lastElement->previousElement;       //prvni se posune na dalsi
        free(elemPtr);          //smaze prvni prvek
    }
    if (listFrame->lastElement->previousElement == listFrame->globalElement){
        bst_dispose(&(listFrame->lastElement->node));
        free(listFrame->lastElement);
    }
    bst_dispose(&(listFrame->globalElement->node));
    free(listFrame->globalElement);
    listFrame->globalElement = NULL;
    listFrame->lastElement = NULL;
}

bst_node_t *search_Iden(Dynamic_string *string, SLList_Frame *listFrame){
    SLList_Frame *elemPtr;
    elemPtr = listFrame;
    SLList_Frame *pElemPtr;
    bst_node_t *treePtr;
    if (listFrame->lastElement != NULL){
        while (elemPtr->lastElement->previousElement != elemPtr->globalElement){
            pElemPtr->lastElement = elemPtr->lastElement->previousElement;
            if(bst_search(elemPtr->lastElement->node, treePtr,string)){
                return treePtr;
            } else{
                elemPtr = pElemPtr;
            }
        }
        if (elemPtr->lastElement->previousElement == elemPtr->globalElement){
            if(bst_search(elemPtr->lastElement->node, treePtr, string)){
                return treePtr;
            }
        }
    }
    if(listFrame->globalElement != NULL){
        elemPtr = listFrame;
        if(bst_search(elemPtr->globalElement->node, treePtr,string)){
            return treePtr;
        }
    }
    treePtr = NULL;
    return treePtr;
}

Table_data_func_t *getDataF(bst_node_t *tree){
    /**Table_data_func_t *elemPtr;
    if (tree == NULL){
        return elemPtr = NULL;
    } else{
        elemPtr->def = tree->funcData->def;
        elemPtr->paramList = tree->funcData->paramList;
        elemPtr->returnList = tree->funcData->returnList;
        return elemPtr;
    }**/
    return tree->funcData;
}

Table_data_var_t *getDataV(bst_node_t *tree){
    return tree->varData;
}