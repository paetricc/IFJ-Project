#ifndef TOKEN_STACK
#define TOKEN_STACK

#include "scanner.h"

#define STACK_MAX 420

#define SERR 4

#define INIT_ERR 4

#define TOP_ERR 3

#define POP_ERR 2

#define PUSH_ERR 1

/* ADT zasobnik Token_stack */
typedef struct {
    /* pole pro ulozeni tokenu */
    Token_ID token_array[STACK_MAX];
    /* index prvku na vrcholu zasobniku */
    int top;
} Token_stack;

void Token_stack_init( Token_stack * );

int Token_stack_isFull( const Token_stack *);

int Token_stack_isEmpty( const Token_stack * );

void Token_stack_top(const Token_stack *, Token_ID *);

void Token_stack_pop( Token_stack * );

void Token_stack_push( Token_stack  *, Token_ID );

void Token_stack_error( int );

#endif