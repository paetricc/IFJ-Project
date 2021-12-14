/**
 * Projekt: IFJ2021
 *
 * @brief Zasobnik pro datove typy
 *
 * @details Zasobnik je implementovany jednosmerne vazanym seznamem
 *
 * @property Kostra je založena na základě IAL DU1
 *
 * @author Tomáš Bártů xbartu11@stud.fit.vutbr.cz
 * @
 */

#ifndef _TYPE_STACK_H_
#define _TYPE_STACK_H_


#include "scanner.h"
#include <stdlib.h>

#define TYPE_SERR 4

#define TYPE_INIT_ERR 4

#define TYPE_TOP_ERR 3

#define TYPE_POP_ERR 2

#define TYPE_PUSH_ERR 1

typedef enum{ DATA_TYPE_INTEGER, DATA_TYPE_STRING, DATA_TYPE_NUMBER, DATA_TYPE_NIL
} DataTypes;

typedef struct TypeStackElement{
    DataTypes data;
    struct TypeStackElement *previousElement;
} *TypeStackElementPtr;


typedef struct {
    TypeStackElementPtr topElement;
} TypeStack;


void TypeStack_init( TypeStack *);

int TypeStack_isEmpty( const TypeStack * );

void TypeStack_top( const TypeStack *, TypeStackElementPtr *);

DataTypes TypeStack_pop( TypeStack * );

void TypeStack_push( TypeStack *, DataTypes );

void TypeStack_error( int );

void TypeStack_dispose( TypeStack * );

#endif

/** @endcode */