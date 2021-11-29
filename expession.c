#include "expression.h"
#include <stdlib.h>

static TermsAndNonTerms p_table[17][17] =
{       /* $  +  -  *  /  // (  )  i  #  .. <  >  <= >= == ~= */ 
/*  $ */ {-1, R, R, R, R, R, R,-1, R, R, R, I, I, I, I, I, I },
/*  + */ { I, I, I, R, R, R, R, I, R, R, I, I, I, I, I, I, I },
/*  - */ { I, I, I, R, R, R, R, I, R, R, I, I, I, I, I, I, I },
/*  * */ { I, I, I, I, I, I, R, I, R, R, I, I, I, I, I, I, I },
/*  / */ { I, I, I, I, I, I, R, I, R, R, I, I, I, I, I, I, I },
/* // */ { I, I, I, I, I, I, R, I, R, R, I, I, I, I, I, I, I },
/*  ( */ {-1, R, R, R, R, R, R, E, R, R, R, R, R, R, R, R, R },
/*  ) */ { I, I, I, I, I, I,-1, I,-1,-1, I, I, I, I, I, I, I },
/*  i */ { I, I, I, I, I, I,-1, I, R, R, I, I, I, I, I, I, I },
/*  # */ { I, I, I, I, I, I, R,-1, R,-1, I, I, I, I, I, I, I },
/* .. */ { I, R, R, R, R, R, R, I, R, R, R, I, I, I, I, I, I },
/*  < */ { I, R, R, R, R, R, R, I, R, R, R, I, I, I, I, I, I },
/*  > */ { I, R, R, R, R, R, R, I, R, R, R, I, I, I, I, I, I },
/* <= */ { I, R, R, R, R, R, R, I, R, R, R, I, I, I, I, I, I },
/* >= */ { I, R, R, R, R, R, R, I, R, R, R, I, I, I, I, I, I },
/* == */ { I, R, R, R, R, R, R, I, R, R, R, I, I, I, I, I, I },
/* ~= */ { I, R, R, R, R, R, R, I, R, R, R, I, I, I, I, I, I },
};

TermsAndNonTerms convertTokenType_To_TermsAndNonTerms( Token_ID token_ID ) {
    switch (token_ID) {
    case TOKEN_ID_ADD:
        return ADD;
    case TOKEN_ID_SUB:
        return SUB;
    case TOKEN_ID_MUL:
        return MUL;
    case TOKEN_ID_DIV:
        return DIV;
    case TOKEN_ID_DIV2:
        return DIV2;
    case TOKEN_ID_LT:
        return LT;
    case TOKEN_ID_LTE:
        return LTE;
    case TOKEN_ID_GT:
        return GT;
    case TOKEN_ID_GTE:
        return GTE;
    case TOKEN_ID_NEQ2:
        return NEQ;
    case TOKEN_ID_EQ:
        return EQ;
    case TOKEN_ID_LBR:
        return LBR;
    case TOKEN_ID_RBR:
        return RBR;
    case TOKEN_ID_ID:
    case TOKEN_ID_INT0:
    case TOKEN_ID_INT:
    case TOKEN_ID_ZERO:
    case TOKEN_ID_DBL2:
    case TOKEN_ID_EXP3:
    case TOKEN_ID_DHEX2:
    case TOKEN_ID_HEXP3:
    case TOKEN_ID_FSTR:
        return ID;
    case TOKEN_ID_LEN:
        return LEN;
    case TOKEN_ID_DDOT:
        return DDOT;
    /*case TOKEN_ID_SPACE:
    case TOKEN_ID_EOL:
    case TOKEN_ID_TAB:
        break;*/
    default:
        return USD;
        break;
    }
}

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
        printf(")");
        break;
    case LBR:
        printf("(");
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

void skipNonPrintChar(Token *token, FILE *file) {
    do {
        get_token(token, file);
    } while (token->ID == TOKEN_ID_SPACE || token->ID == TOKEN_ID_TAB);
}

int checkRulesAndApply( TermStack *stack ) {
    TermStackElementPtr ptr = NULL;
    TermStack_top(stack, &ptr);
    if (ptr->data == ID && ptr->previousElement->data == R) {
        TermStack_applyReduce(stack);
    } else if (ptr->data == EXP && ptr->previousElement->data == ADD && ptr->previousElement->previousElement->data == EXP) {
        TermStack_applyReduce(stack);
    } else if (ptr->data == EXP && ptr->previousElement->data == SUB && ptr->previousElement->previousElement->data == EXP) {
        TermStack_applyReduce(stack);
    } else if (ptr->data == EXP && ptr->previousElement->data == MUL && ptr->previousElement->previousElement->data == EXP) {  
        TermStack_applyReduce(stack);
    } else if (ptr->data == EXP && ptr->previousElement->data == DIV && ptr->previousElement->previousElement->data == EXP) {   
        TermStack_applyReduce(stack);
    } else if (ptr->data == EXP && ptr->previousElement->data == DIV2 && ptr->previousElement->previousElement->data == EXP) {    
        TermStack_applyReduce(stack);
    } else if (ptr->data == EXP && ptr->previousElement->data == DDOT && ptr->previousElement->previousElement->data == EXP) {   
        TermStack_applyReduce(stack);
    } else if (ptr->data == RBR && ptr->previousElement->data == EXP && ptr->previousElement->previousElement->data == LBR) {   
        TermStack_applyReduce(stack);
    } else if (ptr->data == EXP && ptr->previousElement->data == LT && ptr->previousElement->previousElement->data == EXP) {   
        TermStack_applyReduce(stack);
    } else if (ptr->data == EXP && ptr->previousElement->data == GT && ptr->previousElement->previousElement->data == EXP) {   
        TermStack_applyReduce(stack);
    } else if (ptr->data == EXP && ptr->previousElement->data == LTE && ptr->previousElement->previousElement->data == EXP) {   
        TermStack_applyReduce(stack);
    } else if (ptr->data == EXP && ptr->previousElement->data == GTE && ptr->previousElement->previousElement->data == EXP) {   
        TermStack_applyReduce(stack);
    } else if (ptr->data == EXP && ptr->previousElement->data == EQ && ptr->previousElement->previousElement->data == EXP) {   
        TermStack_applyReduce(stack);
    } else if (ptr->data == EXP && ptr->previousElement->data == NEQ && ptr->previousElement->previousElement->data == EXP) {   
        TermStack_applyReduce(stack);
    } else if (ptr->data == EXP && ptr->previousElement->data == LEN) {   
        TermStack_applyReduce(stack);
    } else {
        return ERROR_SEM_ASSIGN;
    }
}

int SA_isOK(TermStack *stack) {
    struct TermStackElement *tmp = NULL;
    TermStack_top(stack, &tmp);
    return (tmp->data == EXP && tmp->previousElement->data == USD && tmp->previousElement->previousElement == NULL) ? 1 : 0;
}

int exprSyntaxCheck(Token *token, FILE *file) {

    TermStack *stack = (TermStack *) malloc(sizeof(TermStack));
    TermStack_init(stack);
    TermStack_push(stack, USD);

    TermsAndNonTerms ID_of_Term;
    struct TermStackElement *top = NULL;
    int error = 0;

    skipNonPrintChar(token, file);
    TermsAndNonTerms vstup = convertTokenType_To_TermsAndNonTerms(token->ID);

    do {
        TermStack_firstTermPtr(stack, &top);
        switch (p_table[(top->data-4)][vstup-4]) {
        case E:
            TermStack_push(stack, vstup);
            skipNonPrintChar(token, file);
            vstup = convertTokenType_To_TermsAndNonTerms(token->ID);
            break;
        case R:
            TermStack_insertReduce(stack, top);
            TermStack_push(stack, vstup);
            skipNonPrintChar(token, file);
            vstup = convertTokenType_To_TermsAndNonTerms(token->ID);
            break;
        case I:
            error = checkRulesAndApply(stack);
            if(error == ERROR_SEM_ASSIGN) return ERROR_SEM_ASSIGN;
            break;
        default:
            return ERROR_SEM_ASSIGN;
        }
    } while( vstup != USD  || !SA_isOK(stack));
    TermStack_dispose(stack);
    return ERROR_PASSED;
}