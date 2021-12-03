/**
 * Projekt: IFJ2021
 *
 * @brief Zasobnik pro terminaly a neterminaly
 * 
 * @details Zasobnik je implementovany jednosmerne vazanym seznamem
 *
 * @author Tomáš Bártů xbartu11@stud.fit.vutbr.cz
 */

#include "term_stack.h"

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
void TermStack_error( int error ) {
    static const char *ERRORS[TERM_SERR+1] = {
        "Stack error: unknown",
        "Stack error: PUSH",
        "Stack error: POP",
        "Stack error: TOP",
        "Stack error: INIT",
        "Stack error: REDUCE"
    };
    if (error <= 0 || error > TERM_SERR) {
        error = 0;
    }
    fprintf(stderr ,"%s\n", ERRORS[error]);
}


/**
 * @brief Inicializace zasobniku pro analyzu zdola nahoru
 * 
 * @details Jedna se o zasobnik implementovany jednosmerne vazanym seznamem,
 *  protoze bude potreba se divat nejen na prvni polozku zasobniku ale in na
 *  polozky pod ni. Budeme totiz potrebovat najit prvni neterminal a ten
 *  nemusi byt vzdy na vrcholu zasobniku
 * 
 * @param stack Ukazatel na strukturu jednosmerne vazaneho seznamu
 */
void TermStack_init( TermStack *stack ) {
    if (stack == NULL) {
        TermStack_error(TERM_INIT_ERR);
    } else {
        stack->topElement = NULL;
    }
}

/**
 * @brief Vkladani prvku na vrchol zasobniku
 * 
 * @details Je treba si naallocovat pamet abychom mohli ulozit terminal ci
 *  neterminal na vrchol zasobniku.
 * 
 * @param stack Ukazatel na strukturo jednosmerne vazaneho seznamu 
 * @param element Terminal nebo neterminal ktery vlozime na vrchol zasobniku
 */
void TermStack_push( TermStack *stack, TermsAndNonTerms element) {
    struct TermStackElement *tmp = (struct TermStackElement *) malloc(sizeof(struct TermStackElement));
    if(!tmp) {
        TermStack_error(TERM_INIT_ERR);
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
TermsAndNonTerms TermStack_pop( TermStack *stack ) {
    if(TermStack_isEmpty(stack)) {
        TermStack_error(TERM_POP_ERR);
        return -1;
    }
    TermStackElementPtr ptr = NULL;
    TermStack_top(stack, &ptr);
    TermsAndNonTerms ret = ptr->data;

    struct TermStackElement *tmp = stack->topElement;
    stack->topElement = stack->topElement->previousElement;
    free(tmp);
    return ret;
}

/**
 * @brief Hledani v seznamu prvniho neterminalu
 * 
 * @details Terminaly maji hodnotu vyssi nez 4, tedy neterminaly maji hodnotu 0, 1, 2, 3 a
 *  hledame prvni terminal v zasobniku. Neterminaly preskakujeme. Vracime odkaz
 *  na prvni najity neterminal
 * 
 * @param stack Ukazatel na strukturu jednosmerne vazaneho seznamu 
 * @param ptr Ukazatel na najity terminal
 */
void TermStack_firstTermPtr( const TermStack *stack, struct TermStackElement **ptr){
    struct TermStackElement *tmp = stack->topElement;
    while (tmp->previousElement != NULL && tmp->data < 4) {
        tmp = tmp->previousElement;
    }
    *ptr = tmp;
}

/**
 * @brief Vlozeni R pred prvni nalezeny terminal
 * 
 * @param stack Ukazatel na stukuturu jednosmerne vazaneho seznamu
 * @param ptr Ukazatel na prvek seznamu ktery lezi pred terminalem
 */
void TermStack_insertReduce( TermStack *stack, struct TermStackElement *ptr) {
    struct TermStackElement *tmp = stack->topElement;
    //prvek je na vrcholu
    if(tmp == ptr) {
        TermStack_push(stack, R);
        return;
    }

    while(tmp->previousElement != ptr) {
        tmp = tmp->previousElement;
    }

    struct TermStackElement *new = (struct TermStackElement *) malloc(sizeof(struct TermStackElement));
    if(!new) {
        TermStack_error(TERM_INIT_ERR);
        return;
    }

    new->data = R;
    new->previousElement = tmp->previousElement;
    tmp->previousElement = new;
}

/**
 * @brief Zredukuje obsah zasobniku pop prvni <
 * 
 * @details Pokud nenajde znak redukce tak vyprintuje error
 * 
 * @param stack Ukazatel na strukturu jednosmerne vazaneho seznamu
 */
void TermStack_applyReduce( TermStack *stack ) {
    struct TermStackElement *tmp = stack->topElement;
    if(tmp == NULL) {
        TermStack_error(TERM_RED_ERR);
        return;
    }

    while(tmp->data != R) {
        tmp = tmp->previousElement;
        TermStack_pop(stack);
        if(TermStack_isEmpty(stack)) {
            TermStack_error(TERM_RED_ERR);
            return;
        }   
    }
    TermStack_pop(stack);
    TermStack_push(stack, EXP);
}

/**
 * @brief Prazdnost zasobniku
 * 
 * @param stack Ukazatel na strukturu jednosmerne vazaneho seznamu
 * @returns V pripade ze je zasobnik prazdny, tak vraci nenulovou hodnotu, jinak nulu 
 */
int TermStack_isEmpty( const TermStack *stack ) {
    return !(stack->topElement);
}

int TermStack_isUSDLast( const TermStack *stack) {
    struct TermStackElement *tmp = stack->topElement;
    while(tmp->previousElement != NULL) {
        tmp = tmp->previousElement;
    }
    return (tmp->data == USD) ? 1 : 0;
}

/**
 * @brief Prvni prvek na vrcholu zasobniku;
 * 
 * @param stack Ukazatel na strukturu jednosmerne vazaneho seznamu
 * @param element Ukazatel do nehoz se vlozi najit hodnota
 */
void TermStack_top( const TermStack *stack, TermStackElementPtr *element ) {
    if (TermStack_isEmpty(stack)) {
        TermStack_error(TERM_TOP_ERR);
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
void TermStack_dispose( TermStack *stack ) {
    while(!TermStack_isEmpty(stack)) {
        TermStack_pop(stack);
    }
}