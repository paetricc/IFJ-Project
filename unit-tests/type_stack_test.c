#include "../type_stack.h"
#include "../type_stack.c"
#include <stdlib.h>

TypeStack *stack;
void typePrint(struct TypeStackElement *data) {
    switch (data->data)
    {
        case TYPE_STRING:
            printf("STRING");
            break;
        case TYPE_INTEGER:
            printf("INTEGER");
            break;
        case TYPE_NIL:
            printf("NIL");
            break;
        case TYPE_NUMBER:
            printf("NUMBER");
            break;
        default:
            break;
    }
}

void stackPrint( TypeStack *stack ) {
    struct TypeStackElement *tmp = stack->topElement;
    printf("--- TOP ----");
    while(tmp != NULL) {
        printf("\n     ");
        typePrint(tmp);
        tmp = tmp->previousElement;
    }
    printf("\n-- BOTTOM --\n");
}

void fnc_stack_isEmpty( TypeStack *stack ) {
    int i = TypeStack_isEmpty(stack);
    printf("Zasobnik %s prazdny\n", i ? "je" : "neni");
}

void fnc_princtElementData( TypeStackElementPtr ptr ) {
    printf("Data v elementu jsou: ");
    typePrint(ptr);
    printf("\n");
}

int main() {
    printf("Zasobnik implementovany jednosmerne vazanym seznamem\n");
    printf("----------------------------------------------------\n");

    int i = 1;
    stack = (TypeStack *) malloc(sizeof(TypeStack));
    TypeStackElementPtr ptr = NULL;
    DataTypes data;

    printf("[TEST%02d] Inicializace zasobniku\n", i++);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    TypeStack_init(stack);
    stackPrint(stack);

    printf("[TEST%02d] Kontrola stavu zasobniku\n", i++);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    fnc_stack_isEmpty(stack);

    printf("[TEST%02d] Vlozeni prvniho prvku\n", i++);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    TypeStack_push(stack, TYPE_NIL);
    stackPrint(stack);

    printf("[TEST%02d] Zjisteni prvku na vrcholu zasobniku\n", i++);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    TypeStack_top(stack, &ptr);
    fnc_princtElementData(ptr);

    printf("[TEST%02d] Kontrola stavu zasobniku\n", i++);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    fnc_stack_isEmpty(stack);

    printf("[TEST%02d] Odstraneni prvku zasobniku\n", i++);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    TypeStack_pop(stack);
    stackPrint(stack);

    printf("[TEST%02d] Vlozeni nekolika hodnot\n", i++);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    TypeStack_push(stack, TYPE_INTEGER);
    TypeStack_push(stack, TYPE_NIL);
    TypeStack_push(stack, TYPE_STRING);
    TypeStack_push(stack, TYPE_STRING);
    TypeStack_push(stack, TYPE_INTEGER);
    TypeStack_push(stack, TYPE_NUMBER);
    stackPrint(stack);

    printf("[TEST%02d] Uvolneni pameti zasobniku\n", i++);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    TypeStack_dispose(stack);
    stackPrint(stack);

    free(stack);

    return 0;
}