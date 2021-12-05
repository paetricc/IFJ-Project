/**
 * Projekt: IFJ2021
 *
 * @brief Implementace DLListu pro instrukce.
 *
 * @author Vít Janeček xjanec30@stud.fit.vutbr.cz
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "instruct_dllist.h"


void DLL_Instruct_Init( DLList_Instruct * listInstruct){
    listInstruct->firstElement = NULL;              //inicalizuje prvni, aktivni a posledni prvek na NULL
    listInstruct->lastElement = NULL;
    listInstruct->activeElement = NULL;
    listInstruct->count = 0;
}

void DLL_Instruct_Dispose( DLList_Instruct * listInstruct){
    while (listInstruct->firstElement != NULL){                     //dokud prvni element neni NULL
        DLLElementPtr_Instruct elemPtr;
        elemPtr = listInstruct->firstElement;                       //pomocny ukazatel ukazuje na 1. prvek
        free(elemPtr->instruction);
        listInstruct->firstElement = listInstruct->firstElement->nextElement;       //prvni se posune na dalsi
        free(elemPtr);          //smaze prvni prvek
    }
    DLL_Instruct_Init(listInstruct);
}

int DLL_Instruct_InsertFirst( DLList_Instruct *listInstruct, Dynamic_string *string){
    DLLElementPtr_Instruct nElemPtr = (DLLElementPtr_Instruct)malloc(sizeof(struct DLLElement_Instruct));          //naalokuje prostor pro pridavany prvek
    Dynamic_string *string2 = (Dynamic_string *)malloc(sizeof(Dynamic_string));
    if (nElemPtr == NULL || string2 == NULL){
        return ERROR_COMPILER;        //kdyby nenaalokoval
    }
    nElemPtr->instruction = string2;          //nahraje data
    nElemPtr->instruction->str = string->str;
    nElemPtr->nextElement = listInstruct->firstElement;     //prenastavi ukazatele
    nElemPtr->previousElement = NULL;
    if(listInstruct->firstElement != NULL){
        listInstruct->firstElement->previousElement = nElemPtr;         //kdyz neni prvni prvek, prida novy prvek pred prvni prvek
    }else{
        listInstruct->lastElement = nElemPtr;           //jinak ho prida jako posledni prvek
    }
    listInstruct->firstElement = nElemPtr;          //nastavi novy prvek na prvni
    (listInstruct->count)++;
    return ERROR_PASSED;
}

int DLL_Instruct_InsertLast( DLList_Instruct *listInstruct, Dynamic_string *string){
    DLLElementPtr_Instruct nElemPtr = (DLLElementPtr_Instruct)malloc(sizeof(struct DLLElement_Instruct));  //naalokuje prostor pro pridavany prvek
    Dynamic_string *string2 = (Dynamic_string *)malloc(sizeof(Dynamic_string));
    if (nElemPtr == NULL || string == NULL){
        return ERROR_COMPILER;        //kdyby nenaalokoval
    }
    nElemPtr->instruction = string2;
    nElemPtr->instruction->str = string->str;
    nElemPtr->previousElement = listInstruct->lastElement;          //prenastavi ukazatele
    nElemPtr->nextElement = NULL;
    if(listInstruct->lastElement != NULL){
        listInstruct->lastElement->nextElement = nElemPtr;      //kdyz neni posledni prvek, prida novy prvek za posledni
    }else{
        listInstruct->firstElement = nElemPtr;          //jinak ho prida jako prvni prvek
    }
    listInstruct->lastElement = nElemPtr;           //nastavi novy prvek na posledni
    (listInstruct->count)++;        //inkrementuje counter
    return ERROR_PASSED;
}

void DLL_Instruct_First( DLList_Instruct *listInstruct){
    listInstruct->activeElement = listInstruct->firstElement;            //zmeni aktivitu na prvni prvek
}

void DLL_Instruct_Last( DLList_Instruct *listInstruct){
    listInstruct->activeElement = listInstruct->lastElement;            //zmeni aktivitu na posledni prvek
}

void DLL_Instruct_GetFirst( DLList_Instruct *listInstruct, Dynamic_string *string){
    if (listInstruct->firstElement == NULL){
        return;
    } else{
        *string = *(listInstruct->firstElement->instruction);            //ziska data od prvniho prvku
    }
}

void DLL_Instruct_GetLast( DLList_Instruct *listInstruct, Dynamic_string *string){
    if (listInstruct->lastElement == NULL){
        return;
    } else{
        *string = *(listInstruct->lastElement->instruction);            //ziska data od posledniho prvku
    }
}

void DLL_Instruct_DeleteFirst( DLList_Instruct *listInstruct){
    DLLElementPtr_Instruct elemPtr;
    if (listInstruct->firstElement != NULL){            //jestli existuje 1. prvek
        elemPtr = listInstruct->firstElement;               //pomocny ukazatel ukazuje na 1. prvek
        if (listInstruct->activeElement == listInstruct->firstElement){
            listInstruct->activeElement = listInstruct->firstElement->nextElement;             //ztraci aktivitu
        }
        if (listInstruct->firstElement == listInstruct->lastElement){           //zdali ma seznam jen jeden prvek
            listInstruct->firstElement = NULL;              //vyprazdni celi seznam
            listInstruct->lastElement = NULL;
        }else{
            listInstruct->firstElement = listInstruct->firstElement->nextElement;       //posune 1.prvek na nasledujici
            listInstruct->firstElement->previousElement = NULL;         //nastavy ukazatel na predesli 1. prvku na NULL
        }
        free(elemPtr->instruction);
        free(elemPtr);      //uvolni 1. prvek
        (listInstruct->count)--;
    }
}

void DLL_Instruct_DeleteLast( DLList_Instruct *listInstruct){
    if (listInstruct->lastElement != NULL){     //jestli existuje posledni prvek
        DLLElementPtr_Instruct elemPtr;
        elemPtr = listInstruct->lastElement;            //pomocny ukazatel ukazuje na posledni prvek
        if (listInstruct->activeElement == listInstruct->lastElement){
            listInstruct->activeElement = listInstruct->lastElement->previousElement;         //ztraci aktivitu
        }
        if (listInstruct->firstElement == listInstruct->lastElement){           //zdali ma seznam jen jeden prvek
            listInstruct->firstElement = NULL;          //vyprazdni celi seznam
            listInstruct->lastElement = NULL;
        }else{
            listInstruct->lastElement = listInstruct->lastElement->previousElement;         //posune posledni prvek na predchozi
            listInstruct->lastElement->nextElement = NULL;              //nastavy ukazatel na nasledujici posledni prvku na NULL
        }
        free(elemPtr->instruction);
        free(elemPtr);          //uvolni predesli posledni prvek
        (listInstruct->count)--;
    }
}

void DLL_Instruct_DeleteAfter( DLList_Instruct *listInstruct){
    if (listInstruct->activeElement != NULL){
        if (listInstruct->activeElement->nextElement != NULL){          //jestlize existuje nasledujici prvek
            DLLElementPtr_Instruct elemPtr;
            elemPtr = listInstruct->activeElement->nextElement;         //pomocny ukazatel ukazuje na nasledujici prvek za aktivnim
            listInstruct->activeElement->nextElement = elemPtr->nextElement;            //nastavi ukazatel na nasledujici az na dalsi za nasledujicim prvkem
            if (elemPtr == listInstruct->lastElement){
                listInstruct->lastElement = listInstruct->activeElement;            //jestli je mazany posledni, stane se posledni aktivnim
            } else{
                elemPtr->nextElement->previousElement = listInstruct->activeElement;        // jinak nastavi ukazatel na predesli prvku za smazanym na aktivni prvek
            }
            free(elemPtr->instruction);
            free(elemPtr);          //uvolni mazany prvek
            (listInstruct->count)--;
        }
    }
}

void DLL_Instruct_DeleteBefore( DLList_Instruct *listInstruct){
    if (listInstruct->activeElement != NULL){
        if (listInstruct->activeElement->previousElement != NULL){          //jestlize existuje predesli prvek
            DLLElementPtr_Instruct elemPtr;
            elemPtr = listInstruct->activeElement->previousElement;         //pomocny ukazatel ukazuje na predesli prvek pred aktivnim
            listInstruct->activeElement->previousElement = elemPtr->previousElement;        //nastavi ukazatel na predchozi  na prepredesli prvek
            if (elemPtr == listInstruct->firstElement){
                listInstruct->firstElement = listInstruct->activeElement;           //jestli je mazany prvni, stane se prvni aktivnim
            } else{
                elemPtr->previousElement->nextElement = listInstruct->activeElement;        // jinak nastavi ukazatel na nasledujici prvku pred smazanym na aktivni prvek
            }
            free(elemPtr->instruction);
            free(elemPtr);  //uvolni mazany prvek
            (listInstruct->count)--;
        }
    }
}

int DLL_Instruct_InsertAfter( DLList_Instruct *listInstruct, Dynamic_string *string){
    if(listInstruct->activeElement != NULL){
        DLLElementPtr_Instruct nElemPtr = (DLLElementPtr_Instruct)malloc(sizeof(struct DLLElement_Instruct));          //naalokuje prostor pro pridavany prvek
        Dynamic_string *string2 = (Dynamic_string *)malloc(sizeof(Dynamic_string));
        if (nElemPtr == NULL || string2 == NULL){          //kdyby nenaalokoval
            return ERROR_COMPILER;
        }
        nElemPtr->instruction = string2;              //nahraje data
        nElemPtr->instruction->str = string->str;
        nElemPtr->nextElement = listInstruct->activeElement->nextElement;           //prenastavi ukazatele
        nElemPtr->previousElement = listInstruct->activeElement;
        listInstruct->activeElement->nextElement = nElemPtr;            //prida novy prvek
        if (listInstruct->activeElement == listInstruct->lastElement){
            listInstruct->lastElement = nElemPtr;               //jestlize je nasledujici prvek posledni, novy prvek nastavi na posledni prvek
        } else{
            nElemPtr->nextElement->previousElement = nElemPtr;          //jinak nastavi ukazatel na prdechozi prvku za nevym prvkem na novy prvek
        }
        (listInstruct->count)++;
    }
    return ERROR_PASSED;
}

int DLL_Instruct_InsertBefore( DLList_Instruct *listInstruct, Dynamic_string *string){
    if(listInstruct->activeElement != NULL){
        DLLElementPtr_Instruct nElemPtr = (DLLElementPtr_Instruct)malloc(sizeof(struct DLLElement_Instruct));          //naalokuje prostor pro pridavany prvek
        Dynamic_string *string2 = (Dynamic_string *)malloc(sizeof(Dynamic_string));
        if (nElemPtr == NULL || string2 == NULL){          //kdyby nenaalokoval
            return ERROR_COMPILER;
        }
        nElemPtr->instruction = string2;              //nahraje data
        nElemPtr->instruction->str = string->str;
        nElemPtr->previousElement = listInstruct->activeElement->previousElement;       //prenastavi ukazatele
        nElemPtr->nextElement = listInstruct->activeElement;
        listInstruct->activeElement->previousElement = nElemPtr;            //prida novy prvek
        if (listInstruct->activeElement == listInstruct->firstElement){
            listInstruct->firstElement = nElemPtr;              //jestlize je predchozi prvek prvni, novy prvek nastavi na prvni prvek
        } else{
            nElemPtr->previousElement->nextElement = nElemPtr;          //jinak nastavi ukazatel na nasledujici prvku pred nevym prvkem na novy prvek
        }
        (listInstruct->count)++;
    }
    return ERROR_PASSED;
}

void DLL_Instruct_GetValue( DLList_Instruct *listInstruct, Dynamic_string *string){
    if(listInstruct->activeElement != NULL) {
        *string = *(listInstruct->activeElement->instruction);           //ziska data od aktivniho prvku, kdyz je aktivni
    } else{
        return;
    }
}

void DLL_Instruct_SetValue( DLList_Instruct *listInstruct, Dynamic_string *string){
    if(listInstruct->activeElement != NULL) {
        listInstruct->activeElement->instruction->str = string->str;           //nastavi data do aktivniho prvku, kdyz je aktivni
    }
}

void DLL_Instruct_Next( DLList_Instruct *listInstruct){
    if (listInstruct->activeElement == listInstruct->lastElement) {
        listInstruct->activeElement = NULL;         //nastavi na neaktivitu
    }
    if(listInstruct->activeElement != NULL) {
        listInstruct->activeElement = listInstruct->activeElement->nextElement;         //presune aktivitu
    }
}

void DLL_Instruct_Previous( DLList_Instruct *listInstruct){
    if (listInstruct->activeElement == listInstruct->firstElement) {
        listInstruct->activeElement = NULL;         //nastavi na neaktivitu
    }
    if(listInstruct->activeElement != NULL) {
        listInstruct->activeElement = listInstruct->activeElement->previousElement; //presune aktivitu
    }
}

void DLL_Instruct_setSource(DLList_Instruct *listInstruct, int position){
    if (position <= listInstruct->count){
        listInstruct->sourceElement = listInstruct->firstElement;
        if (position == 1){
            return;
        }
        for (int i = 0; i < position-1; i++) {
            listInstruct->sourceElement = listInstruct->sourceElement->nextElement;
        }
    }
}

void DLL_Instruct_setDestination(DLList_Instruct *listInstruct, int position){
    if (position <= listInstruct->count){
        listInstruct->destinationElement = listInstruct->firstElement;
        if (position == 1){
            return;
        }
        for (int i = 0; i < position-2; i++) {
            listInstruct->destinationElement = listInstruct->destinationElement->nextElement;
        }
    }
}

int DLL_Instruct_Move(DLList_Instruct *listInstruct, int countOfElem, int positionOfDestination, int positionOfSource){
    int isRdyToCpy = positionOfSource + countOfElem;
    int countOfElemForDes = countOfElem;
    if ((listInstruct->count+1) >= isRdyToCpy){
        DLLElementPtr_Instruct elemPtr = listInstruct->sourceElement;
        if (positionOfDestination == 1){
            DLL_Instruct_InsertFirst(listInstruct, listInstruct->sourceElement->instruction);
            countOfElemForDes--;
            listInstruct->activeElement = listInstruct->destinationElement->previousElement;
            elemPtr = elemPtr->nextElement;
        } else{
            listInstruct->activeElement = listInstruct->destinationElement;
        }
        for (int i = 0; i < countOfElemForDes; i++) {
            DLL_Instruct_InsertAfter(listInstruct,elemPtr->instruction);
            listInstruct->activeElement = listInstruct->activeElement->nextElement;
            elemPtr = elemPtr->nextElement;
        }
        listInstruct->activeElement = listInstruct->sourceElement->previousElement;
        if (positionOfSource == 1){
            countOfElem--;
            listInstruct->activeElement = listInstruct->sourceElement;
        }
        for (int i = 0; i < countOfElem; i++) {
            DLL_Instruct_DeleteAfter(listInstruct);
        }
        if (positionOfSource == 1){
            DLL_Instruct_DeleteFirst(listInstruct);
        }
    } else{
        return ERROR_COMPILER;
    }
    return ERROR_PASSED;
}
