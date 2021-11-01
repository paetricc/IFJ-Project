#ifndef _DYNAMIC_STRING_H
#define _DYNAMIC_STRING_H

#include <stdbool.h>

typedef struct {
    char *str;
    unsigned int length;
} Dynamic_string;


bool DS_Init(Dynamic_string *string);

bool DS_Add(Dynamic_string *string, char c);

bool DS_Copy(Dynamic_string *string,Dynamic_string *s_new);

int DS_Cmp(Dynamic_string *string, const char *c_string);

void DS_Free(Dynamic_string *string);

#endif