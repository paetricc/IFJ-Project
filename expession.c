/**
 * Projekt: IFJ2021
 *
 * @brief Syntaktická analýza zdola nahoru
 *
 * @author Tomáš Bártů xbartu11@stud.fit.vutbr.cz
 */

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
/*  i */ { I, I, I, I, I, I,-1, I, H, R, I, I, I, I, I, I, I },
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


/**
 * @brief Pomocna funkce pro lepsi vizualizaci dat na zasobniku
 * 
 * @param data Ukazatel na prvek v zasobniku
 */
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

/**
 * @brief Pomocna funkce pro vypis zasobniku
 * 
 * @param stack Ukazatel na strukturu jednosmerne vazaneho seznamu
 */
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

/**
 * @brief Preskoci netisknutelne znaku
 * 
 * @param token Struktura tokenu
 * @param file Ukazatel na zdrojovy soubor
 */
void skipNonPrintChar(Token *token, FILE *file) {
    do {
        get_token(token, file);
    } while (token->ID == TOKEN_ID_SPACE || token->ID == TOKEN_ID_TAB);
}

/**
 * @brief Gramaticka pravidla
 * 
 * @param stack Ukazatel na strukturu jednosmerne vazaneho seznamu
 * @return int Typ erroru generovany analyzou
 */
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
        return ERROR_SYNTAX;
    }
    return ERROR_SYNTAX;
}

/**
 * @brief Simulace terminalu $ na vstup
 * 
 * @param stack Ukazatel na strukturu jednosmerne vazaneho seznamu
 * @return int Logickou jednicku pokud zasobnik od dna vypadata takto: {|$|E|} jinak vraci logickou nulu
 */
int SA_isOK(TermStack *stack) {
    struct TermStackElement *tmp = NULL;
    TermStack_top(stack, &tmp);
    return (tmp->data == EXP && tmp->previousElement->data == USD && tmp->previousElement->previousElement == NULL) ? 1 : 0;
}

/**
 * @brief Syntakticka kontrola vyrazu
 * 
 * @param token Struktura tokenu
 * @param file Ukazatel na zdrojovy soubor
 * @return int Typ erroru generovany analyzou
 */
int exprSyntaxCheck(Token *token, FILE *file) {
    // pridelim pamet zasobniku
    TermStack *stack = (TermStack *) malloc(sizeof(TermStack));
    if(!stack) return ERROR_COMPILER;
    TermStack_init(stack);
    // vlozim $
    TermStack_push(stack, USD);

    struct TermStackElement *top = NULL;
    int error = 0;

    skipNonPrintChar(token, file);
    // prevedu si token->ID na terminal ci neterminal
    TermsAndNonTerms vstup = convertTokenType_To_TermsAndNonTerms(token->ID);

    do {
        TermStack_firstTermPtr(stack, &top);
        // podle pravidla v tabulce rozhodnu co budu delat
        switch (p_table[(top->data-4)][vstup-4]) {
        // terminaly jsou si rovny
        case E:
            TermStack_push(stack, vstup);
            skipNonPrintChar(token, file);
            vstup = convertTokenType_To_TermsAndNonTerms(token->ID);
            break;
        // vlozim znak redukce
        case R:
            TermStack_insertReduce(stack, top);
            TermStack_push(stack, vstup);
            skipNonPrintChar(token, file);
            vstup = convertTokenType_To_TermsAndNonTerms(token->ID);
            break;
        // zredukuju po nejblizsi znak redukce
        case I:
            error = checkRulesAndApply(stack);
            // neexistije pro redukci pravidlo
            if(error == ERROR_SYNTAX) {
                TermStack_dispose(stack);
                return ERROR_SYNTAX;
            }
            break;
        // vyhodnotim co se jde a predam rizeni zpet top-down analyze
        case H:
            vstup = USD;
            break;
        // chybovy stav
        default:
            TermStack_dispose(stack);
            return ERROR_SYNTAX;
        }
    } while( vstup != USD  || !SA_isOK(stack)); // opakuji dokud vstup neni $ a dokud muzu redukovat
    //uvolnim pamet zasobniku
    TermStack_dispose(stack);
    return ERROR_PASSED;
}