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

// Prediktivni tabulka
static TermsAndNonTerms p_table[17][17] =  {
        /*  $  +  -  *  / //  (  )  i  # ..  <  > <= >= == ~= */
/*  $ */ { -1, R, R, R, R, R, R,-1, R, R, R, R, R, R, R, R, R },
/*  + */ {  I, I, I, R, R, R, R, I, R, R, I, I, I, I, I, I, I },
/*  - */ {  I, I, I, R, R, R, R, I, R, R, I, I, I, I, I, I, I },
/*  * */ {  I, I, I, I, I, I, R, I, R, R, I, I, I, I, I, I, I },
/*  / */ {  I, I, I, I, I, I, R, I, R, R, I, I, I, I, I, I, I },
/* // */ {  I, I, I, I, I, I, R, I, R, R, I, I, I, I, I, I, I },
/*  ( */ { -1, R, R, R, R, R, R, E, R, R, R, R, R, R, R, R, R },
/*  ) */ {  I, I, I, I, I, I,-1, I,-1,-1, I, I, I, I, I, I, I },
/*  i */ {  I, I, I, I, I, I,-1, I, H, R, I, I, I, I, I, I, I },
/*  # */ {  I, I, I, I, I, I, R, I, R,-1, I, I, I, I, I, I, I },
/* .. */ {  I, R, R, R, R, R, R, I, R, R, R, I, I, I, I, I, I },
/*  < */ {  I, R, R, R, R, R, R, I, R, R, R, I, I, I, I, I, I },
/*  > */ {  I, R, R, R, R, R, R, I, R, R, R, I, I, I, I, I, I },
/* <= */ {  I, R, R, R, R, R, R, I, R, R, R, I, I, I, I, I, I },
/* >= */ {  I, R, R, R, R, R, R, I, R, R, R, I, I, I, I, I, I },
/* == */ {  I, R, R, R, R, R, R, I, R, R, R, I, I, I, I, I, I },
/* ~= */ {  I, R, R, R, R, R, R, I, R, R, R, I, I, I, I, I, I } };

// hodnota predchoziho tokenu
Token_ID prev;
// pomocna promenna kontrolujici zda bylo jiz pouzti {==, ~=, <, >, <=, >=}
int isCorrect = 1;
// pomocna promenna udavajici jaky kod vytiskneme
TermsAndNonTerms decide;

int elseCounter = 0;

/**
 * @brief Preskoci netisknutelne znaky
 *
 * @param token Struktura tokenu
 * @param file Ukazatel na zdrojovy soubor
 */
int skipNonPrintChar(Token *token, FILE *file) {
    int error = ERROR_PASSED;
    prev = token->ID;
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
 * @brief Funkce pro prevod stringu na string podporovany jazykem IFJcode21
 *
 * @param input Ukazatel na pole charu
 * @return Upraveny string
 */
char *converString(char *input) {
    char *output = (char *)malloc(strlen(input));
    int positon = 0;
    int i;
    for(i = 1; input[i+1] != '\0'; i++) {
        if (input[i] == '\\') {
            if(input[i+1] == '\\') {
                output = (char *) realloc(output, strlen(input) + 4);
                output[positon++] = '\\';
                output[positon++] = '0';
                output[positon++] = '9';
                output[positon++] = '2';
            } else if (input[i+1] == '"') {
                output = (char *) realloc(output, strlen(input) + 4);
                output[positon++] = '\\';
                output[positon++] = '0';
                output[positon++] = '3';
                output[positon++] = '4';
            } else if (input[i+1] == 'n') {
                output = (char *) realloc(output, strlen(input) + 4);
                output[positon++] = '\\';
                output[positon++] = '0';
                output[positon++] = '1';
                output[positon++] = '0';
            } else if (input[i+1] == 't') {
                output = (char *) realloc(output, strlen(input) + 4);
                output[positon++] = '\\';
                output[positon++] = '0';
                output[positon++] = '0';
                output[positon++] = '9';
            } else {
                output[positon++] = input[i];
                i--;
            }
            i+=1;
        } else if(input[i] <= 32 || input[i] == 34 || input[i] == 92) {
            output = (char *) realloc(output, strlen(input) + 4);
            output[positon++] = '\\';
            output[positon++] = (char)((int)input[i]/100+48);
            output[positon++] = (char)((int)input[i]/10+48);
            output[positon++] = (char)((int)input[i]%10+48);
        } else {
            output[positon++] = input[i];
        }
    }
    output[positon] = '\0';
    return output;
}

/**
 * @brief Prevod tokenu na terminal nebo neterminal
 *
 * @param token Ukazatel na aktualni token
 * @param typeStack Zasobnik datovych typu
 * @return Terminal ci neterminal daneho tokenu
 */
TermsAndNonTerms convertTokenType_To_TermsAndNonTerms(Token *token, TypeStack *typeStack) {
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
            if(!isCorrect)
                return E;
            decide = LT;
            isCorrect = false;
            return LT;
        case TOKEN_ID_LTE:
            if(!isCorrect)
                return E;
            decide = LTE;
            isCorrect = false;
            return LTE;
        case TOKEN_ID_GT:
            if(!isCorrect)
                return E;
            decide = GT;
            isCorrect = false;
            return GT;
        case TOKEN_ID_GTE:
            if(!isCorrect)
                return E;
            decide = GTE;
            isCorrect = false;
            return GTE;
        case TOKEN_ID_NEQ2:
            if(!isCorrect)
                return E;
            decide = NEQ;
            isCorrect = false;
            return NEQ;
        case TOKEN_ID_EQ:
            if(!isCorrect)
                return E;
            decide = EQ;
            isCorrect = false;
            return EQ;
        case TOKEN_ID_LBR:
            return LBR;
        case TOKEN_ID_RBR:
            return RBR;
        case TOKEN_ID_ID:
            return ID;
        case TOKEN_ID_INT0:
        case TOKEN_ID_ZERO:
        case TOKEN_ID_INT:
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
        default:
            if (token->Value.keyword == KEYWORD_NIL) {
                TypeStack_push(typeStack, DATA_TYPE_NIL);
                return ID;
            }
            return USD;
    }
}

/**
 * @brief Kontrola kompatibility datoych typu
 *  ADD, SUB, MUL, DIV respektive v tomto poradi +, -, *, /
 *
 * @details Po zpracovani vlozi na zasobnik vysledny datovy typ
 *  | <firstOp>  | <- topElement
 *  |------------|
 *  | <secondOp> |
 *  |------------|
 *  |   ......   |
 *  |___BOTTOM___|
 *  tedy vyraz odpovida:
 *  <secondOp> {+,-,*,/} <firstOp>
 *
 * @param typeStack Zasobnik datovych typu
 * @return Typ erroru generovany analyzou
 */
int checkDataTypes_ADD_SUB_MUL_DIV(TypeStack *typeStack) {
    DataTypes firstOp, secondOp;
    firstOp = TypeStack_pop(typeStack);
    secondOp = TypeStack_pop(typeStack);
    if (firstOp == DATA_TYPE_INTEGER && secondOp == DATA_TYPE_INTEGER) {
        TypeStack_push(typeStack, DATA_TYPE_INTEGER);
    } else if (firstOp == DATA_TYPE_NUMBER && secondOp == DATA_TYPE_INTEGER) {
        // druhe operande je INT tak vyndej druhy ho pretypuj
        printf("POPS GF@!varFloat\n");
        printf("INT2FLOATS\n");
        printf("PUSHS GF@!varFloat\n");
        TypeStack_push(typeStack, DATA_TYPE_NUMBER);
    } else if (firstOp == DATA_TYPE_INTEGER && secondOp == DATA_TYPE_NUMBER) {
        // prvni operande je INT tak ho pretypuj
        printf("INT2FLOATS\n");
        TypeStack_push(typeStack, DATA_TYPE_NUMBER);
    } else if (firstOp == DATA_TYPE_NUMBER && secondOp == DATA_TYPE_NUMBER) {
        TypeStack_push(typeStack, DATA_TYPE_NUMBER);
    } else {
        return ERROR_SEM_COMPAT;
    }
    return ERROR_PASSED;
}

/**
 * @brief Kontrola kompatibility datoveho typu
 *  DIV2 respektive //
 *
 * @details Po zpracovani vlozi na zasobnik vysledny datovy typ
 *  | <firstOp>  | <- topElement
 *  |------------|
 *  | <secondOp> |
 *  |------------|
 *  |   ......   |
 *  |___BOTTOM___|
 *  tedy vyraz odpovida:
 *  <secondOp> {//} <firstOp>
 *
 * @param typeStack Zasobnik datovych typu
 * @return Typ erroru generovany analyzou
 */
int checkDataTypes_DIV2(TypeStack *typeStack) {
    DataTypes firstOP, secondOp;
    firstOP = TypeStack_pop(typeStack);
    secondOp = TypeStack_pop(typeStack);
    if (firstOP == DATA_TYPE_INTEGER && secondOp == DATA_TYPE_INTEGER) {
        TypeStack_push(typeStack, DATA_TYPE_INTEGER);
    } else {
        return ERROR_SEM_COMPAT;
    }
    return ERROR_PASSED;
}

/**
 * @brief Kontrola kompatibility datoych typu
 *  LT, GT respektive v tomto poradi <, >
 *  | <firstOp>  | <- topElement
 *  |------------|
 *  | <secondOp> |
 *  |------------|
 *  |   ......   |
 *  |___BOTTOM___|
 *  tedy vyraz odpovida:
 *  <secondOp> {<,>} <firstOp>
 *
 * @details Po zpracovani vlozi na zasobnik vysledny datovy typ
 *
 * @param typeStack Zasobnik datovych typu
 * @return Typ erroru generovany analyzou
 */
int checkDataTypes_LT_GT(TypeStack *typeStack) {
    DataTypes firstOp, secondOp;
    firstOp = TypeStack_pop(typeStack);
    secondOp = TypeStack_pop(typeStack);
    if (firstOp == DATA_TYPE_INTEGER && secondOp == DATA_TYPE_INTEGER) {
        printf("POPS <tmp>\n");
        printf("MOVE GF@<tmp1> <tmp>\n");
        TypeStack_push(typeStack, DATA_TYPE_INTEGER);
    } else if (firstOp == DATA_TYPE_NUMBER && secondOp == DATA_TYPE_INTEGER) {
        printf("POPS <var>\n");
        printf("INT2FLOATS\n");
        printf("PUSHS <var>\n");
        printf("POPS <tmp>\n");
        printf("MOVE GF@<tmp1> <tmp>\n");
        TypeStack_push(typeStack, DATA_TYPE_NUMBER);
    } else if (firstOp == DATA_TYPE_INTEGER && secondOp == DATA_TYPE_NUMBER) {
        printf("INT2FLOATS\n");
        printf("POPS <tmp>\n");
        printf("MOVE GF@<tmp1> <tmp>\n");
        TypeStack_push(typeStack, DATA_TYPE_NUMBER);
    } else if (firstOp == DATA_TYPE_NUMBER && secondOp == DATA_TYPE_NUMBER) {
        printf("POPS <tmp>\n");
        printf("MOVE GF@<tmp1> <tmp>\n");
        TypeStack_push(typeStack, DATA_TYPE_NUMBER);
    } else {
        return ERROR_SEM_COMPAT;
    }
    return ERROR_PASSED;
}

/**
 * @brief Kontrola kompatibility datoych typu
 *  EQ, NEQ respetive v tomto poradi ==, ~=
 *
 * @details Po zpracovani vlozi na zasobnik vysledny datovy typ
 *  | <firstOp>  | <- topElement
 *  |------------|
 *  | <secondOp> |
 *  |------------|
 *  |   ......   |
 *  |___BOTTOM___|
 *  tedy vyraz odpovida:
 *  <secondOp> {==, ~=} <firstOp>
 *
 * @param typeStack Zasobnik datovych typu
 * @return Typ erroru generovany analyzou
 */
int checkDataTypes_EQ_NEQ(TypeStack *typeStack) {
    DataTypes firstOp, secondOp;
    firstOp = TypeStack_pop(typeStack);
    secondOp = TypeStack_pop(typeStack);
    if (firstOp == DATA_TYPE_INTEGER && secondOp == DATA_TYPE_INTEGER) {
        printf("POPS TF@_tmp\n");
        printf("MOVE TF@_tmp1 TF@_tmp\n");
        TypeStack_push(typeStack, DATA_TYPE_INTEGER);
    } else if (firstOp == DATA_TYPE_NUMBER && secondOp == DATA_TYPE_INTEGER) {
        printf("POPS GF@!varFloat\n");
        printf("INT2FLOATS\n");
        printf("PUSHS GF@!varFloat\n");
        printf("POPS TF@_tmp\n");
        printf("MOVE TF@_tmp1 TF@_tmp\n");
        TypeStack_push(typeStack, DATA_TYPE_NUMBER);
    } else if (firstOp == DATA_TYPE_INTEGER && secondOp == DATA_TYPE_NUMBER) {
        printf("INT2FLOATS\n");
        printf("POPS TF@_tmp\n");
        printf("MOVE TF@_tmp1 TF@_tmp\n");
        TypeStack_push(typeStack, DATA_TYPE_NUMBER);
    } else if (firstOp == DATA_TYPE_NUMBER && secondOp == DATA_TYPE_NUMBER) {
        printf("POPS TF@_tmp\n");
        printf("MOVE TF@_tmp1 TF@_tmp\n");
        TypeStack_push(typeStack, DATA_TYPE_NUMBER);
    } else if (firstOp == DATA_TYPE_NIL && secondOp == DATA_TYPE_NIL) {
        printf("POPS TF@_tmp>\n");
        printf("MOVE TF@_tmp1 TF@_tmp\n");
        TypeStack_push(typeStack, DATA_TYPE_NIL);
    } else if (firstOp == DATA_TYPE_STRING && secondOp == DATA_TYPE_STRING) {
        printf("POPS TF@_tmp\n");
        printf("MOVE TF@_tmp1 TF@_tmp\n");
        TypeStack_push(typeStack, DATA_TYPE_STRING);
    } else {
        return ERROR_SEM_COMPAT;
    }
    return ERROR_PASSED;
}

/**
 * @brief Kontrola kompatibility datoych typu
 *  LTE, GTE respektive v tomto poradi <=, >=
 *
 * @details Po zpracovani vlozi na zasobnik vysledny datovy typ
 *  | <firstOp>  | <- topElement
 *  |------------|
 *  | <secondOp> |
 *  |------------|
 *  |   ......   |
 *  |___BOTTOM___|
 *  tedy vyraz odpovida:
 *  <secondOp> {<=, >=} <firstOp>
 *
 * @param typeStack Zasobnik datovych typu
 * @return Typ erroru generovany analyzou
 */
int checkDataTypes_LTE_GTE(TypeStack *typeStack) {
    DataTypes firstOp, secondOp;
    firstOp = TypeStack_pop(typeStack);
    secondOp = TypeStack_pop(typeStack);
    if (firstOp == DATA_TYPE_INTEGER && secondOp == DATA_TYPE_INTEGER) {
        printf("POPS GF@<tmp>\n");
        printf("MOVE GF@<tmp1> GF@<tmp>\n");
        TypeStack_push(typeStack, DATA_TYPE_INTEGER);
    } else if (firstOp == DATA_TYPE_NUMBER && secondOp == DATA_TYPE_INTEGER) {
        printf("POPS GF@<var>\n");
        printf("INT2FLOATS\n");
        printf("PUSHS GF@<var>\n");
        printf("POPS GF@<tmp>\n");
        printf("MOVE GF@<tmp1> GF@<tmp>\n");
        TypeStack_push(typeStack, DATA_TYPE_NUMBER);
    } else if (firstOp == DATA_TYPE_INTEGER && secondOp == DATA_TYPE_NUMBER) {
        printf("INT2FLOATS\n");
        printf("POPS GF@<tmp>\n");
        printf("MOVE GF@<tmp1> GF@<tmp>\n");
        TypeStack_push(typeStack, DATA_TYPE_NUMBER);
    } else if (firstOp == DATA_TYPE_NUMBER && secondOp == DATA_TYPE_NUMBER) {
        printf("POPS GF@<tmp>\n");
        printf("MOVE GF@<tmp1> GF@<tmp>\n");
        TypeStack_push(typeStack, DATA_TYPE_NUMBER);
    } else if (firstOp == DATA_TYPE_NIL && secondOp == DATA_TYPE_NIL) {
        printf("POPS GF@<tmp>\n");
        printf("MOVE GF@<tmp1> GF@<tmp>\n");
        TypeStack_push(typeStack, DATA_TYPE_NIL);
    } else {
        return ERROR_SEM_COMPAT;
    }
    return ERROR_PASSED;
}

/**
 * @brief Kontrola kompatibility datoveho typu DDOT
 *
 * @details Po zpracovani vlozi na zasobnik vysledny datovy typ
 *  | <firstOp>  | <- topElement
 *  |------------|
 *  | <secondOp> |
 *  |------------|
 *  |   ......   |
 *  |___BOTTOM___|
 *  tedy vyraz odpovida: <secondOp> .. <firstOp>
 *
 * @param typeStack Zasobnik datovych typu
 * @return Typ erroru generovany analyzou
 */
int checkDataTypes_DDOT(TypeStack *typeStack) {
    DataTypes firstOp, secondOp;
    firstOp = TypeStack_pop(typeStack);
    secondOp = TypeStack_pop(typeStack);
    if (firstOp == DATA_TYPE_STRING && secondOp == DATA_TYPE_STRING) {
        printf("POPS GF@<tmp>\n");
        printf("MOVE GF@<tmp1> GF@<tmp>\n");
        TypeStack_push(typeStack, DATA_TYPE_STRING);
    } else {
        return ERROR_SEM_COMPAT;
    }
    return ERROR_PASSED;
}

/**
 * @brief Gramaticka pravidla
 * 
 * @param termStack Ukazatel na strukturu jednosmerne vazaneho seznamu
 * @return int Typ erroru generovany analyzou
 */
int checkRulesAndApply(TermStack *termStack, TypeStack *typeStack) {
    TermStackElementPtr ptr = NULL;
    TermStack_top(termStack, &ptr);
    if (ptr->data == ID && ptr->previousElement->data == R) {
        TermStack_applyReduce(termStack);
    } else if (ptr->data == EXP && ptr->previousElement->data == ADD &&
               ptr->previousElement->previousElement->data == EXP &&
               ptr->previousElement->previousElement->previousElement->data == R) {
        if (checkDataTypes_ADD_SUB_MUL_DIV(typeStack)) return ERROR_SEM_COMPAT;
        // TODO instrukce ADDS
        printf("ADDS\n");
        TermStack_applyReduce(termStack);
    } else if (ptr->data == EXP && ptr->previousElement->data == SUB &&
               ptr->previousElement->previousElement->data == EXP &&
               ptr->previousElement->previousElement->previousElement->data == R) {
        if (checkDataTypes_ADD_SUB_MUL_DIV(typeStack)) return ERROR_SEM_COMPAT;
        // TODO instrukce SUBS
        printf("SUBS\n");
        TermStack_applyReduce(termStack);
    } else if (ptr->data == EXP && ptr->previousElement->data == MUL &&
               ptr->previousElement->previousElement->data == EXP &&
               ptr->previousElement->previousElement->previousElement->data == R) {
        if (checkDataTypes_ADD_SUB_MUL_DIV(typeStack)) return ERROR_SEM_COMPAT;
        // TODO istrukce MULS
        printf("MULS\n");
        TermStack_applyReduce(termStack);
    } else if (ptr->data == EXP && ptr->previousElement->data == DIV &&
               ptr->previousElement->previousElement->data == EXP &&
               ptr->previousElement->previousElement->previousElement->data == R) {
        if (checkDataTypes_ADD_SUB_MUL_DIV(typeStack)) return ERROR_SEM_COMPAT;
        // TODO istrukce DIVS
        printf("DIVS\n");
        TermStack_applyReduce(termStack);
    } else if (ptr->data == EXP && ptr->previousElement->data == DIV2 &&
               ptr->previousElement->previousElement->data == EXP &&
               ptr->previousElement->previousElement->previousElement->data == R) {
        if (checkDataTypes_DIV2(typeStack)) return ERROR_SEM_COMPAT;
        // TODO instrukce IDIVS
        printf("IDIVS\n");
        TermStack_applyReduce(termStack);
    } else if (ptr->data == EXP && ptr->previousElement->data == DDOT &&
               ptr->previousElement->previousElement->data == EXP &&
               ptr->previousElement->previousElement->previousElement->data == R) {
        if (checkDataTypes_DDOT(typeStack)) return ERROR_SEM_COMPAT;
        // TODO instrukce CONCAT (POPS <str1>(firstOp), POPS <str2>(secondOp), CONCAT <var> <str2> <str1> PUSHS <var>(str2..str1))
        printf("POPS <str1>\n");
        printf("POPS <str2>\n");
        printf("CONCAT <var> <str2> <str1>\n");
        printf("PUSHS <var>\n");
        TermStack_applyReduce(termStack);
    } else if (ptr->data == RBR && ptr->previousElement->data == EXP &&
               ptr->previousElement->previousElement->data == LBR &&
               ptr->previousElement->previousElement->previousElement->data == R) {
        TermStack_applyReduce(termStack);
    } else if (ptr->data == EXP && ptr->previousElement->data == LT &&
               ptr->previousElement->previousElement->data == EXP &&
               ptr->previousElement->previousElement->previousElement->data == R) {
        if (checkDataTypes_LT_GT(typeStack)) return ERROR_SEM_COMPAT;
        TermStack_applyReduce(termStack);
    } else if (ptr->data == EXP && ptr->previousElement->data == GT &&
               ptr->previousElement->previousElement->data == EXP &&
               ptr->previousElement->previousElement->previousElement->data == R) {
        if (checkDataTypes_LT_GT(typeStack)) return ERROR_SEM_COMPAT;
        TermStack_applyReduce(termStack);
    } else if (ptr->data == EXP && ptr->previousElement->data == LTE &&
               ptr->previousElement->previousElement->data == EXP &&
               ptr->previousElement->previousElement->previousElement->data == R) {
        if (checkDataTypes_LTE_GTE(typeStack)) return ERROR_SEM_COMPAT;
        TermStack_applyReduce(termStack);
    } else if (ptr->data == EXP && ptr->previousElement->data == GTE &&
               ptr->previousElement->previousElement->data == EXP &&
               ptr->previousElement->previousElement->previousElement->data == R) {
        if (checkDataTypes_LTE_GTE(typeStack)) return ERROR_SEM_COMPAT;
        TermStack_applyReduce(termStack);
    } else if (ptr->data == EXP && ptr->previousElement->data == EQ &&
               ptr->previousElement->previousElement->data == EXP &&
               ptr->previousElement->previousElement->previousElement->data == R) {
        if (checkDataTypes_EQ_NEQ(typeStack)) return ERROR_SEM_COMPAT;
        TermStack_applyReduce(termStack);
    } else if (ptr->data == EXP && ptr->previousElement->data == NEQ &&
               ptr->previousElement->previousElement->data == EXP &&
               ptr->previousElement->previousElement->previousElement->data == R) {
        if (checkDataTypes_EQ_NEQ(typeStack)) return ERROR_SEM_COMPAT;
        TermStack_applyReduce(termStack);
    } else if (ptr->data == EXP && ptr->previousElement->data == LEN &&
               ptr->previousElement->previousElement->data == R) {
        DataTypes firstOp = TypeStack_pop(typeStack);
        if (firstOp == DATA_TYPE_STRING) {
            TypeStack_push(typeStack, DATA_TYPE_INTEGER);
        } else {
            return ERROR_SEM_COMPAT;
        }
        // TODO intrukce STRLEN (POPS <str>(firstOp), STRLEN <var> <str>, PUSHS <var>)
        printf("POPS <str>\n");
        printf("STRLEN <var> <str>\n");
        printf("PUSHS <var>\n");
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
int exprSyntaxCheck(Token *token, FILE *file, SLList_Frame *listFrame, Data_type retData, char *var) {
    bool isNew = true;
    int error;
    bst_node_t *ptr_node = NULL;
    // pridelim pamet zasobniku datovych typu
    TypeStack *typeStack = (TypeStack *) malloc(sizeof(TypeStack));
    if (!typeStack) return ERROR_COMPILER;
    TypeStack_init(typeStack);
    // pridelim pamet zasobniku terminalu a neterminalu
    TermStack *termStack = (TermStack *) malloc(sizeof(TermStack));
    if (!termStack) return ERROR_COMPILER;
    TermStack_init(termStack);
    // vlozim $
    TermStack_push(termStack, USD);

    struct TermStackElement *top = NULL;
    // zjistim si dalsi token
    skipNonPrintChar(token, file);
    // proslo to, tak prelozime token na terminal
    TermsAndNonTerms vstup = convertTokenType_To_TermsAndNonTerms(token, typeStack);
    if(vstup == E) {
        freeStacks(termStack, typeStack);
        return ERROR_SEM_OTHERS;
    }
    do {
        TermStack_firstTermPtr(termStack, &top);
        if ((prev == TOKEN_ID_DIV2 || prev == TOKEN_ID_DIV) && (token->ID == TOKEN_ID_ZERO || token->ID == TOKEN_ID_INT0 || token->Value.Double == 0.0)) {
            freeStacks(termStack, typeStack);
            return ERROR_RUNTIME_DIV_ZERO;
        }
        // jedna se o identifikator?
        if (token->ID == TOKEN_ID_ID && p_table[(top->data - 4)][vstup - 4] != H && vstup != USD) {
            /* chujoviny zacinaji tady */
            //ptr_node = search_Iden(token->Value.string, listFrame);
            SLLElementPtr_Frame *ptrFrame = (SLLElementPtr_Frame *) malloc(sizeof(SLLElementPtr_Frame));
            if(!ptrFrame) return ERROR_COMPILER;
            (*ptrFrame) = listFrame->TopLocalElement;
            // iterace ramci dokud ptrFrame neni global && dokud !isInitVar(bst_search((*ptrFrame)->node, token->Value.string)))

            while(true) {
                if ((*ptrFrame) == listFrame->globalElement) {
                    free(ptrFrame);
                    freeStacks(termStack, typeStack);
                    return ERROR_SEM_UNDEFINED;
                }
                ptr_node = bst_search((*ptrFrame)->node, token->Value.string);
                if (ptr_node != NULL && isInitVar(ptr_node)) {
                    printf("PUSHS TF@_%s\n", token->Value.string->str);
                    if (ptr_node->varData->type == TYPE_STRING) {
                        TypeStack_push(typeStack, DATA_TYPE_STRING);
                    } else if (ptr_node->varData->type == TYPE_NUMBER) {
                        TypeStack_push(typeStack, DATA_TYPE_NUMBER);
                    } else if (ptr_node->varData->type == TYPE_INTEGER) {
                        TypeStack_push(typeStack, DATA_TYPE_INTEGER);
                    } else {
                        freeStacks(termStack, typeStack);
                        return ERROR_SEM_UNDEFINED;
                    }
                    break;
                }
                (*ptrFrame) = (*ptrFrame)->previousElement;
            }

            /* chujoviny konci tady */
        }
        // podle pravidla v tabulce rozhodnu co budu delat
        switch (p_table[(top->data - 4)][vstup - 4]) {
            // terminaly jsou si rovny
            case E:
                TermStack_push(termStack, vstup);
                // zjistim si dalsi token
                skipNonPrintChar(token, file);
                // jedna se o identifikator?
                if (token->ID == TOKEN_ID_ID) {
                    if (ptr_node != NULL) {
                        // pokud to neni to promenna a je inicializovane?
                        // muze to ale byt buldin funkce (write, readi, ...)
                        if (isFnc(ptr_node) || !isInitVar(ptr_node)) {
                            freeStacks(termStack, typeStack);
                            return ERROR_SEM_UNDEFINED;
                        }
                    }
                }
                if ((prev == TOKEN_ID_DIV2 || prev == TOKEN_ID_DIV) && (token->Value.Double == 0.0 || token->Value.Integer == 0))
                    return ERROR_RUNTIME_DIV_ZERO;
                // proslo to, tak prelozime token na terminal
                vstup = convertTokenType_To_TermsAndNonTerms(token, typeStack);
                if(vstup == E) {
                    freeStacks(termStack, typeStack);
                    return ERROR_SEM_OTHERS;
                }
                break;
                // vlozim znak redukce
            case R:
                // TODO instrukce pushnuti na zasobnik kdyz operand
                //      PUSHS <symb>
                if(isNew) {
                    if(token->ID == TOKEN_ID_INT || token->ID == TOKEN_ID_INT0 || token->ID == TOKEN_ID_ZERO )
                        printf("PUSHS int@%lld\n", token->Value.Integer);
                    if(token->ID == TOKEN_ID_DBL2 || token->ID == TOKEN_ID_EXP3 || token->ID == TOKEN_ID_DHEX2 || token->ID == TOKEN_ID_HEXP3)
                        printf("PUSHS float@%a\n", token->Value.Double);
                    if(token->ID == TOKEN_ID_FSTR) {
                        char *str = converString(token->Value.string->str);
                        printf("PUSHS string@%s\n", str);
                        free(str);
                    }
                    if(token->Value.keyword == KEYWORD_NIL)
                        printf("PUSHS nil@nil\n");
                }
                TermStack_insertReduce(termStack, top);
                TermStack_push(termStack, vstup);
                // zjisitm si dalsi token
                skipNonPrintChar(token, file);
                isNew = true;
                vstup = convertTokenType_To_TermsAndNonTerms(token, typeStack);
                if(vstup == E) {
                    freeStacks(termStack, typeStack);
                    return ERROR_SEM_OTHERS;
                }
                break;
                // zredukuju po nejblizsi znak redukce
            case I:
                isNew = false;
                error = checkRulesAndApply(termStack, typeStack);
                // neexistije pro redukci pravidlo
                if (error) {
                    freeStacks(termStack, typeStack);
                    return error;
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
    } while (vstup != USD || !SA_isOK(termStack)); // opakuji dokud vstup neni $ a dokud muzu redukovat
    //if(listFrame->globalElement->node->funcData->returnList->firstElement->type != typeStack->topElement->data) return ERROR_SEM_TYPE_COUNT;

    //kontrola vyslednych datovych typu
    if(retData == TYPE_INTEGER) {
        if (typeStack->topElement->data == DATA_TYPE_STRING) {
            freeStacks(termStack, typeStack);
            return ERROR_SEM_ASSIGN;
        }
        if (typeStack->topElement->data == DATA_TYPE_NUMBER)
            printf("FLOAT2INTS\n");
    } else if (retData == TYPE_NUMBER) {
        if (typeStack->topElement->data == DATA_TYPE_STRING) {
            freeStacks(termStack, typeStack);
            return ERROR_SEM_ASSIGN;
        }
        if (typeStack->topElement->data == DATA_TYPE_INTEGER)
            printf("INT2FLOATS\n");
    } else if (retData == TYPE_STRING) {
        if (typeStack->topElement->data == DATA_TYPE_INTEGER) {
            freeStacks(termStack, typeStack);
            return ERROR_SEM_ASSIGN;
        }
        if (typeStack->topElement->data == DATA_TYPE_NUMBER) {
            freeStacks(termStack, typeStack);
            return ERROR_SEM_ASSIGN;
        }
    }

    // neni tam rovnost nebo nerovnost
    if (!isCorrect) {
        printf("POPS TF@_tmp\n");
        printf("MOVE TF@_tmp2 TF@_tmp\n");
    } else {
        printf("POPS TF@_%s\n", var);
        //printf("POPS GF@<tmp>\n");
        //printf("MOVE LF@<var> GF@<tmp>\n");
    }
    // je tam rovnost nebo nerovnost
    switch (decide) {
        case EQ:
            //printf("PUSHS TF@_tmp1\n");
            //printf("PUSHS TF@_tmp2\n");
            printf("JUMPIFNEQ !else%d TF@_tmp2 TF@_tmp1\n", elseCounter++);
            break;
        case NEQ:
            printf("JUMPIFEQ !else%d TF@_tmp2 TF@_tmp1\n", elseCounter++);
            break;
        case LT:
            printf("LTS\n");
            printf("PUSHS bool@true\n");
            printf("JUMPIFEQS <label>\n");
            break;
        case LTE:
            printf("LT <bool> <tmp2> <tmp1>\n");
            printf("EQS\n");
            printf("PUSHS bool@<bool>\n");
            printf("JUMPIFEQS <label>\n");
            break;
        case GT:
            printf("GTS\n");
            printf("PUSHS bool@true\n");
            printf("JUMPIFEQS <label>\n");
            break;
        case GTE:
            printf("GT <bool> <tmp2> <tmp1>\n");
            printf("EQS\n");
            printf("PUSHS bool@<bool>\n");
            printf("JUMPIFEQS <label>\n");
            break;
        default:
            break;
    }
    isCorrect = 1;
    //uvolnim pamet zasobniku
    freeStacks(termStack, typeStack);
    decide = USD;
    isCorrect = 1;
    fsetpos(file, &last_read_pos);
    return ERROR_PASSED;
}

/**
 * @brief Uvolneni pameti zasobniku
 *
 * @param termStack Ukazatel na zasobnik datovych typu
 * @param typeStack Ukazatel na zasobmil terminalu a neterminalu
 */
void freeStacks(TermStack *termStack, TypeStack *typeStack) {
    TypeStack_dispose(typeStack);
    free(typeStack);
    TermStack_dispose(termStack);
    free(termStack);
}
