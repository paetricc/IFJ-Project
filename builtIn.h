
#ifndef _BUILTIN_H
#define _BUILTIN_H

#include <stdio.h>
#include <stdbool.h>
#include "Dynamic_string.h"
#include "error.h"
#include "parser.h"
#include "symtable.h"
#include "expression.h"
#include "instruct_dllist.h"

void make_reads();
void make_readi();
void make_readn();
void make_write();
void make_toInteger();
void make_substr();
void make_chr();
void make_ord();
void call_reads();
void call_readi();
void call_readn();
void call_write();//todo
void call_toInteger(double var);
void call_substr(Dynamic_string *str, int var1, int var2);
void call_chr(int var);
void call_ord(Dynamic_string *str, int var);

#endif //_BUILTIN_H
