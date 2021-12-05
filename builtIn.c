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

/**
 * Funkce vytvori kod pro vestavenou funkci reads.
 *
 */
void make_reads(){
    printf("#builtIn reads()\n");
    printf("JUMP $end_fnc_reads\n");
    printf("LABEL $fnc_reads\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@*return\n");
    printf("MOVE LF@*return nil@nil\n");
    printf("READ LF@*return string\n");
    printf("DEFVAR LF@_vartype\n");
    printf("TYPE LF@_vartype LF@*return\n");
    printf("JUMPIFEQ $isString LF@_vartype string@string\n");
    printf("MOVE LF@*return nil@nil\n");
    printf("LABEL $isString\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $end_fnc_reads\n");
}

/**
 * Funkce vytvori kod pro vestavenou funkci readi.
 *
 */
void make_readi(){
    printf("#builtIn readi()\n");
    printf("JUMP $end_fnc_readi\n");
    printf("LABEL $fnc_readi\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@*return\n");
    printf("MOVE LF@*return nil@nil\n");
    printf("READ LF@*return int\n");
    printf("DEFVAR LF@_vartype\n");
    printf("TYPE LF@_vartype LF@*return\n");
    printf("JUMPIFEQ $isInt LF@_vartype string@int\n");
    printf("MOVE LF@*return nil@nil\n");
    printf("LABEL $isInt\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $end_fnc_readi\n");
}

/**
 * Funkce vytvori kod pro vestavenou funkci readn.
 *
 */
void make_readn(){
    printf("#builtIn readn()\n");
    printf("JUMP $end_fnc_readn\n");
    printf("LABEL $fnc_readn\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@*return\n");
    printf("MOVE LF@*return nil@nil\n");
    printf("READ LF@*return float\n");
    printf("DEFVAR LF@_vartype\n");
    printf("TYPE LF@_vartype LF@*return\n");
    printf("JUMPIFEQ $isFloat LF@_vartype string@float\n");
    printf("MOVE LF@*return nil@nil\n");
    printf("LABEL $isFloat\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $end_fnc_readn\n");
}

/**
 * Funkce vytvori kod pro vestavenou funkci write.
 *
 */
void make_write(){
    printf("#builtIn write()\n");
    printf("JUMP $end_fnc_write\n");
    printf("LABEL $fnc_write\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@_var\n");
    printf("MOVE LF@_var LF@_cvar\n");
    printf("DEFVAR LF@_vartype\n");
    printf("TYPE LF@_vartype LF@_var\n");
    printf("JUMPIFEQ $Continue LF@_vartype string@nil\n");
    printf("JUMPIFEQ $isString LF@_vartype string@string\n");
    printf("JUMPIFEQ $isInt LF@_vartype string@int\n");
    printf("JUMPIFEQ $isFloat LF@_vartype string@float\n");
    printf("LABEL $isString\n");
    printf("WRITE LF@_var\n");
    printf("JUMP $Continue\n");
    printf("LABEL $isInt\n");
    printf("WRITE LF@_var\n");
    printf("JUMP $Continue\n");
    printf("LABEL $isFloat\n");
    printf("WRITE LF@_var\n");
    printf("LABEL $Continue\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $end_fnc_write\n");
}

/**
 * Funkce vytvori kod pro vestavenou funkci toInteger.
 *
 */
void make_toInteger(){
    printf("#builtIn toInteger()\n");
    printf("JUMP $end_fnc_toInteger\n");
    printf("LABEL $fnc_toInteger\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@*return\n");
    printf("MOVE LF@*return nil@nil\n");
    printf("DEFVAR LF@_var1\n");
    printf("MOVE LF@_var1 LF@_cvar1\n");
    printf("FLOAT2INT LF@*return LF@_var1\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $end_fnc_chr\n");
}

/**
 * Funkce vytvori kod pro vestavenou funkci substr.
 *
 */
void make_substr(){
    printf("#builtIn substr()\n");
    printf("JUMP $end_fnc_substr\n");
    printf("LABEL $fnc_substr\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@*return\n");
    printf("MOVE LF@*return string@\n");
    printf("DEFVAR LF@_var1\n");
    printf("MOVE LF@_var1 LF@_cvar1\n");
    printf("DEFVAR LF@_var2\n");
    printf("MOVE LF@_var2 LF@_cvar2\n");
    printf("DEFVAR LF@_var3\n");
    printf("MOVE LF@_var3 LF@_cvar3\n");
    printf("DEFVAR LF@$_tmp\n");
    printf("SUB LF@_var2 LF@_var2 int@1\n");
    printf("LABEL $while_substr\n");
    printf("LT LF@$_tmp LF@_var2 LF@_var3\n");
    printf("JUMPIFNEQ $end_while_substr LF@$_tmp bool@true\n");
    printf("GETCHAR LF@$_tmp LF@_var1 LF@_var2\n");
    printf("CONCAT LF@*return LF@*return LF@$_tmp\n");
    printf("ADD LF@_var2 LF@_var2 int@1\n");
    printf("JUMP $while_substr\n");
    printf("LABEL $end_while_substr\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $end_fnc_substr\n");
    
    /**
    printf("#builtIn substr()\n");
    printf("JUMP $end_fnc_substr\n");
    printf("LABEL $fnc_substr\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@*return\n");
    printf("MOVE LF@*return string@\n");
    printf("DEFVAR LF@_var1\n");
    printf("MOVE LF@_var1 LF@_cvar1\n");
    printf("DEFVAR LF@_var2\n");
    printf("MOVE LF@_var2 LF@_cvar2\n");
    printf("DEFVAR LF@_var3\n");
    printf("MOVE LF@_var3 LF@_cvar3\n");
    printf("DEFVAR LF@tmp\n");
    //test jestli var3 < var2, vrací funkce prázdný řetězec
    printf("LT LF@$tmp LF@_var3 LF@_var2\n");
    printf("JUMPIFEQ $end_while_substr LF@tmp bool@true\n");

    //jestli jsou v intervalu od 0 dál
    printf("GT LF@$tmp LF@_var3 int@0\n");
    printf("JUMPIFEQ $end_while_substr LF@tmp bool@true\n");

    printf("GT LF@$tmp LF@_var2 int@0\n");
    printf("JUMPIFEQ $end_while_substr LF@tmp bool@true\n");

    //jestli jsou od intervalu od delky stringu míň TODO počítá se do délky i odřádkování? otestovat
    printf("DEFVAR LF@_strLen\n");
    printf("STRLEN LF@_strLen LF@_strLen\n");

    printf("LT LF@$tmp LF@_var3 LF@_strLen\n");
    printf("JUMPIFEQ $end_while_substr LF@tmp bool@true\n");

    printf("LT LF@$tmp LF@_var2 LF@_strLen");
    printf("JUMPIFEQ $end_while_substr LF@tmp bool@true\n");

    //TODO zkontrolovat tuto část
    printf("LABEL $while_substr\n");
    printf("EQ LF@$tmp LF@_var2 LF@_var3\n");
    printf("JUMPIFEQ $end_while_substr LF@tmp bool@true\n");
    printf("GETCHAR LF@$tmp LF@_var1 LF@_var2\n");
    printf("CONCAT LF@*return LF@*return LF@$tmp\n");
    printf("ADD LF@_var2 LF@_var2 int@1\n");
    printf("JUMP $while_substr\n");
    printf("LABEL $end_while_substr\n");

    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $end_fnc_substr\n");**/
}

/**
 * Funkce vytvori kod pro vestavenou funkci chr.
 *
 */
void make_chr(){
    printf("#builtIn chr()\n");
    printf("JUMP $end_fnc_chr\n");
    printf("LABEL $fnc_chr\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@*return\n");
    printf("MOVE LF@*return nil@nil\n");
    printf("DEFVAR LF@_var1\n");
    printf("MOVE LF@_var1 LF@_cvar1\n");
    printf("DEFVAR LF@_vartmp\n");
    printf("LT LF@_vartmp LF@_var1 int@0\n");
    printf("JUMPIFEQ $outOfRange LF@_vartmp bool@true\n");
    printf("GT LF@_vartmp LF@_var1 int@255\n");
    printf("JUMPIFEQ $outOfRange LF@_vartmp bool@true\n");
    printf("INT2CHAR LF@*return LF@_var1\n");
    printf("LABEL $outOfRange\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $end_fnc_chr\n");
}

/**
 * Funkce vytvori kod pro vestavenou funkci ord.
 *
 */
void make_ord(){
    printf("#builtIn ord()\n");
    printf("JUMP $end_fnc_ord\n");
    printf("LABEL $fnc_ord\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@*return\n");
    printf("MOVE LF@*return nil@nil\n");
    printf("DEFVAR LF@_var1\n");
    printf("MOVE LF@_var1 LF@_cvar1\n");
    printf("DEFVAR LF@_var2\n");
    printf("MOVE LF@_var2 LF@_cvar2\n");
    printf("SUB LF@_var2 LF@_var2 int@1");
    printf("DEFVAR LF@_vartmp");
    printf("LT LF@_vartmp LF@_var2 int@0");
    printf("JUMPIFEQ $outOfRange LF@_vartmp bool@true");
    printf("DEFVAR LF@_var3");
    printf("STRLEN LF@_var3 LF@_var1");
    printf("GT LF@_vartmp LF@_var2 LF@_var3");
    printf("JUMPIFEQ $outOfRange LF@_vartmp bool@true");
    printf("STRI2INT LF@*return LF@_var1 LF@_var2");
    printf("LABEL $outOfRange");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $end_fnc_ord\n");
}

/**
 * Funkce vytvori kod pro volani kodu pro vestavenou funkci reads.
 *
 */
void call_reads(){
    printf("CREATEFRAME\n");
    printf("CALL $fnc_reads\n");
}

/**
 * Funkce vytvori kod pro volani kodu pro vestavenou funkci readi.
 *
 */
void call_readi(){
    printf("CREATEFRAME\n");
    printf("CALL $fnc_readi\n");
}

/**
 * Funkce vytvori kod pro volani kodu pro vestavenou funkci readn.
 *
 */
void call_readn(){
    printf("CREATEFRAME\n");
    printf("CALL $fnc_readn\n");
}

//void make_write

/**
 * Funkce vytvori kod pro volani kodu pro vestavenou funkci toInteger.
 *
 */
void call_toInteger(double var){
    printf("CREATEFRAME\n");
    printf("DEFVAR TF@_cvar1\n");
    printf("MOVE TF@_cvar1 float@%a\n", var);
    printf("CALL $toInteger\n");
}

/**
 * Funkce vytvori kod pro volani kodu pro vestavenou funkci substr.
 *
 */
void call_substr(Dynamic_string *str, int var1, int var2){
    printf("CREATEFRAME\n");
    printf("DEFVAR TF@_cvar1\n");
    printf("MOVE TF@_cvar1 %s\n", str->str);
    printf("DEFVAR TF@_cvar2\n");
    printf("MOVE TF@_cvar2 int@%d\n", var1);
    printf("DEFVAR TF@_cvar3\n");
    printf("MOVE TF@_cvar3 int@%d\n", var2);
    printf("CALL $substr\n");
}

/**
 * Funkce vytvori kod pro volani kodu pro vestavenou funkci chr.
 *
 * @param var
 */
void call_chr(int var){
    printf("CREATEFRAME\n");
    printf("DEFVAR TF@_cvar1\n");
    printf("MOVE TF@_cvar1 int@%d\n", var);
    printf("CALL $fnc_chr\n");
}

/**
 * Funkce vytvori kod pro volani kodu pro vestavenou funkci ord.
 *
 * @param str ukazatel na DS se stringem
 * @param var
 */
void call_ord(Dynamic_string *str, int var){
    printf("CREATEFRAME\n");
    printf("DEFVAR TF@_cvar1\n");
    printf("MOVE TF@_cvar1 %s\n", str->str);
    printf("DEFVAR TF@_cvar1\n");
    printf("MOVE TF@_cvar2 int@%d\n", var);
    printf("CALL $fnc_chr\n");
}