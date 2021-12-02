/**
 * Projekt: IFJ2021
 *
 * @brief Syntaktická analýza zdola nahoru
 *
 * @author Tomáš Bártů xbartu11@stud.fit.vutbr.cz
 */

#include "expression.h"
#include <stdlib.h>

// jezdec udavajici skutecnou pozici v ctenem souboru
fpos_t last_read_pos;

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

TermsAndNonTerms convertTokenType_To_TermsAndNonTerms( Token *token, TypeStack *typeStack ) {
    if(token->Value.keyword == KEYWORD_NIL) {
        TypeStack_push(typeStack, DATA_TYPE_NIL);
        return ID;
    }
    switch (token->ID) {
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
        return ID;
    case TOKEN_ID_INT0:
    case TOKEN_ID_INT:
    case TOKEN_ID_ZERO:
        TypeStack_push(typeStack, DATA_TYPE_INTEGER);
        return ID;
    case TOKEN_ID_DBL2:
    case TOKEN_ID_EXP3:
    case TOKEN_ID_DHEX2:
    case TOKEN_ID_HEXP3:
        TypeStack_push(typeStack, DATA_TYPE_NUMBER);
        return ID;
    case TOKEN_ID_FSTR:
        TypeStack_push(typeStack, DATA_TYPE_STRING);
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
    }
}

int checkDataTypes_ADD_SUB_MUL_DIV(TypeStack *typeStack) {
    DataTypes firstOP, secondOp;
    firstOP = TypeStack_pop(typeStack);
    secondOp = TypeStack_pop(typeStack);
    if(firstOP == DATA_TYPE_INTEGER && secondOp == DATA_TYPE_INTEGER) {
        TypeStack_push(typeStack, DATA_TYPE_INTEGER);
    } else if (firstOP == DATA_TYPE_NUMBER  && secondOp == DATA_TYPE_INTEGER ||
               firstOP == DATA_TYPE_INTEGER && secondOp == DATA_TYPE_NUMBER) {
        TypeStack_push(typeStack, DATA_TYPE_NUMBER);
    } else if (firstOP == DATA_TYPE_NUMBER  && secondOp == DATA_TYPE_NUMBER) {
        TypeStack_push(typeStack, DATA_TYPE_NUMBER);
    } else {
        return ERROR_SEM_COMPAT;
    }
    return ERROR_PASSED;
}

int checkDataTypes_DIV2(TypeStack *typeStack) {
    DataTypes firstOP, secondOp;
    firstOP = TypeStack_pop(typeStack);
    secondOp = TypeStack_pop(typeStack);
    if(firstOP == DATA_TYPE_INTEGER && secondOp == DATA_TYPE_INTEGER) {
        TypeStack_push(typeStack, DATA_TYPE_INTEGER);
    } else {
        return ERROR_SEM_COMPAT;
    }
    return  ERROR_PASSED;
}

/**
 * @brief Preskoci netisknutelne znaku
 * 
 * @param token Struktura tokenu
 * @param file Ukazatel na zdrojovy soubor
 */
int skipNonPrintChar(Token *token, FILE *file) {
    int error = ERROR_PASSED;
    do {
        fgetpos(file, &last_read_pos);
        // nacteni noveho tokenu a kontrola vysupu scanneru
        if ((error = get_token(token, file)) != ERROR_PASSED)
            // lexikalni nebo kompilatorova chyba
            return error;
    } while (token->ID != TOKEN_ID_EOF && (
            token->ID == TOKEN_ID_SPACE || token->ID == TOKEN_ID_EOL ||
            token->ID == TOKEN_ID_TAB || token->ID == TOKEN_ID_LCMT2 ||
            token->ID == TOKEN_ID_BCMT4));
    return error;
}

/**
 * @brief Gramaticka pravidla
 * 
 * @param termStack Ukazatel na strukturu jednosmerne vazaneho seznamu
 * @return int Typ erroru generovany analyzou
 */
int checkRulesAndApply( TermStack *termStack, TypeStack *typeStack ) {
    TermStackElementPtr ptr = NULL;
    TermStack_top(termStack, &ptr);
    if (ptr->data == ID && ptr->previousElement->data == R) {
        TermStack_applyReduce(termStack);
    } else if (ptr->data == EXP && ptr->previousElement->data == ADD && ptr->previousElement->previousElement->data == EXP && ptr->previousElement->previousElement->previousElement->data == R) {
        if(checkDataTypes_ADD_SUB_MUL_DIV(typeStack)) return ERROR_SEM_COMPAT;
        TermStack_applyReduce(termStack);
    } else if (ptr->data == EXP && ptr->previousElement->data == SUB && ptr->previousElement->previousElement->data == EXP && ptr->previousElement->previousElement->previousElement->data == R) {
        if(checkDataTypes_ADD_SUB_MUL_DIV(typeStack)) return ERROR_SEM_COMPAT;
        TermStack_applyReduce(termStack);
    } else if (ptr->data == EXP && ptr->previousElement->data == MUL && ptr->previousElement->previousElement->data == EXP && ptr->previousElement->previousElement->previousElement->data == R) {
        if(checkDataTypes_ADD_SUB_MUL_DIV(typeStack)) return ERROR_SEM_COMPAT;
        TermStack_applyReduce(termStack);
    } else if (ptr->data == EXP && ptr->previousElement->data == DIV && ptr->previousElement->previousElement->data == EXP && ptr->previousElement->previousElement->previousElement->data == R) {
        if(checkDataTypes_ADD_SUB_MUL_DIV(typeStack)) return ERROR_SEM_COMPAT;
        TermStack_applyReduce(termStack);
    } else if (ptr->data == EXP && ptr->previousElement->data == DIV2 && ptr->previousElement->previousElement->data == EXP && ptr->previousElement->previousElement->previousElement->data == R) {
        if(checkDataTypes_DIV2(typeStack)) return ERROR_SEM_COMPAT;
        TermStack_applyReduce(termStack);
    } else if (ptr->data == EXP && ptr->previousElement->data == DDOT && ptr->previousElement->previousElement->data == EXP && ptr->previousElement->previousElement->previousElement->data == R) {
        TermStack_applyReduce(termStack);
    } else if (ptr->data == RBR && ptr->previousElement->data == EXP && ptr->previousElement->previousElement->data == LBR && ptr->previousElement->previousElement->previousElement->data == R) {
        TermStack_applyReduce(termStack);
    } else if (ptr->data == EXP && ptr->previousElement->data == LT && ptr->previousElement->previousElement->data == EXP && ptr->previousElement->previousElement->previousElement->data == R) {
        TermStack_applyReduce(termStack);
    } else if (ptr->data == EXP && ptr->previousElement->data == GT && ptr->previousElement->previousElement->data == EXP && ptr->previousElement->previousElement->previousElement->data == R) {
        TermStack_applyReduce(termStack);
    } else if (ptr->data == EXP && ptr->previousElement->data == LTE && ptr->previousElement->previousElement->data == EXP && ptr->previousElement->previousElement->previousElement->data == R) {
        TermStack_applyReduce(termStack);
    } else if (ptr->data == EXP && ptr->previousElement->data == GTE && ptr->previousElement->previousElement->data == EXP && ptr->previousElement->previousElement->previousElement->data == R) {
        TermStack_applyReduce(termStack);
    } else if (ptr->data == EXP && ptr->previousElement->data == EQ && ptr->previousElement->previousElement->data == EXP && ptr->previousElement->previousElement->previousElement->data == R) {
        TermStack_applyReduce(termStack);
    } else if (ptr->data == EXP && ptr->previousElement->data == NEQ && ptr->previousElement->previousElement->data == EXP && ptr->previousElement->previousElement->previousElement->data == R) {
        TermStack_applyReduce(termStack);
    } else if (ptr->data == EXP && ptr->previousElement->data == LEN && ptr->previousElement->previousElement->data == R) {
        DataTypes firstOp = TypeStack_pop(typeStack);
        if (firstOp == DATA_TYPE_STRING) {
            TypeStack_push(typeStack, DATA_TYPE_INTEGER);
        } else {
            return ERROR_SEM_COMPAT;
        }
        TermStack_applyReduce(termStack);
    } else {
        return ERROR_SYNTAX;
    }
    return ERROR_PASSED;
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
int exprSyntaxCheck(Token *token, FILE *file, SLList_Frame *listFrame) {
    int error;
    bst_node_t *ptr_node = NULL;
    // pridelim pamet zasobniku datovych typu
    TypeStack *typeStack = (TypeStack *) malloc(sizeof (TypeStack));
    if(!typeStack) return ERROR_COMPILER;
    TypeStack_init(typeStack);
    // pridelim pamet zasobniku terminalu a neterminalu
    TermStack *termStack = (TermStack *) malloc(sizeof(TermStack));
    if(!termStack) return ERROR_COMPILER;
    TermStack_init(termStack);
    // vlozim $
    TermStack_push(termStack, USD);

    struct TermStackElement *top = NULL;
    // zjistim si dalsi token
    skipNonPrintChar(token, file);
    // jedna se o identifikator?
    if(token->ID == TOKEN_ID_ID) {
        ptr_node = search_Iden(token->Value.string, listFrame);
        // nasel jsem ho?
        if(ptr_node != NULL) {
            // pokud to neni to promenna a je inicializovane?
            // muze to ale byt buldin funkce (write, readi, ...)
            if(!(isFnc(ptr_node) && isInitVar(ptr_node))) {
                return ERROR_SEM_UNDEFINED;
            }
        } else {
            // nenasel, takze klasika. Yeetni error
            freeStacks(termStack, typeStack);
            return ERROR_SEM_UNDEFINED;
        }
    }
    // proslo to, tak prelozime token na terminal
    TermsAndNonTerms vstup = convertTokenType_To_TermsAndNonTerms(token, typeStack);

    do {
        TermStack_firstTermPtr(termStack, &top);
        // podle pravidla v tabulce rozhodnu co budu delat
        switch (p_table[(top->data-4)][vstup-4]) {
        // terminaly jsou si rovny
        case E:
            TermStack_push(termStack, vstup);
            // zjistim si dalsi token
            skipNonPrintChar(token, file);
            // jedna se o identifikator?
            if(token->ID == TOKEN_ID_ID) {
                // zname ho?
                if(search_Iden(token->Value.string, listFrame)) {
                    // nee? tak hod error
                    freeStacks(termStack, typeStack);
                    return ERROR_SEM_UNDEFINED;
                }
            }
            // proslo to, tak prelozime token na terminal
            vstup = convertTokenType_To_TermsAndNonTerms(token, typeStack);
            break;
        // vlozim znak redukce
        case R:
            TermStack_insertReduce(termStack, top);
            TermStack_push(termStack, vstup);
            // zjisitm si dalsi token
            skipNonPrintChar(token, file);
            // jedna se o identifikator?
            if(token->ID == TOKEN_ID_ID) {
                // a zname ho?
                if(!search_Iden(token->Value.string, listFrame)) {
                    // nee? tak hod error
                    freeStacks(termStack, typeStack);
                    return ERROR_SEM_UNDEFINED;
                }
            }
            vstup = convertTokenType_To_TermsAndNonTerms(token, typeStack);
            break;
        // zredukuju po nejblizsi znak redukce
        case I:
            error = checkRulesAndApply(termStack, typeStack);
            // neexistije pro redukci pravidlo
            if(error == ERROR_SYNTAX) {
                freeStacks(termStack, typeStack);
                return ERROR_SYNTAX;
            } else if(error == ERROR_SEM_COMPAT) {
                freeStacks(termStack, typeStack);
                return ERROR_SEM_ASSIGN;
            }
            break;
        // vyhodnotim co se jde a predam rizeni zpet top-down analyze
        case H:
            vstup = USD;
            break;
        // chybovy stav
        default:
            freeStacks(termStack, typeStack);
            return ERROR_SYNTAX;
        }
    } while( vstup != USD  || !SA_isOK(termStack)); // opakuji dokud vstup neni $ a dokud muzu redukovat
    //uvolnim pamet zasobniku
    freeStacks(termStack, typeStack);
    fsetpos(file, &last_read_pos);
    return ERROR_PASSED;
}

void freeStacks(TermStack *termStack, TypeStack *typeStack) {
    TypeStack_dispose(typeStack);
    free(typeStack);
    TermStack_dispose(termStack);
    free(termStack);
}
