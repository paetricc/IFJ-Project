#ifndef _SYMTABLE_H
#define _SYMTABLE_H

#include <stdio.h>
#include <stdbool.h>
#include "Dynamic_string.h"

typedef enum {
    TYPE_UNDEFINED,
    TYPE_INTEGER,
    TYPE_NUMBER,
    TYPE_STRING,
}Data_type_local;

typedef struct SLLElement_Param {
    Data_type_local type;
    Dynamic_string *name;
    /** Ukazatel na předcházející prvek seznamu. */
    struct SLLElement_Param *previousElement;
} *SLLElementPtr_Param;

typedef struct {
    /** Ukazatel na první prvek seznamu. */
    SLLElementPtr_Param lastElement;
} SLList_Param;

typedef struct SLLElement_Return {
    Data_type_local type;
    /** Ukazatel na předcházející prvek seznamu. */
    struct SLLElement_Return *previousElement;
} *SLLElementPtr_Return;

typedef struct {
    /** Ukazatel na první prvek seznamu. */
    SLLElementPtr_Return lastElement;
} SLList_Return;

typedef struct Table_data_var{
    Data_type_local type;
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
    /** Ukazatel na první prvek seznamu. */
    SLLElementPtr_Frame lastElement;
    /** Ukazatel na aktuální prvek seznamu. */
    SLLElementPtr_Frame globalElement;
} SLList_Frame;


void SLL_Param_Init(SLList_Param *listParam);

void SLL_Param_Insert(SLList_Param *listParam, Data_type_local type, Dynamic_string *stringName);

void SLL_Param_Dispose(SLList_Param *listParam);

void SLL_Return_Init(SLList_Return *listReturn);

void SLL_Return_Insert(SLList_Return *listReturn, Data_type_local type);

void SLL_Return_Dispose(SLList_Return *listReturn);

void bst_init(bst_node_t **tree);

bool bst_search(bst_node_t *tree, Dynamic_string *string);

void bst_insert(bst_node_t **tree, Dynamic_string *string, Table_data_func_t *dataFunc, Table_data_var_t *dataVar, bool isFnc);

void bst_dispose(bst_node_t **tree);

void SLL_Frame_Init(SLList_Frame *listFrame);

void SLL_Frame_Insert(SLList_Frame *listFrame, bst_node_t *tree);

void SLL_Frame_Dispose(SLList_Frame *listFrame);

void SLL_Frame_Delete(SLList_Frame *listFrame);

void SLL_Frame_DeleteGlobal(SLList_Frame *listFrame);

bst_node_t *search_Iden(Dynamic_string *, SLList_Frame *listFrame);

Table_data_func_t *getDataF(bst_node_t *, SLList_Frame *listFrame);

Table_data_var_t *getDataV(bst_node_t *, SLList_Frame *listFrame);

void setDataF(Table_data_func_t *dataFunc, SLList_Param *listParam, SLList_Return *listReturn, bool);

void setDataV(Table_data_var_t *dataVar, Data_type_local type, bool init, bool used);

#endif
