/**
 * Projekt: IFJ2021
 *
 * @brief Implementace DLListu pro instrukce.
 *
 * @author Vít Janeček xjanec30@stud.fit.vutbr.cz
*/

#ifndef _INSTRUCT_DLLIST_H

#include <stdio.h>
#include <stdbool.h>
#include "Dynamic_string.h"
#include "error.h"

typedef struct DLLElement_Instruct {
    /** Dynamicky string s instrukci */
    Dynamic_string *instruction;
    /** Ukazatel na předcházející prvek seznamu. */
    struct DLLElement_Instruct *previousElement;
    /** Ukazatel na následující prvek seznamu. */
    struct DLLElement_Instruct *nextElement;
} *DLLElementPtr_Instruct;

/** Dvousměrně vázaný seznam. */
typedef struct {
    /** Pocet prvku. */
    int count;
    /** Ukazatel na první prvek seznamu. */
    DLLElementPtr_Instruct firstElement;
    /** Ukazatel na aktuální prvek seznamu. */
    DLLElementPtr_Instruct activeElement;
    /** Ukazatel na source prvek seznamu. */
    DLLElementPtr_Instruct sourceElement;
    /** Ukazatel na destination prvek seznamu. */
    DLLElementPtr_Instruct destinationElement;
    /** Ukazatel na posledni prvek seznamu. */
    DLLElementPtr_Instruct lastElement;
} DLList_Instruct;

void DLL_Instruct_Init( DLList_Instruct * );

void DLL_Instruct_Dispose( DLList_Instruct * );

int DLL_Instruct_InsertFirst( DLList_Instruct *, Dynamic_string *);

int DLL_Instruct_InsertLast( DLList_Instruct *, Dynamic_string *);

void DLL_Instruct_First( DLList_Instruct * );

void DLL_Instruct_Last( DLList_Instruct * );

void DLL_Instruct_GetFirst( DLList_Instruct *, Dynamic_string *);

void DLL_Instruct_GetLast( DLList_Instruct *, Dynamic_string *);

void DLL_Instruct_DeleteFirst( DLList_Instruct * );

void DLL_Instruct_DeleteLast( DLList_Instruct * );

void DLL_Instruct_DeleteAfter( DLList_Instruct * );

void DLL_Instruct_DeleteBefore( DLList_Instruct * );

int DLL_Instruct_InsertAfter( DLList_Instruct *, Dynamic_string *);

int DLL_Instruct_InsertBefore( DLList_Instruct *, Dynamic_string *);

void DLL_Instruct_GetValue( DLList_Instruct *, Dynamic_string * );

void DLL_Instruct_SetValue( DLList_Instruct *, Dynamic_string * );

void DLL_Instruct_Next( DLList_Instruct * );

void DLL_Instruct_Previous( DLList_Instruct * );

void DLL_Instruct_setSource(DLList_Instruct *, int position);

void DLL_Instruct_setDestination(DLList_Instruct *, int position);

int DLL_Instruct_Move(DLList_Instruct *, int countOfElem, int positionOFDestination, int positionOFSource);

#define _INSTRUCT_DLLIST_H

#endif //IFJ_PROJECT_INSTRUCT_DLLIST_H

/** @endcode */