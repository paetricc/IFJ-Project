/**
 * Projekt: IFJ2021
 *
 * @brief Zasobnik pro terminaly a neterminaly
 *
 * @details Zasobnik je implementovany jednosmerne vazanym seznamem
 *
 * @author Tomáš Bártů xbartu11@stud.fit.vutbr.cz
 */

#ifndef __TERM_STACK__
#define __TERM_STACK__

#include "scanner.h"
#include <stdlib.h>

#define TERM_SERR 5

#define TERM_RED_ERR 5

#define TERM_INIT_ERR 4

#define TERM_TOP_ERR 3

#define TERM_POP_ERR 2

#define TERM_PUSH_ERR 1

typedef enum{EXP, R, I, E, USD,
    ADD, SUB, MUL, DIV, DIV2, LBR, RBR, ID, LEN, DDOT, LT, GT, LTE, GTE, EQ, NEQ, H
} TermsAndNonTerms;

typedef struct TermStackElement{
   TermsAndNonTerms data;
   struct TermStackElement *previousElement; 
} *TermStackElementPtr;


typedef struct {
    TermStackElementPtr topElement;
} TermStack;


void TermStack_init( TermStack *);

int TermStack_isEmpty( const TermStack * );

int TermStack_isUSDLast( const TermStack * );

void TermStack_top( const TermStack *, TermStackElementPtr *);

void TermStack_firstTermPtr( const TermStack *, struct TermStackElement **);

void TermStack_insertReduce( TermStack *, struct TermStackElement *);

void TermStack_applyReduce( TermStack * );

TermsAndNonTerms TermStack_pop( TermStack * );

void TermStack_push( TermStack *, TermsAndNonTerms );

void TermStack_error( int );

void TermStack_dispose( TermStack * );

#endif

/** @endcode */