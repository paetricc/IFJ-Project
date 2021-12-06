/**
 * Projekt: IFJ2021
 *
 * @brief Implementace generovani kodu a vestavenych funkci.
 *
 * @author Vít Janeček xjanec30@stud.fit.vutbr.cz
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "builtIn.h"

#include "generate_code.h"

void make_header(){
    printf(".IFJcode2021\n");
}

/**
 * Funkce vytvori kod pro hlavicku funkce.
 *
 */
void make_func_header(Dynamic_string *string){
    printf("JUMP $$end_fnc_%s\n", string->str);
    printf("LABEL $%s\n", string->str);
    printf("PUSHFRAME\n");
}

/**
 * Funkce vytvori kod pro promenou return.
 *
 */
void make_noVoidReturn(Dynamic_string *string){
    printf("DEFVAR LF@*return_%s\n", string->str);
    printf("MOVE LF@*return_%s nil@nil\n", string->str);
}

void make_DEFVAR(Dynamic_string *string){
    printf("DEFVAR LF@_%s\n", string->str);
}

/**
 * Funkce vytvori kod pro pomocne promenne.
 *
 */
void make_CREATEFRAME_TMP(){
    printf("CREATEFRAME\n");
    printf("#VYTVORENI DOCASNYCH FUNKCI\n");
    printf("DEFVAR LF@_tmp\n");
    printf("DEFVAR LF@_tmp1\n");
    printf("DEFVAR LF@_tmp2\n");
}

/**
 * Funkce vytvori kod pro konec funkce.
 *
 */
void make_endOfFunc(Dynamic_string *string){
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $$end_fnc_%s\n", string->str);
}

/**
 * Funkce vytvori kod pro nahrani vsech promennych do noveho ramce.
 *
 */
void movePrevious(DLList_Instruct *dlListInstruct) {
    DLLElementPtr_Instruct pointer = dlListInstruct->firstElement;
    while (pointer != NULL) {
        printf("DEFVAR TF@_%s\n", pointer->instruction->str);
        printf("MOVE TF@_%s LF@_%s\n", pointer->instruction->str, pointer->instruction->str);
        pointer = pointer->nextElement;
    }
}