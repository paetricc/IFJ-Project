#ifndef TERM_STACK_TEST_H
#define TERM_STACK_TEST_H

#include "../term_stack.h"
#include "../term_stack.c"

void termPrint(struct TermStackElement *);

void stackPrint( TermStack * );

void fnc_stack_isEmpty( TermStack * );

void fnc_stack_isUSDLast( TermStack * );

void fnc_princtElementData( TermStackElementPtr );

#endif
