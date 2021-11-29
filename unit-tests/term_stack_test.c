#include "../term_stack.h"
#include "../term_stack.c"
#include <stdlib.h>

TermStack *stack;
void termPrint(struct TermStackElement *data) {
    switch (data->data)
    {
    case EXP:
        printf("E");
        break;
    case R:
        printf("←");
        break;
    case I:
        printf("→");
        break;
    case E:
        printf("=");
        break;
    case USD:
        printf("$");
        break;
    case ADD:
        printf("+");
        break;
    case SUB:
        printf("-");
        break;
    case MUL:
        printf("*");
        break;
    case DIV:
        printf("/");
        break;
    case DIV2:
        printf("//");
        break;
    case RBR:
        printf("(");
        break;
    case LBR:
        printf(")");
        break;
    case ID:
        printf("i");
        break;
    case LEN:
        printf("#");
        break;
    case DDOT:
        printf("..");
        break;
    case LT:
        printf("<");
        break;
    case GT:
        printf(">");
        break;
    case LTE:
        printf("<=");
        break;
    case GTE:
        printf(">=");
        break;
    case EQ:
        printf("==");
        break;
    case NEQ:
        printf("~=");
        break;
    default:
        break;
    }
}

void stackPrint( TermStack *stack ) {
    struct TermStackElement *tmp = stack->topElement;
    printf("--- TOP ----");
    while(tmp != NULL) {
        printf("\n     ");
        termPrint(tmp);
        tmp = tmp->previousElement;
    }
    printf("\n-- BOTTOM --\n");
}

void fnc_stack_isEmpty( TermStack *stack ) {
    int i = TermStack_isEmpty(stack);
    printf("Zasobnik %s prazdny\n", i ? "je" : "neni");
}

void fnc_stack_isUSDLast( TermStack *stack ) {
    int i = TermStack_isUSDLast(stack);
    printf("Na spodku zasobniku %s dolar\n", i ? "je" : "neni");
}

void fnc_princtElementData( TermStackElementPtr ptr ) {
    printf("Data v elementu jsou: ");
    termPrint(ptr);
    printf("\n");
}

int main() {
    printf("Zasobnik mplementovany jednosmerne vazanym sezname\n");
    printf("--------------------------------------------------\n");

    int i = 1;
    stack = (TermStack *) malloc(sizeof(TermStack));
    TermStackElementPtr ptr = NULL; 
    TermsAndNonTerms data;

    printf("[TEST%02d] Inicializace zasobniku\n", i++);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    TermStack_init(stack);
    stackPrint(stack);

    printf("[TEST%02d] Kontrola stavu zasobniku\n", i++);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    fnc_stack_isEmpty(stack);

    printf("[TEST%02d] Vlozeni prvniho prvku\n", i++);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    TermStack_push(stack, ADD);
    stackPrint(stack);

    printf("[TEST%02d] Kontrola zda je dolar na spodku zasobniku\n", i++);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    stackPrint(stack);
    fnc_stack_isUSDLast(stack);

    printf("[TEST%02d] Zjisteni prvku na vrcholu zasobniku\n", i++);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    TermStack_top(stack, &ptr);
    fnc_princtElementData(ptr);

    printf("[TEST%02d] Kontrola stavu zasobniku\n", i++);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    fnc_stack_isEmpty(stack);

    printf("[TEST%02d] Odstraneni prvku zasobniku\n", i++);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    TermStack_pop(stack);
    stackPrint(stack);

    printf("[TEST%02d] Vlozeni terminalu na vrchol zasobniku\n", i++);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    TermStack_push(stack, USD);
    stackPrint(stack);

    printf("[TEST%02d] Kontrola zda je dolar na spodku zasobniku\n", i++);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    stackPrint(stack);
    fnc_stack_isUSDLast(stack);

    printf("[TEST%02d] Simulace vlozeni redukce s terminalem\n", i++);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    struct TermStackElement *tmp = NULL;
    TermStack_firstTermPtr(stack, &tmp);
    TermStack_insertReduce(stack, tmp);
    TermStack_push(stack, ID);
    stackPrint(stack);

    printf("[TEST%02d] Simulace redukce\n", i++);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    TermStack_applyReduce(stack);
    stackPrint(stack);

    printf("[TEST%02d] Vlozeni terminalu\n", i++);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    TermStack_push(stack, ADD);
    stackPrint(stack);

    printf("[TEST%02d] Simulace slozitejsi redukce s chybou\n", i++);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    TermStack_applyReduce(stack);
    stackPrint(stack);

    printf("[TEST%02d] Vlozeni nekolika hodnot\n", i++);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    TermStack_push(stack, USD);
    TermStack_push(stack, R);
    TermStack_push(stack, EXP);
    TermStack_push(stack, ADD);
    TermStack_push(stack, R);
    TermStack_push(stack, ID);
    stackPrint(stack);

    printf("[TEST%02d] Simulace slozitejsi redukce bez chyby\n", i++);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    TermStack_applyReduce(stack);
    stackPrint(stack);

    printf("[TEST%02d] Redukce pouze na expression\n", i++);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    TermStack_applyReduce(stack);
    stackPrint(stack);

    printf("[TEST%02d] Uvolneni pameti zasobniku\n", i++);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    TermStack_dispose(stack);
    stackPrint(stack);

    free(stack);

    return 0;
}