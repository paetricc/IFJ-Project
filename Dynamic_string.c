
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Dynamic_string.h"

#define DYN_STR_LEN 8

bool DS_Init(Dynamic_string *string){
    if (!(string->str = (char *) malloc(DYN_STR_LEN))){
        return false;
    }
    string->length = 0;
    string->str[string->length] = '\0';
    return true;
}

bool DS_Add(Dynamic_string *string, char c){
    unsigned int n_size = string->length + DYN_STR_LEN;
    if (!(string->str = (char *) realloc(string->str, n_size))){
        return false;
    }
    string->str[string->length] = c;
    string->length++;
    string->str[string->length] = '\0';
    return true;
}

bool DS_Copy(Dynamic_string *string,Dynamic_string *s_new){
    unsigned int s_new_size = string->length;
    if (!(s_new->str = realloc(s_new->str,s_new_size))){
        return false;
    }
    strcpy(s_new->str,string->str);
    s_new->length = s_new_size;
    return true;
}

int DS_Cmp(Dynamic_string *string, const char *c_string){
    return (strcmp(string->str, c_string) == 0);
}

void DS_Free(Dynamic_string *string){
    free(string->str);
}







