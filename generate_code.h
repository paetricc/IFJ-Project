/**
 * Projekt: IFJ2021
 *
 * @brief Implementace generovani kodu a vestavenych funkci.
 *
 * @author Vít Janeček xjanec30@stud.fit.vutbr.cz
 * @author Tony Pham xphamt00@stud.fit.vutbr.cz
*/

#ifndef _GENERATE_CODE_H
#define _GENERATE_CODE_H

#include <stdio.h>
#include <stdbool.h>
#include "Dynamic_string.h"
#include "instruct_dllist.h"

void make_header();

void make_CREATEFRAME();

void make_PUSHFRAME();

void make_POPFRAME();

void make_ADDS();

void make_SUBS();

void make_MULS();

void make_DIVS();

void make_IDIVS();

void make_LTS();

void make_GTS();

void make_EQS();

void make_NOTS();

void make_CALL(char *string);

void make_JUMP_end_fnc(Dynamic_string *string);

void make_JUMP_end_if(int ifCounter);

void make_LABEL_fnc(Dynamic_string *string);

void make_LABEL_else(int ifCounter);

void make_LABEL_endif(int ifCounter);

void make_func_header(Dynamic_string *string);

void make_noVoidReturn(Dynamic_string *string);

void make_DEFVAR_LF(Dynamic_string *string);

void make_DEFVAR_TF(Dynamic_string *string);

void make_MOVE_LF_nil(Dynamic_string *string);

void make_MOVE_TF_nil(char *string);

void make_WRITE_LF(Dynamic_string *string);

void make_WRITE_TF();

void makeWriteCall(Dynamic_string *string);

void make_CREATEFRAME_TMP();

void make_endOfFunc(char *string);

void movePrevious(DLList_Instruct *dlListInstruct);

void moveAfter(DLList_Instruct *dlListInstruct);

void make_POPSandMOVE_tmp1();

void make_POPSandMOVE_tmp2();

void make_INT2FLOATS();

void make_FLOAT2INTS();

void make_POPSandPUSH_float();

void call_NILcompare();

void make_NILcompare();

void make_insertNILcompare();

void make_PUSHS_LF(Dynamic_string *string);

void make_POPS_LF(Dynamic_string *string);

void make_PUSHS_TF(Dynamic_string *string);

void make_PUSHS_int(long long int Integer);

void make_PUSHS_float(double DoubleVal);

void make_PUSHS_string(char *str);

void make_POPS_TF(char *var);

void make_POPS_GF_if();

void make_PUSHS_nil();

void make_CONTAT();

void make_STRLEN();

void make_LT_all(int elseCounter);

void make_LTE_all(int elseCounter);

void make_GT_all(int elseCounter);

void make_GTE_all(int elseCounter);

void make_LT_loop();

void make_LTE_loop();

void make_GT_loop();

void make_GTE_loop();

void make_IF_nil(int elseCounter);

void make_EQ(int elseCounter);

void make_NEQ(int elseCounter);

void make_distribute();

void make_MOVE_TF_to_LF(Dynamic_string *stringTF, Dynamic_string *stringLF);

void make_INT2FLOAT_TF_TF(Dynamic_string *stringTF);

void make_MOVE_TF_INT(Dynamic_string *stringTF , long long int x);

void make_LOOP_TMP(int loopCounter);

void make_LOOP_JUMP(int loopCounter);

void make_MOVE_TF_FLOAT_INT2FLOAT(Dynamic_string *stringTF , long long int x);

void make_MOVE_TF_FLOAT(Dynamic_string *stringTF , double x);

void make_MOVE_TF_STRING(Dynamic_string *stringTF , char *string);

void make_MOVE_RETURN_NIL();

void make_TF_RETURN();

void make_TF_RETURN_to_GF();

void make_MOVE_GF_return_to_TF(char *var);

#endif //_GENERATE_CODE_H

/** @endcode */