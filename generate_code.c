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
void make_CREATEFRAME(){
    printf("CREATEFRAME\n");
}
void make_PUSHFRAME(){
    printf("PUSHFRAME\n");
}
void make_POPFRAME(){
    printf("POPFRAME\n");
}

/**
 * Funkce vytvori kod pro volani funkce.
 *
 */
void make_CALL(Dynamic_string *string){
    printf("CALL $%s\n", string->str);
}
/**
 * Funkce vytvori kod pro skakani na konec funce.
 *
 */
void make_JUMP_end_fnc(Dynamic_string *string){
    printf("#SKOCI NA KONEC FUNKCE %s\n", string->str);
    printf("JUMP $$end_fnc_%s\n", string->str);
}

/**
 * Funkce vytvori kod pro skakani na konec ifu.
 *
 */
void make_JUMP_end_if(int ifCounter){
    printf("#SKOCI NA KONEC %d. IFU\n", ifCounter);
    printf("JUMP !endif%d\n", ifCounter);
}

/**
 * Funkce vytvori kod pro label funkce.
 *
 */
void make_LABEL_fnc(Dynamic_string *string){
    printf("LABEL $%s\n", string->str);
}

/**
 * Funkce vytvori kod pro label elsu.
 *
 */
void make_LABEL_else(int ifCounter){
    printf("LABEL !else%d\n", ifCounter);
}

/**
 * Funkce vytvori kod pro label endifu.
 *
 */
void make_LABEL_endif(int ifCounter){
    printf("LABEL !endif%d\n", ifCounter);
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
    printf("#VYTVORENI RETURN VARIABLE\n");
    printf("DEFVAR LF@*return_%s\n", string->str);
    printf("MOVE LF@*return_%s nil@nil\n", string->str);
}

/**
 * Funkce vytvori kod pro promenou na localframu.
 *
 */
void make_DEFVAR_LF(Dynamic_string *string){
    printf("#DEKLARACE PROMENNE %s V LOCALFRAMU\n", string->str);
    printf("DEFVAR LF@&%s\n", string->str);
}

/**
 * Funkce vytvori kod pro nastaveni na nil pro promenou localframu.
 *
 */
void make_MOVE_TF_nil(Dynamic_string *string){
    printf("#NASTAVENI PROMENNE _%s NA NIL\n", string->str);
    printf("MOVE LF@&%s nil@nil\n", string->str);
}

/**
 * Funkce vytvori kod pro promenou temporyframu.
 *
 */
void make_DEFVAR_TF(Dynamic_string *string){
    printf("#DEKLARACE PROMENNE %s V TEMPORARYFRAMU\n", string->str);
    printf("DEFVAR TF@&%s\n", string->str);
}

/**
 * Funkce vytvori kod pro nastaveni na nil pro promenou temporyframu.
 *
 */
void make_MOVE_TF_nil(Dynamic_string *string){
    printf("#NASTAVENI PROMENNE _%s NA NIL\n", string->str);
    printf("MOVE TF@&%s nil@nil\n", string->str);
}

/**
 * Funkce vytvori kod pro vypsani v localframu.
 *
 */
void make_WRITE_LF(Dynamic_string *string){
    printf("#VYPSANI %s V LOCALFRAMU\n", string->str);
    printf("WRITE LF@&%s\n", string->str);
}
/**
 * Funkce vytvori kod pro vypsani v temporyframu.
 *
 */
void make_WRITE_TF(Dynamic_string *string){
    printf("#VYPSANI %s V TEMPORARYFRAMU\n", string->str);
    printf("WRITE TF@&%s\n", string->str);
}
/**
 * Funkce vytvori kod pro pomocne promenne.
 *
 */
void make_CREATEFRAME_TMP(){
    printf("CREATEFRAME\n");
    printf("#VYTVORENI DOCASNYCH PROMENNYCH\n");
    printf("DEFVAR TF@&tmp\n");
    printf("DEFVAR TF@&tmp1\n");
    printf("DEFVAR TF@&tmp2\n");
    printf("#----------------\n");
}

/**
 * Funkce vytvori kod pro konec funkce.
 *
 */
void make_endOfFunc(Dynamic_string *string){
    printf("#KONEC FUNKCE %s\n", string->str);
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $$end_fnc_%s\n", string->str);
}

/**
 * Funkce vytvori kod pro nahrani vsech promennych do noveho ramce.
 *
 */
void movePrevious(DLList_Instruct *dlListInstruct) {
    printf("#DEKLAROVANI PREDCHOZICH PROMENNYCH\n")
    DLLElementPtr_Instruct pointer = dlListInstruct->firstElement;
    while (pointer != NULL) {
        printf("DEFVAR TF@&%s\n", pointer->instruction->str);
        printf("MOVE TF@&%s LF@&%s\n", pointer->instruction->str, pointer->instruction->str);
        pointer = pointer->nextElement;
    }
    printf("#------------------------------\n")
}

/**
 * Funkce vytvori kod pro POPS a MOVE pomocne promenne.
 *
 */
void make_POPSandMOVE_tmp1(){
    printf("#POPS a MOVE DOCASNYCH PROMENNYCH\n");
    printf("POPS TF@&tmp\n");
    printf("MOVE TF@&tmp1 TF@&tmp\n");
}

/**
 * Funkce vytvori kod pro POPS a MOVE pomocne promenne.
 *
 */
void make_POPSandMOVE_tmp2(){
    printf("#POPS a MOVE DOCASNYCH PROMENNYCH\n");
    printf("POPS TF@&tmp\n");
    printf("MOVE TF@&tmp2 TF@&tmp\n");
}

void make_INT2FLOATS(){
    printf("INT2FLOATS\n");
}

void make_FLOAT2INTS(){
    printf("FLOAT2INTS\n");
}

/**
 * Funkce vytvori kod pro POPS a PUSH promenne na pretypovani na float.
 *
 */
void make_POPSandPUSH_float(){
    printf("POPS TF@&varFloat\n");
    printf("INT2FLOATS\n");
    printf("PUSHS TF@&varFloat\n");
}