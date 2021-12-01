#ifndef _SYMTABLE_H
#define _SYMTABLE_H

#include <stdio.h>
#include <stdbool.h>
#include "Dynamic_string.h"
#include "error.h"

/**
 * @enum Typy promennych.
 */
typedef enum {
    TYPE_UNDEFINED,
    TYPE_INTEGER,
    TYPE_NUMBER,
    TYPE_STRING,
}Data_type;

/**
 * @struct Struktura reprezentujici data pro seznam parametru.
 */
typedef struct SLLElement_Param {
    /** Data parametru*/
    Data_type type;
    Dynamic_string *name;
    /** Ukazatel na nasledujici prvek seznamu. */
    struct SLLElement_Param *nextElement;
} *SLLElementPtr_Param;

/**
 * @struct Struktura reprezentujici seznam parametru.
 */
typedef struct {
    /** Ukazatel na první prvek seznamu. */
    SLLElementPtr_Param firstElement;
} SLList_Param;

/**
 * @struct Struktura reprezentujici data pro seznam navratovych typu.
 */
typedef struct SLLElement_Return {
    /** Data navratove hodnoty*/
    Data_type type;
    /** Ukazatel na nasledujici prvek seznamu. */
    struct SLLElement_Return *nextElement;
} *SLLElementPtr_Return;

/**
 * @struct Struktura reprezentujici seznam navratovych typu.
 */
typedef struct {
    /** Ukazatel na první prvek seznamu. */
    SLLElementPtr_Return firstElement;
} SLList_Return;

/**
 * @struct Struktura reprezentujici data pro promenne v BVS.
 */
typedef struct Table_data_var{
    /** Data pro promennou. */
    Data_type type;
    bool init;
    bool used;
}Table_data_var_t;

/**
 * @struct Struktura reprezentujici data pro funce v BVS.
 */
typedef struct Table_data_func{
    /** Ukazatel na seznam parametru*/
    SLList_Param *paramList;
    /** Ukazatel na seznam navratovych typu*/
    SLList_Return *returnList;
    /** Fce definovana*/
    bool def;
} Table_data_func_t;

/**
 * @struct Struktura reprezentujici uzel v Binarnim vyhledavacim strome.
 */
typedef struct bst_node{
    /** Data pro uzel*/
    Dynamic_string *name;
    bool isFnc;
    /** Ukazatel na structuru dat pro promennou*/
    struct Table_data_var *varData;
    /** Ukazatel na structuru dat pro funkci*/
    struct Table_data_func *funcData;
    /** Ukazatel na levy podstrom*/
    struct bst_node *left;
    /** Ukazatel na pravy podstrom*/
    struct bst_node *right;
} bst_node_t;

/**
 * @struct Struktura reprezentujici data pro Tabulku symbolu.
 */
typedef struct SLLElement_Frame {
    /** Ukazatel na koren BVS. */
    struct bst_node *node;
    /** Ukazatel na predchazejici prvek seznamu. */
    struct SLLElement_Frame *previousElement;
} *SLLElementPtr_Frame;

/**
 * @struct Struktura reprezentujici Tabulku szmbolu.
 */
typedef struct {
    /** Ukazatel na posledni prvek lokalnich Framu seznamu. */
    SLLElementPtr_Frame TopLocalElement;
    /** Ukazatel na globalni Frame seznamu. */
    SLLElementPtr_Frame globalElement;
} SLList_Frame;

void SLL_Param_Init(SLList_Param *listParam);

int SLL_Param_Insert(Data_type type, Dynamic_string *stringName, bst_node_t *tree);

void SLL_Param_Dispose(SLList_Param *listParam);

void SLL_Return_Init(SLList_Return *listReturn);

int SLL_Return_Insert(Data_type type, bst_node_t *tree);

void SLL_Return_Dispose(SLList_Return *listReturn);

void bst_init(bst_node_t **tree);

bst_node_t *bst_search(bst_node_t *tree, Dynamic_string *string);

int bst_insert(bst_node_t **tree, Dynamic_string *string, bool isFnc);

void bst_dispose(bst_node_t **tree);

void bst_preorder(bst_node_t *tree);

void SLL_Frame_Init(SLList_Frame *listFrame);

int SLL_Frame_Insert(SLList_Frame *listFrame);


void SLL_Frame_Dispose(SLList_Frame *listFrame);

void SLL_Frame_Delete(SLList_Frame *listFrame);

void SLL_Frame_DeleteGlobal(SLList_Frame *listFrame);

bst_node_t *search_Iden(Dynamic_string *, SLList_Frame *listFrame);

void setDataF(bst_node_t *tree, bool def);

void setDataV(bst_node_t *tree, Data_type type, bool init, bool used);

bool isFnc(bst_node_t *tree);

bool isDefFnc(bst_node_t *tree);

bool isInitVar(bst_node_t *tree);

bool isUsedVar(bst_node_t *tree);

Data_type typeVar(bst_node_t *tree);

//nejake funkce na vraceni hodnoty parametru a returnu

#endif
