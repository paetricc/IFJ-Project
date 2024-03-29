/**
 * Projekt: IFJ2021
 *
 * @brief Implementace generovani kodu a vestavenych funkci.
 *
 * @author Vít Janeček xjanec30@stud.fit.vutbr.cz
 * @author Tony Pham xphamt00@stud.fit.vutbr.cz
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "builtIn.h"

#include "generate_code.h"

/**
 * Funkce vytvori hlavicku, globalni promenne, buildin a pomocne funkce
 *
 */
void make_header() {
    printf(".IFJcode21\n");
    printf("DEFVAR GF@&varFloat\n");
    printf("DEFVAR GF@&varBool\n");
    printf("DEFVAR GF@&if\n");
    printf("DEFVAR GF@&varType\n");
    printf("DEFVAR GF@*return\n");
    make_MOVE_RETURN_NIL();
    printf("DEFVAR GF@&desAct\n");
    printf("DEFVAR GF@&desPrev\n");
    make_WRITE_TF();
    make_write();
    make_reads();
    make_readi();
    make_readn();
    make_substr();
    make_toInteger();
    make_ord();
    make_chr();
    make_NILcompare();
    make_distribute();
}

/** Jednoslovne funkce*/
void make_CREATEFRAME(){
    printf("CREATEFRAME\n");
}
void make_PUSHFRAME(){
    printf("PUSHFRAME\n");
}
void make_POPFRAME(){
    printf("POPFRAME\n");
}
void make_INT2FLOATS(){
    printf("INT2FLOATS\n");
}
void make_FLOAT2INTS(){
    printf("FLOAT2INTS\n");
}
void make_ADDS(){
    printf("ADDS\n");
}
void make_SUBS(){
    printf("SUBS\n");
}
void make_MULS(){
    printf("MULS\n");
}
void make_DIVS(){
    printf("DIVS\n");
}
void make_IDIVS(){
    printf("IDIVS\n");
}
void make_LTS(){
    printf("LTS\n");
}
void make_GTS(){
    printf("GTS\n");
}
void make_EQS(){
    printf("EQS\n");
}
void make_NOTS(){
    printf("NOTS\n");
}
/**
 * Funkce vytvori kod pro volani funkce.
 *
 */
void make_CALL(char *string){
    printf("CALL $%s\n", string);
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
void make_MOVE_LF_nil(Dynamic_string *string){
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
    printf("MOVE TF@&%s nil@nil\n", string->str);
}

/**
 * Funkce vytvori kod pro nastaveni na nil pro promenou temporyframu.
 *
 */
void make_MOVE_TF_nil(char *string){
    printf("#NASTAVENI PROMENNE _%s NA NIL\n", string);
    printf("MOVE TF@&%s nil@nil\n", string);
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
void make_WRITE_TF(){
    printf("JUMP !fnc_call_end\n");
    printf("LABEL !fnc_call\n");
    printf("DEFVAR TF@&varType\n");
    printf("TYPE TF@&varType TF@&typeVar\n");
    printf("JUMPIFEQ !writeNil TF@&varType string@nil\n");
    printf("WRITE TF@&typeVar\n");
    printf("JUMP !writeNilEnd\n");
    printf("LABEL !writeNil\n");
    printf("WRITE string@nil\n");
    printf("LABEL !writeNilEnd\n");
    printf("RETURN\n");
    printf("LABEL !fnc_call_end\n");
}

/**
 * Funkce vytvori kod pro volani funkce.
 *
 */
void makeWriteCall(Dynamic_string *string) {
    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");
    printf("DEFVAR TF@&typeVar\n");
    printf("MOVE TF@&typeVar LF@&%s\n", string->str);
    printf("CALL !fnc_call\n");
    printf("POPFRAME\n");
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
void make_endOfFunc(char *string){
    printf("#KONEC FUNKCE %s\n", string);
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $$end_fnc_%s\n", string);
}

/**
 * Funkce vytvori kod pro nahrani vsech promennych do noveho ramce.
 *
 */
void movePrevious(DLList_Instruct *dlListInstruct) {
    printf("#DEKLAROVANI PREDCHOZICH PROMENNYCH\n");
    DLLElementPtr_Instruct pointer = dlListInstruct->firstElement;
    while (pointer != NULL) {
        printf("DEFVAR TF@&%s\n", pointer->instruction->str);
        printf("MOVE TF@&%s LF@&%s\n", pointer->instruction->str, pointer->instruction->str);
        pointer = pointer->nextElement;
    }
    printf("#------------------------------\n");
}

/**
 * Funkce po zavolanoni distribuce ulozi predchozi promennou.
 *
 */
void moveAfter(DLList_Instruct *dlListInstruct) {
    printf("#DISTRIBUCE ZMENY DOLU\n");
    DLLElementPtr_Instruct pointer = dlListInstruct->firstElement;
    while (pointer != NULL) {
        printf("MOVE GF@&desAct TF@&%s\n", pointer->instruction->str);
        printf("MOVE GF@&desPrev LF@&%s\n", pointer->instruction->str);
        printf("CALL $distrbute\n");
        printf("MOVE LF@&%s GF@&desPrev\n", pointer->instruction->str);
        pointer = pointer->nextElement;
    }
    printf("#>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
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

/**
 * Funkce vytvori kod pro POPS a PUSH promenne na pretypovani na float.
 *
 */
void make_POPSandPUSH_float(){
    printf("POPS GF@&varFloat\n");
    printf("INT2FLOATS\n");
    printf("PUSHS GF@&varFloat\n");
}

/**
 * Funkce vytvori kod pro volanici funkce na porovnavani s nil.
 *
 */
void call_NILcompare() {
    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");
    printf("DEFVAR TF@&var\n");
    printf("MOVE TF@&var LF@&tmp\n");
    printf("CALL $fnc_NILcompare\n");
}

/**
 * Funkce vytvori kod pro porovnavani promenne s nil.
 *
 */
void make_NILcompare() {
    printf("#---------- pomocna NILcompare ----------\n");
    printf("JUMP $$endNILcompare\n");
    printf("LABEL $fnc_NILcompare\n");
    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");
    printf("DEFVAR TF@&varType\n");
    printf("TYPE TF@&varType LF@&var\n");
    printf("JUMPIFEQ $errorNil TF@&varType string@nil\n");
    printf("POPFRAME\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $errorNil\n");
    printf("EXIT int@8\n");
    printf("LABEL $$endNILcompare\n");
    printf("#------------------------------\n\n");
}

/**
 * Funkce vytvori kod pro predani promennych k porovnani s nil.
 *
 */
void make_insertNILcompare() {
    printf("MOVE TF@&tmp TF@&tmp2\n");
    call_NILcompare();
    printf("MOVE TF@&tmp2 TF@&tmp\n");
    printf("MOVE TF@&tmp TF@&tmp1\n");
    call_NILcompare();
    printf("MOVE TF@&tmp1 TF@&tmp\n");
}

/**
 * Funkce vytvori kod pro PUSHS promenne localframu.
 *
 */
void make_PUSHS_LF(Dynamic_string *string){
    printf("PUSHS LF@&%s\n", string->str);
}

/**
 * Funkce vytvori kod pro POPS promenne localframu.
 *
 */
void make_POPS_LF(Dynamic_string *string){
    printf("POPS LF@&%s\n", string->str);
}

/**
 * Funkce vytvori kod pro PUSHS promenne temporaryframu.
 *
 */
void make_PUSHS_TF(Dynamic_string *string){
    printf("PUSHS TF@&%s\n", string->str);
}

/**
 * Funkce vytvori kod pro PUSHS int.
 *
 */
void make_PUSHS_int(long long int Integer){
    printf("PUSHS int@&%lld\n", Integer);
}

/**
 * Funkce vytvori kod pro PUSHS float.
 *
 */
void make_PUSHS_float(double DoubleVal){
    printf("PUSHS float@&%a\n", DoubleVal);
}

/**
 * Funkce vytvori kod pro PUSHS string.
 *
 */
void make_PUSHS_string(char *str){
    printf("PUSHS string@&%s\n", str);
}

/**
 * Funkce vytvori kod pro POPS promenne temporaryframu.
 *
 */
void make_POPS_TF(char *var){
    printf("POPS TF@&%s\n", var);
}

/**
 * Funkce vytvori kod pro POPS if.
 *
 */
void make_POPS_GF_if(){
    printf("POPS GF@&if\n");
}

/**
 * Funkce vytvori kod pro PUSHS nil.
 *
 */
void make_PUSHS_nil(){
    printf("PUSHS nil@nil\n");
}

/**
 * Funkce vytvori kod pro CONCAT.
 *
 */
void make_CONTAT(){
    printf("POPS TF@&tmp2\n");
    printf("CONCAT TF@&tmp  TF@&tmp2 TF@&tmp1\n");
    printf("PUSHS TF@&tmp\n");
}

/**
 * Funkce vytvori kod pro STRLEN.
 *
 */
void make_STRLEN(){
    printf("POPS TF@&tmp1\n");
    printf("STRLEN TF@&tmp TF@&tmp1\n");
    printf("PUSHS TF@&tmp\n");
}

/**
 * Funkce vytvori kod pro cely LT.
 * Potreba dat do funkce inkrementovanou hodnotu
 */
void make_LT_all(int elseCounter){
    printf("PUSHS TF@&tmp2\n");
    printf("PUSHS TF@&tmp1\n");
    printf("LTS\n");
    printf("PUSHS bool@true\n");
    printf("JUMPIFNEQS !else%d\n", elseCounter);
}

/**
 * Funkce vytvori kod pro cely LTE.
 * Potreba dat do funkce inkrementovanou hodnotu
 */
void make_LTE_all(int elseCounter){
    printf("PUSHS TF@&tmp2\n");
    printf("PUSHS TF@&tmp1\n");
    printf("GTS\n");
    printf("NOTS\n");
    printf("PUSHS bool@true\n");
    printf("JUMPIFNEQS !else%d\n", elseCounter);
}

/**
 * Funkce vytvori kod pro cely GT.
 * Potreba dat do funkce inkrementovanou hodnotu
 */
void make_GT_all(int elseCounter){
    printf("PUSHS TF@&tmp2\n");
    printf("PUSHS TF@&tmp1\n");
    printf("GTS\n");
    printf("PUSHS bool@true\n");
    printf("JUMPIFNEQS !else%d\n", elseCounter);
}

/**
 * Funkce vytvori kod pro cely GTE.
 * Potreba dat do funkce inkrementovanou hodnotu
 */
void make_GTE_all(int elseCounter){
    printf("PUSHS TF@&tmp2\n");
    printf("PUSHS TF@&tmp1\n");
    printf("LTS\n");
    printf("NOTS\n");
    printf("PUSHS bool@true\n");
    printf("JUMPIFNEQS !else%d\n", elseCounter);
}

/**
 * Funkce vytvori kod pro cely LT s loopem.
 * Potreba dat do funkce inkrementovanou hodnotu
 */
void make_LT_loop(){
    printf("PUSHS TF@&tmp2\n");
    printf("PUSHS TF@&tmp1\n");
    printf("LTS\n");
    printf("PUSHS bool@true\n");
}

/**
 * Funkce vytvori kod pro cely LTE s loopem.
 * Potreba dat do funkce inkrementovanou hodnotu
 */
void make_LTE_loop(){
    printf("PUSHS TF@&tmp2\n");
    printf("PUSHS TF@&tmp1\n");
    printf("GTS\n");
    printf("NOTS\n");
    printf("PUSHS bool@true\n");
}

/**
 * Funkce vytvori kod pro cely GT s loopem.
 * Potreba dat do funkce inkrementovanou hodnotu
 */
void make_GT_loop(){
    printf("PUSHS TF@&tmp2\n");
    printf("PUSHS TF@&tmp1\n");
    printf("GTS\n");
    printf("PUSHS bool@true\n");
}

/**
 * Funkce vytvori kod pro cely GTE s loopem.
 * Potreba dat do funkce inkrementovanou hodnotu
 */
void make_GTE_loop(){
    printf("PUSHS TF@&tmp2\n");
    printf("PUSHS TF@&tmp1\n");
    printf("LTS\n");
    printf("NOTS\n");
    printf("PUSHS bool@true\n");
}

/**
 * Funkce vytvori kod pro if porovnavajici s nil.
 * Potreba dat do funkce inkrementovanou hodnotu
 */
void make_IF_nil(int elseCounter){
    printf("TYPE GF@&varType GF@&if\n");
    printf("JUMPIFEQ !else%d GF@&varType string@nil\n", elseCounter);
}

/**
 * Funkce vytvori kod pro EQ.
 * Potreba dat do funkce inkrementovanou hodnotu
 */
void make_EQ(int elseCounter){
    printf("JUMPIFNEQ !else%d TF@&tmp2 TF@&tmp1\n", elseCounter);
}

/**
 * Funkce vytvori kod pro NEQ.
 * Potreba dat do funkce inkrementovanou hodnotu
 */
void make_NEQ(int elseCounter){
    printf("JUMPIFEQ !else%d TF@&tmp2 TF@&tmp1\n", elseCounter);
}

/**
 * Funkce vytvori kod pro distribuci.
 *
 */
void make_distribute() {
    printf("JUMP $$distributeEnd\n");
    printf("LABEL $distrbute\n");
    printf("JUMPIFEQ !disCheck GF@&desAct GF@&desPrev\n");
    printf("MOVE GF@&desPrev GF@&desAct\n");
    printf("LABEL !disCheck\n");
    printf("RETURN\n");
    printf("LABEL $$distributeEnd\n");
}

/**
 * Funkce vytvori kod pro presun LF do TF.
 *
 */
void make_MOVE_TF_to_LF(Dynamic_string *stringTF, Dynamic_string *stringLF){
    printf("MOVE TF@&%s LF@&%s\n", stringTF->str, stringLF->str);
}

/**
 * Funkce vytvori kod pro prevedeni INT TF do FLOAT TF.
 *
 */
void make_INT2FLOAT_TF_TF(Dynamic_string *stringTF){
    printf("INT2FLOAT TF@&%s TF@&%s\n", stringTF->str, stringTF->str);
}

/**
 * Funkce vytvori kod pro zapsani int hodnoty do TF.
 *
 */
void make_MOVE_TF_INT(Dynamic_string *stringTF , long long int x) {
    printf("MOVE TF@&%s int@%lld\n", stringTF->str, x);
}

/**
 * Funkce vytvori kod pro vytvoreni docasnych promennych pro loop.
 *
 */
void make_LOOP_TMP(int loopCounter) {
    printf("#VYTVORENI DOCASNYCH PROMENNYCH\n");
    printf("DEFVAR TF@&tmp\n");
    printf("DEFVAR TF@&tmp1\n");
    printf("DEFVAR TF@&tmp2\n");
    printf("#----------------\n");
    printf("LABEL !loop%d\n", loopCounter);
}

/**
 * Funkce vytvori kod pro skok do loopu.
 *
 */
void make_LOOP_JUMP(int loopCounter) {
    printf("JUMP !loop%d\n", --loopCounter);
    printf("LABEL !endLoop%d\n", loopCounter);
    printf("POPFRAME\n");
}

/**
 * Funkce vytvori kod pro zapsani float hodnoty do TF po pouziti INT2FLOAT.
 *
 */
void make_MOVE_TF_FLOAT_INT2FLOAT(Dynamic_string *stringTF , long long int x) {
    printf("MOVE TF@&%s float@%a\n", stringTF->str, (double )x);
}

/**
 * Funkce vytvori kod pro zapsani float hodnoty do TF.
 *
 */
void make_MOVE_TF_FLOAT(Dynamic_string *stringTF , double x) {
    printf("MOVE TF@&%s float@%a\n", stringTF->str, (double)x);
}

/**
 * Funkce vytvori kod pro zapsani string hodnoty do TF.
 *
 */
void make_MOVE_TF_STRING(Dynamic_string *stringTF , char *string) {
    printf("MOVE TF@&%s string@%s\n", stringTF->str, string);
}

/**
 * Funkce vytvori kod pro zapsani nil do GF*return.
 *
 */
void make_MOVE_RETURN_NIL() {
    printf("MOVE GF@*return nil@nil\n");
}

/**
 * Funkce vytvori kod pro vytvoreni TF return.
 *
 */
void make_TF_RETURN() {
    printf("DEFVAR TF@&return\n");
}

/**
 * Funkce vytvori kod pro nahrani TF return do GF.
 *
 */
void make_TF_RETURN_to_GF() {
    printf("MOVE GF@*return TF@&return\n");
}

/**
 * Funkce vytvori kod pro nahrani GF*return do TF.
 *
 */
void make_MOVE_GF_return_to_TF(char *var){
    printf("MOVE TF@&%s GF@*return\n", var);
}
/** @endcode */