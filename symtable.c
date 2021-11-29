#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "symtable.h"

void SLL_Param_Init(SLList_Param *listParam){
    listParam->topElement = NULL;
}

void SLL_Param_Insert(Data_type type, Dynamic_string *stringName, bst_node_t *tree){
    SLLElementPtr_Param nElemPtr = (SLLElementPtr_Param)malloc(sizeof(struct SLLElement_Param));  //naalokuje prostor pro pridavany prvek
    Dynamic_string *string1 = (Dynamic_string *)malloc(sizeof(Dynamic_string));
    if (nElemPtr == NULL){
        return;
    }
    nElemPtr->type = type;
    nElemPtr->name = string1;
    nElemPtr->name->str = stringName->str;
    if (tree->funcData->paramList->topElement == NULL) {
        nElemPtr->previousElement = NULL;
        tree->funcData->paramList->topElement = nElemPtr;
    } else {
        nElemPtr->previousElement = tree->funcData->paramList->topElement;          //prenastavi ukazatele
        tree->funcData->paramList->topElement = nElemPtr;
    }
}

void SLL_Param_Dispose(SLList_Param *listParam){
    while (listParam->topElement != NULL){                     //dokud prvni element neni NULL
        SLLElementPtr_Param elemPtr;
        free(listParam->topElement->name);
        elemPtr = listParam->topElement;                       //pomocny ukazatel ukazuje na 1. prvek
        listParam->topElement = listParam->topElement->previousElement;       //prvni se posune na dalsi
        free(elemPtr);          //smaze prvni prvek
    }
    listParam->topElement = NULL;
}

void SLL_Return_Init(SLList_Return *listReturn){
    listReturn->topElement = NULL;
}

void SLL_Return_Insert(Data_type type, bst_node_t *tree){
    SLLElementPtr_Return nElemPtr = (SLLElementPtr_Return)malloc(sizeof(struct SLLElement_Return));  //naalokuje prostor pro pridavany prvek
    if (nElemPtr == NULL){
        return;
    }
    nElemPtr->type = type;
    if (tree->funcData->returnList->topElement == NULL) {
        nElemPtr->previousElement = NULL;
        tree->funcData->returnList->topElement = nElemPtr;
    } else{
        nElemPtr->previousElement = tree->funcData->returnList->topElement;          //prenastavi ukazatele
        tree->funcData->returnList->topElement = nElemPtr;
    }
}

void SLL_Return_Dispose(SLList_Return *listReturn){
    while (listReturn->topElement != NULL){                     //dokud prvni element neni NULL
        SLLElementPtr_Return elemPtr;
        elemPtr = listReturn->topElement;                       //pomocny ukazatel ukazuje na 1. prvek
        listReturn->topElement = listReturn->topElement->previousElement;       //prvni se posune na dalsi
        free(elemPtr);          //smaze prvni prvek
    }
    listReturn->topElement = NULL;
}

void setDataV(bst_node_t *tree, Data_type type, bool init, bool used){
    tree->varData->type = type;
    tree->varData->init = init;
    tree->varData->used = used;
}

void setDataF(bst_node_t *tree, bool def){
    tree->funcData->def = def;
}

void bst_init(bst_node_t **tree) {
    *tree = NULL;
}

bst_node_t *bst_search(bst_node_t *tree, Dynamic_string *string) {
    bst_node_t *findNode = NULL;
    if (tree != NULL){                                               //dokud nedosel nakonec
        if ((strcmp(tree->name->str, string->str)) == 0){//pokud jsou klice stejne, nahraje hodnotu a vrati true
            findNode = tree;
            return findNode;
        } else{
            if ((strcmp(tree->name->str, string->str)) > 0){
                findNode = bst_search(tree->left, string);
                return findNode;          //pokud je klic mensi, rekurzivne se zavola pro levy podstrom
            } else{
                findNode = bst_search(tree->right, string);         //pokud je klic vetsi, rekurzivne se zavola pro pravy podstrom
                return findNode;
            }
        }
    } else{
        findNode = NULL;
        return findNode;                                               // jestli nenasel
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
            Dynamic_string *string1 = (Dynamic_string *)malloc(sizeof(Dynamic_string));
            (*tree)->name = string1;
            (*tree)->name->str = string->str;
            (*tree)->isFnc = isFnc;
            if (isFnc){
                Table_data_func_t *dataFunc = malloc(sizeof(Table_data_func_t));
                (*tree)->funcData = dataFunc;
                SLList_Param *listParam = (SLList_Param *)malloc(sizeof(SLList_Param));
                SLList_Return *listReturn = (SLList_Return *)malloc(sizeof(SLList_Return));
                SLL_Param_Init(listParam);
                SLL_Return_Init(listReturn);
                (*tree)->funcData->paramList = listParam;
                (*tree)->funcData->returnList = listReturn;
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
    if (*tree != NULL){                         //dokud srom neni prazdny
        bst_dispose(&((*tree)->left));                  //rekurzivne se zavola pro levy podstrom
        bst_dispose(&((*tree)->right));                 //rekurzivne se zavola pro pravy podstrom
        if ((*tree)->isFnc){
            SLL_Param_Dispose((*tree)->funcData->paramList);
            SLL_Return_Dispose((*tree)->funcData->returnList);
            free((*tree)->funcData->paramList);
            free((*tree)->funcData->returnList);
            (*tree)->funcData->returnList = NULL;
            (*tree)->funcData->paramList = NULL;
            free((*tree)->funcData);
            (*tree)->funcData = NULL;
        } else{
            free((*tree)->varData);
            (*tree)->varData = NULL;
        }
        free((*tree)->name);
        free(*tree);                                    //smaze uzel
        *tree = NULL;                                   //inicializuje strom
    } else{
        return;
    }
}

void bst_preorder(bst_node_t *tree) {
    if (tree != NULL){
        printf("%s\n", tree->name->str);
        bst_preorder(tree->left);
        bst_preorder(tree->right);      //vytiskne uzel, rekurzivne se zavola pro levy a pravy podstrom
    } else{
        return;
    }
}

void SLL_Frame_Init(SLList_Frame *listFrame){
    listFrame->TopLocalElement = NULL;
    listFrame->globalElement = NULL;
}

void SLL_Frame_Insert(SLList_Frame *listFrame){
    SLLElementPtr_Frame nElemPtr = (SLLElementPtr_Frame)malloc(sizeof(struct SLLElement_Frame));  //naalokuje prostor pro pridavany prvek
    if (nElemPtr == NULL){
        return;
    }
    bst_node_t *tree = malloc(sizeof (bst_node_t));
    bst_init(&tree);
    nElemPtr->node = tree;         //nahraje data
    if (listFrame->globalElement == NULL){
        nElemPtr->previousElement = NULL;
        listFrame->globalElement = nElemPtr;
    } else{
        if (listFrame->TopLocalElement == NULL) {
            nElemPtr->previousElement = listFrame->globalElement;
            listFrame->TopLocalElement = nElemPtr;
        } else{
            nElemPtr->previousElement = listFrame->TopLocalElement;          //prenastavi ukazatele
            listFrame->TopLocalElement = nElemPtr;
        }
    }
}

void SLL_Frame_Delete(SLList_Frame *listFrame){
    bool isfrstLastElement = false;
    if (listFrame->TopLocalElement != NULL){     //jestli existuje posledni prvek
        bst_dispose(&(listFrame->TopLocalElement->node));
        SLLElementPtr_Frame elemPtr;
        elemPtr = listFrame->TopLocalElement; //pomocny ukazatel ukazuje na 1. prvek
        if (listFrame->TopLocalElement->previousElement == listFrame->globalElement){
            isfrstLastElement = true;
        }
        listFrame->TopLocalElement = listFrame->TopLocalElement->previousElement;       //prvni se posune na dalsi
        free(elemPtr);
        if (isfrstLastElement){
            listFrame->TopLocalElement = NULL;
        }
    }
    return;
}
void SLL_Frame_DeleteGlobal(SLList_Frame *listFrame){
    if (listFrame->TopLocalElement == NULL && listFrame->globalElement != NULL){     //jestli existuje posledni prvek
        bst_dispose(&(listFrame->globalElement->node));
        //listFrame->globalElement->node = NULL;
        free(listFrame->globalElement);          //uvolni global
        listFrame->globalElement = NULL;
        listFrame->TopLocalElement = NULL;
    }
    return;
}

void SLL_Frame_Dispose(SLList_Frame *listFrame){
    if (listFrame->TopLocalElement != NULL){
        while (listFrame->TopLocalElement->previousElement != listFrame->globalElement) {//dokud prvni element neni NULL
            bst_dispose(&(listFrame->TopLocalElement->node));
            SLLElementPtr_Frame elemPtr;
            elemPtr = listFrame->TopLocalElement;                       //pomocny ukazatel ukazuje na 1. prvek
            listFrame->TopLocalElement = listFrame->TopLocalElement->previousElement;       //prvni se posune na dalsi
            free(elemPtr);          //smaze prvni prvek
        }
        if (listFrame->TopLocalElement->previousElement == listFrame->globalElement) {
            bst_dispose(&(listFrame->TopLocalElement->node));
            free(listFrame->TopLocalElement);
            listFrame->TopLocalElement = NULL;
        }
    }
    if (listFrame->globalElement != NULL) {
        bst_dispose(&(listFrame->globalElement->node));
        free(listFrame->globalElement);
        listFrame->globalElement = NULL;
        listFrame->TopLocalElement = NULL;
    }
}

bst_node_t *search_Iden(Dynamic_string *string, SLList_Frame *listFrame){

    SLList_Frame *elemPtr;
    elemPtr = listFrame;
    SLList_Frame *pElemPtr;
    bst_node_t *treePtr;

    if (listFrame->TopLocalElement != NULL){
        while (elemPtr->TopLocalElement->previousElement != elemPtr->globalElement){
            pElemPtr->TopLocalElement = elemPtr->TopLocalElement->previousElement;
            if((treePtr = bst_search(elemPtr->TopLocalElement->node, string))){
                return treePtr;
            } else{
                elemPtr = pElemPtr;
            }
        }
        if (elemPtr->TopLocalElement->previousElement == elemPtr->globalElement){
            if ((treePtr = bst_search(elemPtr->TopLocalElement->node, string))){
                return treePtr;
            }
        }
    }
    if(listFrame->globalElement != NULL){
        if ((treePtr = bst_search(elemPtr->globalElement->node, string))){
            return treePtr;
        }
    }
    return NULL;
}
bool isFnc(bst_node_t *tree){
    bool isFnc = tree->isFnc;
    return isFnc;
}
bool isDefFnc(bst_node_t *tree){
    bool isDefFnc = tree->funcData->def;
    return isDefFnc;
}
bool isInitVar(bst_node_t *tree){
    bool isInitVar = tree->varData->init;
    return isInitVar;
}
bool isUsedVar(bst_node_t *tree) {
    bool isUsedVar = tree->varData->used;
    return isUsedVar;
}
Data_type typeVar(bst_node_t *tree) {
    Data_type typeVar = tree->varData->type;
    return typeVar;
}