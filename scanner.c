#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "scanner.h"

#define STATE_START 99
#define STATE_SUB 100
#define STATE_LCMT2 101
#define STATE_BCMT 102
#define STATE_BCMT2 103
#define STATE_BCMT3 104
#define STATE_DIV 105
#define STATE_NEQ 106
#define STATE_LT 107
#define STATE_GT 108
#define STATE_ASSIGN 109
#define STATE_STR 110
#define STATE_STR2 111
#define STATE_STR3 112
#define STATE_STR4 113
#define STATE_STR5 114
#define STATE_STR6 115
#define STATE_STR7 116
#define STATE_STR8 117
#define STATE_INT0 118
#define STATE_HEX 119
#define STATE_HEX2 120
#define STATE_DHEX 121
#define STATE_DHEX2 122
#define STATE_HEXP 123
#define STATE_HEXP2 124
#define STATE_HEXP3 125
#define STATE_DBL 126
#define STATE_DBL2 127
#define STATE_EXP 128
#define STATE_EXP2 129
#define STATE_EXP3 130
#define STATE_INT 131
#define STATE_ID 132
#define STATE_ERROR 133
#define STATE_INT0 114
#define STATE_HEX 115
#define STATE_HEX2 116
#define STATE_DHEX 117
#define STATE_DHEX2 118
#define STATE_HEXP 119
#define STATE_HEXP2 120
#define STATE_HEXP3 121
#define STATE_DBL 122
#define STATE_DBL2 123
#define STATE_EXP 124
#define STATE_EXP2 125
#define STATE_EXP3 126
#define STATE_INT 127
#define STATE_ID 128
#define STATE_ERROR 129

FILE *source_file;
Dynamic_string *dynamic_string;

void conversion_Int(Dynamic_string *str, Token *token){
    char *ptr;
    int value = (int) strtol(str->str,&ptr,10);
    token->Value.Integer = value;
}

void conversion_Double(Dynamic_string *str, Token *token){
    char *ptr;
    double value = strtod(str->str,&ptr);
    token->Value.Double = value;
}

bool DS_Add_Tester(Dynamic_string *str, char c){
    if (!(DS_Add(str, c))){
        free(str);
        return false;
    }
    return true;
}
void KW_ID_Cmp(Dynamic_string *str, Token *token){
    token->ID = TOKEN_ID_KEYWORD;
    if (DS_Cmp(str, "do")){token->Value.keyword = KEYWORD_DO;}
    else if (DS_Cmp(str, "else")){token->Value.keyword = KEYWORD_ELSE;}
    else if (DS_Cmp(str, "end")){token->Value.keyword = KEYWORD_END;}
    else if (DS_Cmp(str, "function")){token->Value.keyword = KEYWORD_FUNCTION;}
    else if (DS_Cmp(str, "global")){token->Value.keyword = KEYWORD_GLOBAL;}
    else if (DS_Cmp(str, "if")){token->Value.keyword = KEYWORD_IF;}
    else if (DS_Cmp(str, "integer")){token->Value.keyword = KEYWORD_INTEGER;}
    else if (DS_Cmp(str, "local")){token->Value.keyword = KEYWORD_LOCAL;}
    else if (DS_Cmp(str, "nil")){token->Value.keyword = KEYWORD_NIL;}
    else if (DS_Cmp(str, "number")){token->Value.keyword = KEYWORD_NUMBER;}
    else if (DS_Cmp(str, "require")){token->Value.keyword = KEYWORD_REQUIRE;}
    else if (DS_Cmp(str, "return")){token->Value.keyword = KEYWORD_RETURN;}
    else if (DS_Cmp(str, "string")){token->Value.keyword = KEYWORD_STRING;}
    else if (DS_Cmp(str, "then")){token->Value.keyword = KEYWORD_THEN;}
    else if (DS_Cmp(str, "while")){token->Value.keyword = KEYWORD_WHILE;}
    else{
        token->ID = TOKEN_ID_ID;
        if (!(DS_Copy(str, token->Value.string))){
            free(str);
        }
    }
}

int get_token(Token *token){
    source_file = stdin;
    token->Value.string = dynamic_string;

    Dynamic_string string;
    Dynamic_string *ptr_Str = &string;
    if (!(DS_Init(ptr_Str))){
        //error
    }
    char c;
    bool exp_Version = true;
    
int get_token(Token *token){
    source_file = stdin;
    char c;
    int state = STATE_START;
    while ((c = (char) getc(source_file)) != '\0'){
        c = (char) getc(source_file);
        switch (state) {
            case STATE_START:               //co jde ze startu
                switch (c) {
                    case '\n':
                        token->ID = TOKEN_ID_EOL;
                        printf("token EOL\n");
                        state = STATE_START;
                        break;
                    case ',':
                        token->ID = TOKEN_ID_CMA;
                        printf("token CMA\n"); //token CMA
                        state = STATE_START;
                        break;
                    case ' ':
                        token->ID = TOKEN_ID_SPACE;
                        printf("token SPACE\n"); //token SPACE
                        state = STATE_START;
                        break;
                    case ')':
                        token->ID = TOKEN_ID_RBR;
                        printf("token RBR\n"); //token RBR
                        state = STATE_START;
                        break;
                    case '(':
                        token->ID = TOKEN_ID_LBR;
                        printf("token LBR\n"); //token LBR
                        state = STATE_START;
                        break;
                    case '+':
                        token->ID = TOKEN_ID_ADD;
                        printf("token ADD\n"); //token ADD
                        state = STATE_START;
                        break;
                    case '-':
                        state = STATE_SUB;
                        break;
                    case '*':
                        token->ID = TOKEN_ID_MUL;
                        printf("token MUL\n"); //token MUL
                        state = STATE_START;
                        break;
                    case '/':
                        state = STATE_DIV;
                        break;
                    case '~':
                        state = STATE_NEQ;
                        break;
                    case '<':
                        state = STATE_LT;
                        break;
                    case '>':
                        state = STATE_GT;
                        break;
                    case '=':
                        state = STATE_ASSIGN;
                        break;
                    case '"':
                        state = STATE_STR;
                        break;
                    case '0':
                        if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                        state = STATE_INT0;
                        break;
                    case '1' ... '9':
                        if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                        state = STATE_INT;
                        break;
                    case 'A' ... 'Z':
                        if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                        state = STATE_ID;
                        break;
                    case 'a' ... 'z':
                        if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                        state = STATE_ID;
                        break;
                    case '_':
                        if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}

                        state = STATE_INT0;
                        break;
                    case '1' ... '9':
                        state = STATE_INT;
                        break;
                    case 'A' ... 'Z':
                        state = STATE_ID;
                        break;
                    case 'a' ... 'z':
                        state = STATE_ID;
                        break;
                    case '_':
                        state = STATE_ID;
                        break;
                    case '\0':
                        printf("token EOF\n"); //token EOF
                        break;
                    default:
                        state = STATE_ERROR;
                        break;
                }
                break;
            case STATE_SUB:
                if (c == '-') {
                    state = STATE_LCMT2;
                } else {
                    token->ID = TOKEN_ID_SUB;
                    printf("token SUB\n"); //token SUB
                    state = STATE_START;
                    ungetc(c, source_file);
                }
                break;
            case STATE_LCMT2:
                if (c == '[') {
                    state = STATE_BCMT;
                } else if (c == '\n') {
                    printf("token LCMT2\n"); //token LCMT2;
                    token->ID = TOKEN_ID_EOL;
                    printf("token EOL\n"); //token EOL
                    state = STATE_START;
                } else {
                    state = STATE_LCMT2;
                }
                break;
            case STATE_BCMT:
                if (c == '[') {
                    state = STATE_BCMT2;
                } else {
                    state = STATE_ERROR;
                }
                break;
            case STATE_BCMT2:
                if (c == ']') {
                    state = STATE_BCMT3;
                } else if (c == '[') {
                    state = STATE_ERROR;
                } else {
                    state = STATE_BCMT2;
                }
                break;
            case STATE_BCMT3:
                if (c == ']') {
                    printf("token BCMT4\n"); //token BCMT4
                    state = STATE_START;
                } else {
                    state = STATE_ERROR;
                }
                break;
            case STATE_DIV:
                if (c == '/') {
                    token->ID = TOKEN_ID_DIV2;
                    printf("token DIV2\n"); //token DIV2
                    state = STATE_START;
                } else {
                    token->ID = TOKEN_ID_DIV;
                    printf("token DIV\n"); //token DIV
                    state = STATE_START;
                    ungetc(c, source_file);
                }
                break;
            case STATE_NEQ:
                if (c == '=') {
                    token->ID = TOKEN_ID_NEQ2;
                    printf("token NEQ2\n"); //token NEQ2
                    state = STATE_START;
                } else {
                    state = STATE_ERROR;
                }
                break;
            case STATE_LT:
                if (c == '=') {
                    token->ID = TOKEN_ID_LTE;
                    printf("token LTE\n"); //token LTE
                    state = STATE_START;
                } else {
                    token->ID = TOKEN_ID_LT;
                    printf("token LT\n"); //token LT
                    state = STATE_START;
                    ungetc(c, source_file);
                }
                break;
            case STATE_GT:
                if (c == '=') {
                    token->ID = TOKEN_ID_GTE;
                    printf("token GTE\n"); //token GTE
                    state = STATE_START;
                } else {
                    token->ID = TOKEN_ID_GT;
                    printf("token GT\n"); //token GT
                    state = STATE_START;
                    ungetc(c, source_file);
                }
                break;
            case STATE_ASSIGN:
                if (c == '=') {
                    token->ID = TOKEN_ID_EQ;
                    printf("token EQ\n"); //token EQ
                    state = STATE_START;
                } else {
                    token->ID = TOKEN_ID_ASSIGN;
                    printf("token ASSIGN\n"); //token ASSIGN
                    state = STATE_START;
                    ungetc(c, source_file);
                }
                break;
            case STATE_STR:
                if (c == '"') {
                    token->ID = TOKEN_ID_FSTR;
                    if (!(DS_Copy(ptr_Str, token->Value.string))){
                        //error
                    }
                    free(ptr_Str);
                    printf("token FSTR\n");
                    state = STATE_START;
                } else if (c == '\\'){
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_STR2;
                } else if ((c >= 32 && c != '"') || (c >= 32 && c != '\\')){
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    //token STR5
                    printf("token STR5\n");
                    state = STATE_START;
                } else if (c == '\\'){
                    state = STATE_STR2;
                } else if ((c >= 32 && c != '"') || (c >= 32 && c != '\\')){
                    state = STATE_STR;
                } else{
                    state = STATE_ERROR;
                }
                break;
            case STATE_STR2:
                if (c == '0' || c == '1') {
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_STR3;
                } else if (c == '2'){
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_STR6;
                } else if (c == '\\' || c == '"' || c == 'n' || c == 't'){
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/ }
                if (c >= '0' && c <='2') {
                    state = STATE_STR3;
                } else if (c == '\\' || c == '"' || c == 'n' || c == 't'){
                    state = STATE_STR;
                } else {
                    state = STATE_ERROR;
                }
                break;
            case STATE_STR3:
                if (c == '0') {
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_STR4;
                }else if (c >= '1' && c <= '9') {
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_STR5;
                if (c >= '0' && c <='5') {
                    state = STATE_STR4;
                } else {
                    state = STATE_ERROR;
                }
                break;
            case STATE_STR4:
                if (c >= '1' && c <='9') {
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_STR;
                } else {
                    state = STATE_ERROR;
                }
                break;
            case STATE_STR5:
                if (c >= '0' && c <='9') {
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_STR;
                } else {
                    state = STATE_ERROR;
                }
                break;
            case STATE_STR6:
                if (c >= '0' && c <='4') {
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_STR7;
                }else if (c == '5') {
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_STR8;
                } else {
                    state = STATE_ERROR;
                }
                break;
            case STATE_STR7:
                if (c >= '0' && c <='9') {
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_STR;
                } else {
                    state = STATE_ERROR;
                }
                break;
            case STATE_STR8:
                if (c >= '0' && c <='5') {
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                if (c >= '1' && c <='5') {
                    state = STATE_STR;
                } else {
                    state = STATE_ERROR;
                }
                break;
            case STATE_INT0:
                if (c == 'x') {
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_HEX;
                }else if (c == '0') {
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_INT0;
                }else if (c >= '1' && c <= '9'){
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_INT;
                }else if (c == 'e' || c == 'E'){
                    exp_Version = true;
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_EXP;
                } else if (c == '.'){
                    exp_Version = false;
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_DBL;
                }else{
                    conversion_Int(ptr_Str,token);
                    token->ID = TOKEN_ID_INT0;
                    free(ptr_Str);
                    state = STATE_HEX;
                }else if (c == '0') {
                    state = STATE_INT0;
                }else if (c >= '1' && c <= '9'){
                    state = STATE_INT;
                }else if (c == 'e' || c == 'E'){
                    state = STATE_EXP;
                } else if (c == '.'){
                    state = STATE_DBL;
                }else{
                    printf("token INT0\n"); //token INT0
                    state = STATE_START;
                }
                break;
            case STATE_HEX:
                if ((c >= 'a' && c <='f') || (c >= 'A' && c <='F') || (c >= '0' && c <='9')) {
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_HEX2;
                } else{
                    state = STATE_ERROR;
                }
                break;
            case STATE_HEX2:
                if ((c >= 'a' && c <='f') || (c >= 'A' && c <='F') || (c >= '0' && c <='9')) {
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_HEX2;
                } else if (c == '.'){
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_DHEX;
                } else{
                    conversion_Int(ptr_Str,token);
                    token->ID = TOKEN_ID_HEX2;
                    free(ptr_Str);
                    state = STATE_HEX2;
                } else if (c == '.'){
                    state = STATE_DHEX;
                } else{
                    printf("token HEX2\n"); //token HEX2
                    state = STATE_START;
                    ungetc(c, source_file);
                }
                break;
            case STATE_DHEX:
                if ((c >= 'a' && c <='f') || (c >= 'A' && c <='F') || (c >= '0' && c <='9')) {
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_DHEX2;
                } else if (c == 'p'){
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_HEXP;
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_DHEX2;
                } else if (c == 'p'){
                    state = STATE_HEXP;
                } else{
                    state = STATE_ERROR;
                }
                break;
            case STATE_DHEX2: //TODO
            case STATE_DHEX2:
                if ((c >= 'a' && c <='f') || (c >= 'A' && c <='F') || (c >= '0' && c <='9')) {
                    state = STATE_DHEX2;
                } else if (c == 'p'){
                    state = STATE_HEXP;
                } else{
                    printf("token DHEX2\n"); //token DHEX2
                    state = STATE_START;
                    ungetc(c, source_file);
                }
                break;
            case STATE_HEXP:
                if (c == '+' || c == '-'){
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_HEXP2;
                } else{
                    state = STATE_ERROR;
                }
                break;
            case STATE_HEXP2:
                if (c >= '0' && c <='9'){
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_HEXP3;
                } else{
                    state = STATE_ERROR;
                }
                break;
            case STATE_HEXP3:
                if (c >= '1' && c <='9'){
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_HEXP3;
                } else{
                    conversion_Double(ptr_Str,token);
                    token->ID = TOKEN_ID_HEXP3;
                    free(ptr_Str);
                    state = STATE_HEXP3;
                } else{
                    printf("token HEXP3\n"); //token HEXP3
                    state = STATE_START;
                    ungetc(c, source_file);
                }
                break;
            case STATE_EXP:
                if (c >= '0' && c <='9'){
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_EXP3;
                } else if (c == '+' || c == '-'){
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_EXP3;
                } else if (c == '+' || c == '-'){
                    state = STATE_EXP2;
                } else{
                    state = STATE_ERROR;
                }
                break;
            case STATE_EXP2:
                if (c >= '0' && c <='9'){
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_EXP3;
                } else{
                    state = STATE_ERROR;
                }
                break;
            case STATE_EXP3:
                if (c >= '0' && c <='9'){
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_EXP3;
                } else{
                    if (exp_Version){
                        conversion_Int(ptr_Str,token);
                        token->ID = TOKEN_ID_EXP3;
                        free(ptr_Str);
                    } else{
                        conversion_Double(ptr_Str,token);
                        token->ID = TOKEN_ID_EXP3;
                        free(ptr_Str);
                    }
                    state = STATE_EXP3;
                } else{
                    printf("token EXP3\n"); //token EXP3
                    state = STATE_START;
                    ungetc(c, source_file);
                }
                break;
            case STATE_DBL:
                if (c >= '0' && c <='9'){
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_DBL2;
                } else{
                    state = STATE_ERROR;
                }
                break;
            case STATE_DBL2:
                if (c == 'e' || c =='E') {
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_EXP;
                } else if (c >= '0' && c <= '9'){
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_DBL2;
                } else{
                    conversion_Double(ptr_Str,token);
                    token->ID = TOKEN_ID_DBL2;
                    free(ptr_Str);
                    state = STATE_EXP;
                } else if (c >= '0' && c <= '9'){
                        state = STATE_DBL2;
                } else{
                    printf("token DBL2\n"); //token DBL2
                    state = STATE_START;
                    ungetc(c, source_file);
                }
                break;
            case STATE_INT:
                if (c == 'e' || c =='E'){
                    exp_Version = true;
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_EXP;
                } else if (c == '.'){
                    exp_Version = false;
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_DBL;
                }else if (c >= '0' && c <='9'){
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_INT;
                } else{
                    conversion_Int(ptr_Str,token);
                    token->ID = TOKEN_ID_INT;
                    free(ptr_Str);
                    state = STATE_EXP;
                } else if (c == '.'){
                    state = STATE_DBL;
                }else if (c >= '0' && c <='9'){
                    state = STATE_INT;
                } else{
                    printf("token INT\n"); //token INT
                    state = STATE_START;
                    ungetc(c, source_file);
                }
                break;
            case STATE_ID:
                if ((c >= 'a' && c <='z') || (c >= 'A' && c <='Z') || (c >= '0' && c <='9') || c =='_'){
                    if (!(DS_Add_Tester(ptr_Str, c))){/**error**/}
                    state = STATE_ID;
                } else{
                    KW_ID_Cmp(ptr_Str, token);
                    free(ptr_Str);
                    state = STATE_ID;
                } else{
                    printf("token ID\n"); //token ID
                    state = STATE_START;
                    ungetc(c, source_file);
                }
                break;
            default:
                state = STATE_ERROR;
                break;
        }
        printf("%d\n", state);
        if (state == STATE_ERROR){
            break;
        }
    }
    token->ID = TOKEN_ID_EOF;
    printf("token EOF\n"); //token EOF
}