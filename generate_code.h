

#ifndef _GENERATE_CODE_H
#define _GENERATE_CODE_H

#include <stdio.h>
#include <stdbool.h>
#include "Dynamic_string.h"
#include "instruct_dllist.h"

void make_header();

void make_func_header(Dynamic_string *string);

void make_noVoidReturn(Dynamic_string *string);

void make_DEFVAR(Dynamic_string *string);

void make_CREATEFRAME_TMP();

void make_endOfFunc(Dynamic_string *string);

#endif //_GENERATE_CODE_H
