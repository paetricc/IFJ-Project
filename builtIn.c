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
    printf("#---------- builtIn reads() ----------\n");
    printf("JUMP $end_reads\n");
    printf("LABEL $reads\n");
    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");
    printf("READ GF@*return string\n");
    printf("DEFVAR TF@&vartype\n");
    printf("TYPE TF@&vartype GF@*return\n");
    printf("JUMPIFEQ $isStringREADS TF@&vartype string@string\n");
    printf("MOVE GF@*return nil@nil\n");
    printf("LABEL $isStringREADS\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $end_reads\n");
    printf("#------------------------------\n\n");
}

/**
 * Funkce vytvori kod pro vestavenou funkci readi.
 *
 */
void make_readi(){
    printf("#---------- builtIn readi() ----------\n");
    printf("JUMP $end_readi\n");
    printf("LABEL $readi\n");
    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");
    printf("READ GF@*return int\n");
    printf("DEFVAR TF@&vartype\n");
    printf("TYPE TF@&vartype GF@*return\n");
    printf("JUMPIFEQ $isIntREADI TF@&vartype string@int\n");
    printf("MOVE GF@*return nil@nil\n");
    printf("LABEL $isIntREADI\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $end_readi\n");
    printf("#------------------------------\n\n");
}

/**
 * Funkce vytvori kod pro vestavenou funkci readn.
 *
 */
void make_readn(){
    printf("#---------- builtIn readn() ----------\n");
    printf("JUMP $end_readn\n");
    printf("LABEL $readn\n");
    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");
    printf("READ GF@*return float\n");
    printf("DEFVAR TF@&vartype\n");
    printf("TYPE TF@&vartype GF@*return\n");
    printf("JUMPIFEQ $isFloatREADN TF@&vartype string@float\n");
    printf("MOVE GF@*return nil@nil\n");
    printf("LABEL $isFloatREADN\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $end_readn\n");
    printf("#------------------------------\n\n");
}

/**
 * Funkce vytvori kod pro vestavenou funkci write.
 *
 */
void make_write(){
    printf("#---------- builtIn write() ----------\n");
    printf("JUMP $end_fnc_write\n");
    printf("LABEL $fnc_write\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@_var\n");
    printf("MOVE LF@_var LF@_cvar\n");
    printf("DEFVAR LF@_vartype\n");
    printf("TYPE LF@_vartype LF@_var\n");
    printf("JUMPIFEQ $ContinueWrite LF@_vartype string@nil\n");
    printf("JUMPIFEQ $isStringWrite LF@_vartype string@string\n");
    printf("JUMPIFEQ $isIntWrite LF@_vartype string@int\n");
    printf("JUMPIFEQ $isFloatWrite LF@_vartype string@float\n");
    printf("LABEL $isStringWrite\n");
    printf("WRITE LF@_var\n");
    printf("JUMP $ContinueWrite\n");
    printf("LABEL $isIntWrite\n");
    printf("WRITE LF@_var\n");
    printf("JUMP $ContinueWrite\n");
    printf("LABEL $isFloatWrite\n");
    printf("WRITE LF@_var\n");
    printf("LABEL $ContinueWrite\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $end_fnc_write\n");
    printf("#------------------------------\n\n");
}

/**
 * Funkce vytvori kod pro vestavenou funkci toInteger.
 *
 */
void make_toInteger(){
    printf("#---------- builtIn tointeger() ----------\n");
    printf("JUMP $end_tointeger\n");
    printf("LABEL $tointeger\n");
    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");
    printf("DEFVAR TF@&f\n");
    printf("MOVE TF@&f LF@&f\n");
    printf("FLOAT2INT GF@*return TF@&f\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $end_tointeger\n");
    printf("#------------------------------\n\n");
}

/**
 * Funkce vytvori kod pro vestavenou funkci substr.
 *
 */
void make_substr(){
    printf("#---------- builtIn substr() ----------\n");
    printf("JUMP $end_substr\n");
    printf("LABEL $substr\n");
    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");
    printf("MOVE GF@*return string@\n");
    printf("DEFVAR TF@&s\n");
    printf("MOVE TF@&s LF@&s\n");
    printf("DEFVAR TF@&i\n");
    printf("MOVE TF@&i LF@&i\n");
    printf("DEFVAR TF@&j\n");
    printf("MOVE TF@&j LF@&j\n");
    printf("DEFVAR TF@&tmp\n");
    printf("LT TF@&tmp TF@&j TF@&i\n");
    printf("JUMPIFEQ $end_while_substr TF@&tmp bool@true\n");
    printf("GT TF@&tmp TF@&i float@%a\n", 0.0);
    printf("JUMPIFNEQ $end_while_substr TF@&tmp bool@true\n");
    printf("GT TF@&tmp TF@&j float@%a\n", 0.0);
    printf("JUMPIFNEQ $end_while_substr TF@&tmp bool@true\n");
    printf("DEFVAR TF@&strlen\n");
    printf("STRLEN TF@&strlen TF@&s\n");
    printf("ADD TF@&strlen TF@&strlen int@1\n");
    printf("INT2FLOAT TF@&strlen TF@&strlen\n");
    printf("LT TF@&tmp TF@&i TF@&strlen\n");
    printf("JUMPIFNEQ $end_while_substr TF@&tmp bool@true\n");
    printf("LT TF@&tmp TF@&j TF@&strlen\n");
    printf("JUMPIFNEQ $end_while_substr TF@&tmp bool@true\n");
    printf("SUB TF@&i TF@&i float@%a\n", 1.0);
    printf("LABEL $while_substr\n");
    printf("LT TF@&tmp TF@&i TF@&j\n");
    printf("JUMPIFNEQ $end_while_substr TF@&tmp bool@true\n");
    printf("FLOAT2INT TF@&i TF@&i\n");
    printf("GETCHAR TF@&tmp TF@&s TF@&i\n");
    printf("INT2FLOAT TF@&i TF@&i\n");
    printf("CONCAT GF@*return GF@*return TF@&tmp\n");
    printf("ADD TF@&i TF@&i float@%a\n", 1.0);
    printf("JUMP $while_substr\n");
    printf("LABEL $end_while_substr\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $end_substr\n");
    printf("#------------------------------\n\n");
}

/**
 * Funkce vytvori kod pro vestavenou funkci chr.
 *
 */
void make_chr(){
    printf("#---------- builtIn chr() ----------\n");
    printf("JUMP $end_chr\n");
    printf("LABEL $chr\n");
    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");
    printf("DEFVAR TF@&i\n");
    printf("MOVE TF@&i LF@&i\n");
    printf("DEFVAR TF@&vartmp\n");
    printf("LT TF@&vartmp TF@&i int@0\n");
    printf("JUMPIFEQ $outOfRangeCHR TF@&vartmp bool@true\n");
    printf("GT TF@&vartmp TF@&i int@255\n");
    printf("JUMPIFEQ $outOfRangeCHR TF@&vartmp bool@true\n");
    printf("INT2CHAR GF@*return TF@&i\n");
    printf("LABEL $outOfRangeCHR\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $end_chr\n");
    printf("#------------------------------\n\n");
}

/**
 * Funkce vytvori kod pro vestavenou funkci ord.
 *
 */
void make_ord(){
    printf("#---------- builtIn chr() ----------\n");
    printf("JUMP $end_ord\n");
    printf("LABEL $ord\n");
    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");
    printf("DEFVAR TF@&s\n");
    printf("MOVE TF@&s LF@&s\n");
    printf("DEFVAR TF@&i\n");
    printf("MOVE TF@&i LF@&i\n");
    printf("SUB TF@&i TF@&i int@1\n");
    printf("DEFVAR TF@&vartmp\n");
    printf("LT TF@&vartmp TF@&i int@0\n");
    printf("JUMPIFEQ $outOfRangeORD TF@&vartmp bool@true\n");
    printf("DEFVAR TF@&var3\n");
    printf("STRLEN TF@&var3 TF@&s\n");
    printf("SUB TF@&var3 TF@&var3 int@1\n");
    printf("GT TF@&vartmp TF@&i TF@&var3\n");
    printf("JUMPIFEQ $outOfRangeORD TF@&vartmp bool@true\n");
    printf("STRI2INT GF@*return TF@&s TF@&i\n");
    printf("LABEL $outOfRangeORD\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $end_ord\n");
    printf("#------------------------------\n\n");
}

/**
 * Funkce vytvori kod pro volani kodu pro vestavenou funkci write.
 *
 */
void call_write(char *str){
    printf("CREATEFRAME\n");
    printf("DEFVAR TF@_cvar\n");
    printf("MOVE TF@_cvar string@%s\n", str);
    printf("CALL $fnc_write\n");
}