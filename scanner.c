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
#define STATE_ZERO 133
#define STATE_ERROR 134

//FILE *source_file;
Dynamic_string *dynamic_string;

void conversion_Int(Dynamic_string *str, Token *token){
    char *ptr;
    int value = (token->ID == TOKEN_ID_INT || token->ID == TOKEN_ID_INT0) ? 
    strtol(str->str,&ptr,10) : strtol(str->str,&ptr,16);
    token->Value.Integer = value;
}

void conversion_Double(Dynamic_string *str, Token *token){
    token->Value.Double = atof(str->str);
}

bool DS_Add_Tester(Dynamic_string *str, char c) {
    if (!(DS_Add(str, c))) {
        free(str);
        return false;
    }
    return true;
}

void HexaDecimal(char *name, Token *res) {
    int point = 0;
    while(name[point++] != '.');
    int WholeNumberPart = point - 3;
    int DecimalPart = strlen(name) - point;
    double exp = 1.0f;
    double tmp = 0;
    while(WholeNumberPart-- != 1) exp = exp * 16;
    for (int i = 2; name[i] != '\0'; i++) {
        if(name[i] == '.') continue; 
        int shift  = 0;
        if ( name[i] >= 65 && name[i] <= 70 ) {
            shift = name[i] - 55;
        } else if ( name[i] >= 97  && name[i] <= 102 ) {
            shift = name[i] - 87;
        } else {
            shift = name[i] - 48;
        }
        if (shift) tmp += shift*exp;
        exp /= 16;
    }
    (*res).Value.Double = tmp;
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

Token get_token(Token *token, FILE *source_file ) {
    Dynamic_string *ptr_Str = (Dynamic_string *)malloc(sizeof(Dynamic_string));
    token->Value.string = (Dynamic_string *)malloc(sizeof(Dynamic_string));
    if (!(DS_Init(ptr_Str))) {
        //error
    }
        char c;
        int state = STATE_START;
        while (((c = (char) getc(source_file)) != '\0') || (state == STATE_STR)) {
            switch (state) {
                case STATE_START:               //co jde ze startu
                    switch (c) {
                        case 9:
                            token->ID = TOKEN_ID_TAB;
                            return *token;
                            break;
                        case ':':
                            token->ID = TOKEN_ID_CLN;
                            return *token;
                            break;
                        case '\n':
                            token->ID = TOKEN_ID_EOL;
                            return *token;
                            break;
                        case ',':
                            token->ID = TOKEN_ID_CMA;
                            return *token;
                            break;
                        case ' ':
                            token->ID = TOKEN_ID_SPACE;
                            return *token;
                            break;
                        case ')':
                            token->ID = TOKEN_ID_RBR;
                            return *token;
                            break;
                        case '(':
                            token->ID = TOKEN_ID_LBR;
                            return *token;
                            break;
                        case '+':
                            token->ID = TOKEN_ID_ADD;
                            return *token;
                            break;
                        case '-':
                            state = STATE_SUB;
                            break;
                        case '*':
                            token->ID = TOKEN_ID_MUL;
                            return *token;
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
                            if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                            state = STATE_STR;
                            break;
                        case '0':
                            if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                            state = STATE_ZERO;
                            break;
                        case '1' ... '9':
                            if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                            state = STATE_INT;
                            break;
                        case 'A' ... 'Z':
                            if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                            state = STATE_ID;
                            break;
                        case 'a' ... 'z':
                            if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                            state = STATE_ID;
                            break;
                        case '_':
                            if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                            state = STATE_ID;
                            break;
                        case '\0':
                            printf("token EOF\n"); //TODO checknout
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
                        ungetc(c, source_file);
                        return *token;
                    }
                    break;
                case STATE_LCMT2:
                    if (c == '[') {
                        state = STATE_BCMT;
                    } else if (c == '\n') {
                        token->ID = TOKEN_ID_LCMT2;
                        ungetc(c, source_file);
                        return *token;
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
                        token->ID = TOKEN_ID_BCMT4;
                        return *token;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_DIV:
                    if (c == '/') {
                        token->ID = TOKEN_ID_DIV2;
                        return *token;
                    } else {
                        token->ID = TOKEN_ID_DIV;
                        ungetc(c, source_file);
                        return *token;
                    }
                    break;
                case STATE_NEQ:
                    if (c == '=') {
                        token->ID = TOKEN_ID_NEQ2;
                        return *token;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_LT:
                    if (c == '=') {
                        token->ID = TOKEN_ID_LTE;
                        return *token;
                    } else {
                        token->ID = TOKEN_ID_LT;
                        ungetc(c, source_file);
                        return *token;
                    }
                    break;
                case STATE_GT:
                    if (c == '=') {
                        token->ID = TOKEN_ID_GTE;
                        return *token;
                    } else {
                        token->ID = TOKEN_ID_GT;
                        ungetc(c, source_file);
                        return *token;
                    }
                    break;
                case STATE_ASSIGN:
                    if (c == '=') {
                        token->ID = TOKEN_ID_EQ;
                        return *token;
                    } else {
                        token->ID = TOKEN_ID_ASSIGN;
                        ungetc(c, source_file);
                        return *token;
                    }
                    break;
                case STATE_STR:
                    if (c == '"') {
                        token->ID = TOKEN_ID_FSTR;
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        if (!(DS_Copy(ptr_Str, token->Value.string))) {/**error**/}
                        free(ptr_Str);
                        return *token;
                    } else if (c == '\\') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_STR2;
                    } else if ((c >= 32 && c != '"') || (c >= 32 && c != '\\')) {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_STR;
                    }else if (c == '\n'){
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_STR;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_STR2:
                    if (c == '0' || c == '1') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_STR3;
                    } else if (c == '2') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_STR6;
                    } else if (c == '\\' || c == '"' || c == 'n' || c == 't') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/ }
                        state = STATE_STR;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_STR3:
                    if (c == '0') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_STR4;
                    } else if (c >= '1' && c <= '9') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_STR5;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_STR4:
                    if (c >= '1' && c <= '9') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_STR;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_STR5:
                    if (c >= '0' && c <= '9') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_STR;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_STR6:
                    if (c >= '0' && c <= '4') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_STR7;
                    } else if (c == '5') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_STR8;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_STR7:
                    if (c >= '0' && c <= '9') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_STR;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_STR8:
                    if (c >= '0' && c <= '5') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_STR;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_INT0:
                    if (c == '0') {
                        state = STATE_INT0;
                    } else if (c == 'e' || c == 'E') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_EXP;
                    } else if (c == '.') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_DBL;
                    } else if (c >= '1' && c <= '9') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_INT;
                    } else {
                        token->ID = TOKEN_ID_INT0;
                        conversion_Int(ptr_Str, token);
                        free(ptr_Str);
                        ungetc(c, source_file);
                        return *token;
                    }
                    break;
                case STATE_ZERO:
                    if (c == 'x') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_HEX;
                    } else if (c == '0') {
                        state = STATE_INT0;
                    } else if (c == 'e' || c == 'E') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_EXP;
                    } else if (c == '.') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_DBL;
                    }else if (c >= '1' && c <= '9') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_INT;
                    } else {
                        token->ID = TOKEN_ID_ZERO;
                        conversion_Int(ptr_Str, token);
                        free(ptr_Str);
                        ungetc(c, source_file);
                        return *token;
                    }
                    break;
                case STATE_HEX:
                    if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9')) {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_HEX2;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_HEX2:
                    if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9')) {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_HEX2;
                    } else if (c == '.') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_DHEX;
                    } else {
                        token->ID = TOKEN_ID_HEX2;
                        conversion_Int(ptr_Str, token);
                        free(ptr_Str);
                        ungetc(c, source_file);
                        return *token;
                    }
                    break;
                case STATE_DHEX:
                    if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9')) {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_DHEX2;
                    } else if (c == 'p') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_HEXP;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_DHEX2:
                    if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9')) {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_DHEX2;
                    } else if (c == 'p') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_HEXP;
                    } else {
                        token->ID = TOKEN_ID_DHEX2;
                        HexaDecimal(ptr_Str->str, token);
                        ungetc(c, source_file);
                        return *token;
                    }
                    break;
                case STATE_HEXP:
                    if (c == '+' || c == '-') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_HEXP2;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_HEXP2:
                    if (c >= '0' && c <= '9') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_HEXP3;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_HEXP3:
                    if (c >= '1' && c <= '9') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_HEXP3;
                    } else {
                        token->ID = TOKEN_ID_HEXP3;
                        conversion_Double(ptr_Str, token);
                        free(ptr_Str);
                        ungetc(c, source_file);
                        return *token;
                    }
                    break;
                case STATE_EXP:
                    if (c >= '0' && c <= '9') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_EXP3;
                    } else if (c == '+' || c == '-') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_EXP2;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_EXP2:
                    if (c >= '0' && c <= '9') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_EXP3;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_EXP3:
                    if (c >= '0' && c <= '9') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_EXP3;
                    } else {
                        token->ID = TOKEN_ID_EXP3;
                        conversion_Double(ptr_Str, token);
                        free(ptr_Str);
                        ungetc(c, source_file);
                        return *token;
                    }
                    break;
                case STATE_DBL:
                    if (c >= '0' && c <= '9') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_DBL2;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_DBL2:
                    if (c == 'e' || c == 'E') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_EXP;
                    } else if (c >= '0' && c <= '9') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_DBL2;
                    } else {
                        token->ID = TOKEN_ID_DBL2;
                        conversion_Double(ptr_Str, token);
                        free(ptr_Str);
                        ungetc(c, source_file);
                        return *token;
                    }
                    break;
                case STATE_INT:
                    if (c == 'e' || c == 'E') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_EXP;
                    } else if (c == '.') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_DBL;
                    } else if (c >= '0' && c <= '9') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_INT;
                    } else {
                        token->ID = TOKEN_ID_INT;
                        conversion_Int(ptr_Str, token);
                        free(ptr_Str);
                        ungetc(c, source_file);
                        return *token;
                    }
                    break;
                case STATE_ID:
                    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {/**error**/}
                        state = STATE_ID;
                    } else {
                        KW_ID_Cmp(ptr_Str, token);
                        free(ptr_Str);
                        ungetc(c, source_file);
                        return *token;
                    }
                    break;
                default:
                    state = STATE_ERROR;
                    break;
            }
            if (state == STATE_ERROR) {
                break;
            }
        }
        token->ID = TOKEN_ID_EOF;
        return *token;
    }
