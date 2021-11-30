#ifndef _SYMTABLE_H
#define _SYMTABLE_H

#include <stdio.h>
#include <stdbool.h>
#include "Dynamic_string.h"
#include "error.h"

typedef enum {
    TYPE_UNDEFINED,
    TYPE_INTEGER,
    TYPE_NUMBER,
    TYPE_STRING,
}Data_type;

typedef struct SLLElement_Param {
    Data_type type;
    Dynamic_string *name;
    /** Ukazatel na předcházející prvek seznamu. */
    struct SLLElement_Param *nextElement;
} *SLLElementPtr_Param;

typedef struct {
    /** Ukazatel na první prvek seznamu. */
    SLLElementPtr_Param firstElement;
} SLList_Param;

typedef struct SLLElement_Return {
    Data_type type;
    /** Ukazatel na předcházející prvek seznamu. */
    struct SLLElement_Return *nextElement;
} *SLLElementPtr_Return;

typedef struct {
    /** Ukazatel na první prvek seznamu. */
    SLLElementPtr_Return firstElement;
} SLList_Return;

typedef struct Table_data_var{
    Data_type type;
    bool init;
    bool used;
}Table_data_var_t;

typedef struct Table_data_func{
    SLList_Param *paramList;
    SLList_Return *returnList;
    bool def;
} Table_data_func_t;

typedef struct bst_node{
    Dynamic_string *name;
    bool isFnc;
    struct Table_data_var *varData;
    struct Table_data_func *funcData;
    struct bst_node *left;
    struct bst_node *right;
} bst_node_t;

typedef struct SLLElement_Frame {
    struct bst_node *node;
    /** Ukazatel na předcházející prvek seznamu. */
    struct SLLElement_Frame *previousElement;
} *SLLElementPtr_Frame;

typedef struct {
    /** Ukazatel na posledni prvek seznamu. */
    SLLElementPtr_Frame TopLocalElement;
    /** Ukazatel na global prvek seznamu. */
    SLLElementPtr_Frame globalElement;
} SLList_Frame;


void SLL_Param_Init(SLList_Param *listParam); //done

int SLL_Param_Insert(Data_type type, Dynamic_string *stringName, bst_node_t *tree); //done

void SLL_Param_Dispose(SLList_Param *listParam);//<-----------------------------------------------------------------

void SLL_Return_Init(SLList_Return *listReturn); //done

int SLL_Return_Insert(Data_type type, bst_node_t *tree); //done

void SLL_Return_Dispose(SLList_Return *listReturn);//<--------------------------------------------------------------

void bst_init(bst_node_t **tree); //done

bst_node_t *bst_search(bst_node_t *tree, Dynamic_string *string); //done

int bst_insert(bst_node_t **tree, Dynamic_string *string, bool isFnc); //done

void bst_dispose(bst_node_t **tree);//<-----------------------------------------------------------------------------

void bst_preorder(bst_node_t *tree);

void SLL_Frame_Init(SLList_Frame *listFrame); //done

int SLL_Frame_Insert(SLList_Frame *listFrame); //done

void SLL_Frame_Dispose(SLList_Frame *listFrame);//<-----------------------------------------------------------------

void SLL_Frame_Delete(SLList_Frame *listFrame);//<------------------------------------------------------------------

void SLL_Frame_DeleteGlobal(SLList_Frame *listFrame);//<------------------------------------------------------------

bst_node_t *search_Iden(Dynamic_string *, SLList_Frame *listFrame); //done

void setDataF(bst_node_t *tree, bool); //done

void setDataV(bst_node_t *tree, Data_type type, bool init, bool used); //done

bool isFnc(bst_node_t *tree); // -jestli je to funkce nebo var
bool isDefFnc(bst_node_t *tree); //-jestli je na uzlu, danna fce defin
bool isInitVar(bst_node_t *tree); // -jestli je na uzlu, danna fce defin
bool isUsedVar(bst_node_t *tree); //-jestli je na uzlu, danna fce defin
Data_type typeVar(bst_node_t *tree); //- vrati typ promene

//nejake funkce na vraceni hodnoty parametru a returnu

#endif
