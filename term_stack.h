#ifndef __TERM_STACK__
#define __TERM_STACK__

#include "scanner.h"

#define SERR 5

#define RED_ERR 5

#define INIT_ERR 4

#define TOP_ERR 3

#define POP_ERR 2

#define PUSH_ERR 1

typedef enum{EXP, R, I, E, USD,
    ADD, SUB, MUL, DIV, DIV2, RBR, LBR, ID, LEN, DDOT, LT, GT, LTE, GTE, EQ, NEQ
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