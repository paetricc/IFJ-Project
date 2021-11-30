#ifndef _SYMTABLE_H
#define _SYMTABLE_H

#include <stdio.h>
#include <stdbool.h>
#include "Dynamic_string.h"
#include "error.h"

/**
 * @enum Typy prommenych.
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
 * @struct Struktura reprezentujici data pro prommene v BVS.
 */
typedef struct Table_data_var{
    Data_type type;
    bool init;
    bool used;
}Table_data_var_t;

/**
 * @struct Struktura reprezentujici data pro funce v BVS.
 */
typedef struct Table_data_func{
    SLList_Param *paramList;
    SLList_Return *returnList;
    bool def;
} Table_data_func_t;

/**
 * @struct Struktura reprezentujici uzel v Binarnim vyhledavacim strome.
 */
typedef struct bst_node{
    Dynamic_string *name;
    bool isFnc;
    struct Table_data_var *varData;
    struct Table_data_func *funcData;
    struct bst_node *left;
    struct bst_node *right;
} bst_node_t;

/**
 * @struct Struktura reprezentujici data pro Tabulku symbolu.
 */
typedef struct SLLElement_Frame {
    struct bst_node *node;
    /** Ukazatel na předcházející prvek seznamu. */
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

/**
 * Funkce pro inicializaci seznamu parametru.
 *
 * @param listParam Ukazatel na seznam parametru.
 */
void SLL_Param_Init(SLList_Param *listParam);

/**
 * Funkce pro pridani prvku do seznamu parametru.
 *
 * @param type Datovy typ pridavaneho parametru.
 * @param stringName Ukazatel na DS, kde je jmeno pridavaneho parametru.
 * @param tree Ukazatel na dany uzel, ve kterem je seznam parametru, do ktereho chceme pridavat prvek.
 * @return Vraci ERROR_PASSED, jestlize se pridani povedlo, jinak ERROR_COMPILER.
 */
int SLL_Param_Insert(Data_type type, Dynamic_string *stringName, bst_node_t *tree);

/**
 * Funkce pro celkove smazani seznamu parametru.
 *
 * @param listParam Ukazatel na seznam parametru.
 */
void SLL_Param_Dispose(SLList_Param *listParam);

/**
 * Funkce pro inicializaci seznamu navratovych typu.
 *
 * @param listReturn Ukazatel na seznam navratovych typu.
 */
void SLL_Return_Init(SLList_Return *listReturn);

/**
 * Funkce pro pridani prvku do seznamu navratovych typu.
 * @param type Datovy typ pridavaneho navratoveho typu.
 * @param tree Ukazatel na dany uzel, ve kterem je seznam navratovych typu, do ktereho chceme pridavat prvek.
 * @return Vraci ERROR_PASSED, jestlize se pridani povedlo, jinak ERROR_COMPILER.
 */
int SLL_Return_Insert(Data_type type, bst_node_t *tree);

/**
 * Funkce pro celkove smazani seznamu navratovych typu.
 *
 * @param listReturn Ukazatel na seznam navratovych typu.
 */
void SLL_Return_Dispose(SLList_Return *listReturn);

/**
 * Funkce pro inicializaci Binarniho vyhledavaciho stromu.
 *
 * @param tree Ukazatel na koren BVS.
 */
void bst_init(bst_node_t **tree);

/**
 * Funkce pro vyhledani uzlu v BVS podle zadaneho jmena.
 *
 * @param tree Ukazatel na koren Binarniho vyhledavaciho stromu.
 * @param string Ukazatel na DS, kde je jmeno hledaneho uzlu.
 * @return Vraci ukazatel na nalezeny ukazatel, jinak NULL.
 */
bst_node_t *bst_search(bst_node_t *tree, Dynamic_string *string);

/**
 * Funkce pro pridani uzlu v BVS podle zadaneho jmena.
 *
 * @param tree Ukazatel na koren Binarniho vyhledavaciho stromu.
 * @param string Ukazatel na DS, kde je jmeno noveho uzlu.
 * @param isFnc Hodnota, zdali je to uzel pro funkci nebo pro prommenou.
 * @return Vraci ERROR_PASSED, jestlize se pridani povedlo, jinak ERROR_COMPILER
 */
int bst_insert(bst_node_t **tree, Dynamic_string *string, bool isFnc);

/**
 * Funkce pro celkove smazani Binarniho vyhledavaciho stromu.
 *
 * @param tree Ukazatel na koren BVS.
 */
void bst_dispose(bst_node_t **tree);

/**
 * Funkce pro prochazeni Binarniho vyhledavaciho stromu.
 *
 * @param tree Ukazatel na koren BVS.
 */
void bst_preorder(bst_node_t *tree);

/**
 * Funkce pro inicializaci Tabulky symbolu.
 *
 * @param listFrame Ukazatel na Tabulku symbolu.
 */
void SLL_Frame_Init(SLList_Frame *listFrame);

/**
 * Funkce pro pridani Framu do Tabulky symbolu.
 *
 * @param listFrame Ukazatel na Tabulku symbolu.
 * @return Vraci ERROR_PASSED, jestlize se pridani povedlo, jinak ERROR_COMPILER.
 */
int SLL_Frame_Insert(SLList_Frame *listFrame);

/**
 * Funkce pro celkove smazani Tabulky symbolu.
 *
 * @param listFrame Ukazatel na Tabulku symbolu.
 */
void SLL_Frame_Dispose(SLList_Frame *listFrame);

/**
 * Funkce pro smazani posledniho lokalniho Framu Tabulky symbolu.
 * Nesmaze globalni Frame.
 *
 * @param listFrame Ukazatel na Tabulku symbolu.
 */
void SLL_Frame_Delete(SLList_Frame *listFrame);

/**
 * Funkce pro smazani globalniho Framu Tabulky symbolu.
 * Jestlize existuje jeste nejaky lokalni Frame, nic neprovede.
 *
 * @param listFrame Ukazatel na Tabulku symbolu.
 */
void SLL_Frame_DeleteGlobal(SLList_Frame *listFrame);

/**
 * Funkce pro vyhledani uzlu v cele Tabulce symbolu podle zadaneho jmena.
 * Pouziva funkci bst_search().
 *
 * @param string Ukazatel na DS, kde je jmeno hledaneho uzlu.
 * @param listFrame Ukazatel na Tabulku symbolu.
 * @return Vraci ukazatel na nalezeny ukazatel, jinak NULL.
 */
bst_node_t *search_Iden(Dynamic_string *, SLList_Frame *listFrame);

/**
 * Funkce pro nahrani dat do struktury pro data funkce.
 *
 * @param tree Ukazatel na uzel, do ktereho se nahravaji hodnoty.
 * @param def Hodnota, zdali byla funkce definovana.
 */
void setDataF(bst_node_t *tree, bool def);

/**
 * Funkce pro nahrani dat do struktury pro data prommene.
 *
 * @param tree Ukazatel na uzel, do ktereho se nahravaji hodnoty.
 * @param type Hodnota typu prommene.
 * @param init Hodnota, zdali byla prommena inicializovana.
 * @param used Hodnota, zdali byla prommena pouzita.
 */
void setDataV(bst_node_t *tree, Data_type type, bool init, bool used);

/**
 * Funkce pro ziskani dat, jestli je uzel pro funkci nebo pro prommene.
 *
 * @param tree Ukazatel na uzel, ze ktereho je chteno ziskat hodnoty.
 * @return Vraci hodnotu true, jestlize je uzel pro funkci, jinak false.
 */
bool isFnc(bst_node_t *tree);

/**
 * Funkce pro ziskani dat, jestli je funkce definovana.
 *
 * @param tree Ukazatel na uzel, ze ktereho je chteno ziskat hodnoty.
 * @return Vraci hodnotu true, jestlize je funkce definovana, jinak false.
 */
bool isDefFnc(bst_node_t *tree);

/**
 * Funkce pro ziskani dat, jestli je prommena inicializovana.
 *
 * @param tree Ukazatel na uzel, ze ktereho je chteno ziskat hodnoty.
 * @return Vraci hodnotu true, jestlize je prommena inicializovana, jinak false.
 */
bool isInitVar(bst_node_t *tree);

/**
 * Funkce pro ziskani dat, jestli je prommena pouzita.
 *
 * @param tree Ukazatel na uzel, ze ktereho je chteno ziskat hodnoty.
 * @return Vraci hodnotu true, jestlize je prommena pouzita, jinak false.
 */
bool isUsedVar(bst_node_t *tree);

/**
 * Funkce pro ziskani datoveho typu prommene.
 *
 * @param tree Ukazatel na uzel, ze ktereho je chteno ziskat hodnoty.
 * @return Vraci hodnotu datoveho typu prommene.
 */
Data_type typeVar(bst_node_t *tree);

//nejake funkce na vraceni hodnoty parametru a returnu

#endif
