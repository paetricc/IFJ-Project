/**
 * Projekt: IFJ2021
 *
 * @brief Zasobnik pro datove typy
 *
 * @details Zasobnik je implementovany jednosmerne vazanym seznamem
 *
 * @author Tomáš Bártů xbartu11@stud.fit.vutbr.cz
 */

#include "type_stack.h"

/**
 * @brief Pro vnitrni testovani chyb zasobniku
 *
 * @details Tak aby byla pohodlnejsi prace pri debugovani
 *  INIT_ERR: chyba pri inicializaci zasobnkiku
 *  TOP_ERR : chyba pri hledani nejvrchnejsiho prvku
 *  POP_ERR : chyba pri odebirani prvku ze zasobniku
 *  PUSH_ERR: chyba pri vkladani prvku do zasobniku
 *
 * @param error Definovana hodnota erroru
 */
void TypeStack_error( int error ) {
    static const char *ERRORS[TYPE_SERR+1] = {
            "Stack error: unknown",
            "Stack error: PUSH",
            "Stack error: POP",
            "Stack error: TOP",
            "Stack error: INIT"
    };
    if (error <= 0 || error > TYPE_SERR) {
        error = 0;
    }
    fprintf(stderr ,"%s\n", ERRORS[error]);
}


/**
 * @brief Inicializace zasobniku pro analyzu zdola nahoru
 *
 * @details Jedna se o zasobnik implementovany jednosmerne vazanym seznamem.
 *  Nemusel by se takto implementovat. Kdybychom ho ale implementovali polem,
 *  tak bychom museli mit pevne danou velikost ale to nechceme
 *
 * @property Kostra je založena na základě IAL DU1
 *
 * @param stack Ukazatel na strukturu jednosmerne vazaneho seznamu
 */
void TypeStack_init( TypeStack *stack ) {
    if (stack == NULL) {
        TypeStack_error(TYPE_INIT_ERR);
    } else {
        stack->topElement = NULL;
    }
}

/**
 * @brief Vkladani prvku na vrchol zasobniku
 *
 * @details Je treba si naallocovat pamet abychom mohli ulozit datovy typ na vrchol zasobniku.
 *
 * @param stack Ukazatel na strukturu jednosmerne vazaneho seznamu
 * @param element Datovy typ ktery vlozime na vrchol zasobniku
 */
void TypeStack_push( TypeStack *stack, DataTypes element) {
    struct TypeStackElement *tmp = (struct TypeStackElement *) malloc(sizeof(struct TypeStackElement));
    if(!tmp) {
        TypeStack_error(TYPE_INIT_ERR);
        return;
    }
    tmp->previousElement = stack->topElement;
    tmp->data = element;
    stack->topElement = tmp;
    tmp = NULL;
}

/**
 * @brief Odstraneni prvku z vrcholu zasobniku
 *
 * @details Odstaneni z prazdneho je brano jako chyba a dojde k vypisu chybove hlasky
 *
 * @param stack Ukazatel na strukturu jednosmerne vazaneho seznamu
 *
 * @return Vraci data v elementu ktery byl odstranen
 */
DataTypes TypeStack_pop( TypeStack *stack ) {
    if(TypeStack_isEmpty(stack)) {
        TypeStack_error(TYPE_POP_ERR);
        return -1;
    }
    TypeStackElementPtr ptr = NULL;
    TypeStack_top(stack, &ptr);
    DataTypes ret = ptr->data;

    struct TypeStackElement *tmp = stack->topElement;
    stack->topElement = stack->topElement->previousElement;
    free(tmp);
    return ret;
}

/**
 * @brief Prazdnost zasobniku
 *
 * @param stack Ukazatel na strukturu jednosmerne vazaneho seznamu
 * @returns V pripade ze je zasobnik prazdny, tak vraci nenulovou hodnotu, jinak nulu
 */
int TypeStack_isEmpty( const TypeStack *stack ) {
    return !(stack->topElement);
}

/**
 * @brief Prvni prvek na vrcholu zasobniku;
 *
 * @param stack Ukazatel na strukturu jednosmerne vazaneho seznamu
 * @param element Ukazatel do nehoz se vlozi najita hodnota
 */
void TypeStack_top( const TypeStack *stack, TypeStackElementPtr *element ) {
    if (TypeStack_isEmpty(stack)) {
        TypeStack_error(TYPE_TOP_ERR);
        return;
    }
    *element = stack->topElement;
}

/**
 * @brief Uvolnime cely zasobnik
 *
 * @details Rekurzivne popujeme zasobnik dokud neni prazdny
 *
 * @param stack Ukazatal na strukturu jednosmerne vazaneho seznamu
 */
void TypeStack_dispose( TypeStack *stack ) {
    while(!TypeStack_isEmpty(stack)) {
        TypeStack_pop(stack);
    }
}

/** @endcode */