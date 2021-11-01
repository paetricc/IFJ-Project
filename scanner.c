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
                if (c >= '0' && c <='2') {
                    state = STATE_STR3;
                } else if (c == '\\' || c == '"' || c == 'n' || c == 't'){
                    state = STATE_STR;
                } else {
                    state = STATE_ERROR;
                }
                break;
            case STATE_STR3:
                if (c >= '0' && c <='5') {
                    state = STATE_STR4;
                } else {
                    state = STATE_ERROR;
                }
                break;
            case STATE_STR4:
                if (c >= '1' && c <='5') {
                    state = STATE_STR;
                } else {
                    state = STATE_ERROR;
                }
                break;
            case STATE_INT0:
                if (c == 'x') {
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
                    state = STATE_HEX2;
                } else{
                    state = STATE_ERROR;
                }
                break;
            case STATE_HEX2:
                if ((c >= 'a' && c <='f') || (c >= 'A' && c <='F') || (c >= '0' && c <='9')) {
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
                    state = STATE_DHEX2;
                } else if (c == 'p'){
                    state = STATE_HEXP;
                } else{
                    state = STATE_ERROR;
                }
                break;
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
                    state = STATE_HEXP2;
                } else{
                    state = STATE_ERROR;
                }
                break;
            case STATE_HEXP2:
                if (c >= '0' && c <='9'){
                    state = STATE_HEXP3;
                } else{
                    state = STATE_ERROR;
                }
                break;
            case STATE_HEXP3:
                if (c >= '1' && c <='9'){
                    state = STATE_HEXP3;
                } else{
                    printf("token HEXP3\n"); //token HEXP3
                    state = STATE_START;
                    ungetc(c, source_file);
                }
                break;
            case STATE_EXP:
                if (c >= '0' && c <='9'){
                    state = STATE_EXP3;
                } else if (c == '+' || c == '-'){
                    state = STATE_EXP2;
                } else{
                    state = STATE_ERROR;
                }
                break;
            case STATE_EXP2:
                if (c >= '0' && c <='9'){
                    state = STATE_EXP3;
                } else{
                    state = STATE_ERROR;
                }
                break;
            case STATE_EXP3:
                if (c >= '0' && c <='9'){
                    state = STATE_EXP3;
                } else{
                    printf("token EXP3\n"); //token EXP3
                    state = STATE_START;
                    ungetc(c, source_file);
                }
                break;
            case STATE_DBL:
                if (c >= '0' && c <='9'){
                    state = STATE_DBL2;
                } else{
                    state = STATE_ERROR;
                }
                break;
            case STATE_DBL2:
                if (c == 'e' || c =='E') {
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