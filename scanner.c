/**
 * Projekt: IFJ2021
 *
 * @brief Implementace scanneru na predavani tokenu.
 *
 * @author Tomáš Bártů xbartu11@stud.fit.vutbr.cz
 * @author Tony Pham xphamt00@stud.fit.vutbr.cz
 * @author Vít Janeček xjanec30@stud.fit.vutbr.cz
 */

#include "scanner.h"

/**
 * Funkce prevede string na int a ulozi ho do tokenu.
 *
 * @param str Ukazatel na string k prevedeni.
 * @param token Ukazatel na token do ktereho se ulozi prevedeny int.
 */
void conversion_Int(Dynamic_string *str, Token *token){
    char *ptr;
    long value = (token->ID == TOKEN_ID_INT || token->ID == TOKEN_ID_INT0) ?
                strtol(str->str,&ptr,10) : strtol(str->str,&ptr,16);
    token->Value.Integer = (int)value;
}

/**
 * Funkce prevede string na double a ulozi ho do tokenu.
 *
 * @param str Ukazatel na string k prevedeni.
 * @param token Ukazatel na token do ktereho se ulozi prevedeny double.
 */

void conversion_Double(Dynamic_string *str, Token *token){
    token->Value.Double = strtod(str->str, NULL);
}

/**
 * Funkce prida znak do stringu.
 *
 * @param str Ukazatel na string.
 * @param c Znak ktery je pridan do stringu.
 * @return vrati true pokud se povede pridat char do stringu jinak vraci false.
 */
bool DS_Add_Tester(Dynamic_string *str, char c) {
    if (!(DS_Add(str, c))) {
        free(str);
        return false;
    }
    return true;
}

/**
 * Funkce prevede string na hexadecimal a ulozi ho do tokenu.
 *
 * @param name Ukazatel na string.
 * @param res Ukazatel na token do ktereho se ulozi prevedeny HexaDecimal.
 */
void HexaDecimal(char const *name, Token *res) {
    int point = 0;
    while(name[point++] != '.');
    int WholeNumberPart = point - 3;
    double exp = 1.0f;
    double tmp = 0;
    while(WholeNumberPart-- != 1) exp = exp * 16;
    for (int i = 2; name[i] != '\0'; i++) {
        if(name[i] == '.') continue;
        int shift;
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

/**
 * Funkce porovna string s keywordy, jestli se string rovna keywordu da ho do value.keyword a zapise ho do tokenu keyword
 * jinak ho da do tokenu id a value.string.
 *
 * @param str Ukazatel na string.
 * @param token Ukazatel na token do ktereho se ulozi klicove slovo nebo id.
 */
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
            DS_Free(str);
        }

    }
}

int get_token(Token *token, FILE *source_file ) {
    Dynamic_string *ptr_Str = (Dynamic_string *)malloc(sizeof(Dynamic_string));
    if (!(DS_Init(ptr_Str))) return ERROR_COMPILER;
    
        char c;
        int state = STATE_START;
        do {
            c = (char) getc(source_file);
            switch (state) {
                case STATE_START:
                    switch (c) {
                        case EOF:
                            token->ID = TOKEN_ID_EOF;
                            DS_Free(ptr_Str);
                            return ERROR_PASSED;
                        case 9:
                            token->ID = TOKEN_ID_TAB;
                            DS_Free(ptr_Str);
                            return ERROR_PASSED;
                        case ':':
                            token->ID = TOKEN_ID_CLN;
                            DS_Free(ptr_Str);
                            return ERROR_PASSED;
                        case '\n':
                        case '\r':
                            token->ID = TOKEN_ID_EOL;
                            DS_Free(ptr_Str);
                            return ERROR_PASSED;
                        case ',':
                            token->ID = TOKEN_ID_CMA;
                            DS_Free(ptr_Str);
                            return ERROR_PASSED;
                        case ' ':
                            token->ID = TOKEN_ID_SPACE;
                            DS_Free(ptr_Str);
                            return ERROR_PASSED;
                        case ')':
                            token->ID = TOKEN_ID_RBR;
                            DS_Free(ptr_Str);
                            return ERROR_PASSED;
                        case '(':
                            token->ID = TOKEN_ID_LBR;
                            DS_Free(ptr_Str);
                            return ERROR_PASSED;
                        case '+':
                            token->ID = TOKEN_ID_ADD;
                            DS_Free(ptr_Str);
                            return ERROR_PASSED;
                        case '-':
                            state = STATE_SUB;
                            break;
                        case '*':
                            token->ID = TOKEN_ID_MUL;
                            DS_Free(ptr_Str);
                            return ERROR_PASSED;
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
                            if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                            token->Value.string = (Dynamic_string *)malloc(sizeof(Dynamic_string));
                            if (!(DS_Init(token->Value.string))) return ERROR_COMPILER;
                            state = STATE_STR;
                            break;
                        case '0':
                            if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                            state = STATE_ZERO;
                            break;
                        case '1' ... '9':
                            if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                            state = STATE_INT;
                            break;
                        case 'A' ... 'Z':
                            if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                            state = STATE_ID;
                            token->Value.string = (Dynamic_string *)malloc(sizeof(Dynamic_string));
                            if (!(DS_Init(token->Value.string))) {
                                return ERROR_COMPILER;
                            }
                            break;
                        case 'a' ... 'z':
                            if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                            state = STATE_ID;
                            token->Value.string = (Dynamic_string *)malloc(sizeof(Dynamic_string));
                            if (!(DS_Init(token->Value.string))) {
                                return ERROR_COMPILER;
                            }
                            break;
                        case '_':
                            if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                            state = STATE_ID;
                            token->Value.string = (Dynamic_string *)malloc(sizeof(Dynamic_string));
                            if (!(DS_Init(token->Value.string))) {
                                return ERROR_COMPILER;
                            }
                            break;
                        case '\0':
                            break;
                        case '.':
                            state = STATE_DOT;
                            break;
                        case '#':
                            token->ID = TOKEN_ID_LEN;
                            DS_Free(ptr_Str);
                            return ERROR_PASSED;
                        default:
                            state = STATE_ERROR;
                            break;
                    }
                    break;
                case STATE_DOT:
                    if (c == '.') {
                        token->ID = TOKEN_ID_DDOT;
                        DS_Free(ptr_Str);
                        return ERROR_PASSED;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_SUB:
                    if (c == '-') {
                        state = STATE_LCMT2;
                    } else {
                        token->ID = TOKEN_ID_SUB;
                        DS_Free(ptr_Str);
                        ungetc(c, source_file);
                        return ERROR_PASSED;
                    }
                    break;
                case STATE_LCMT2:
                    if (c == '[') {
                        state = STATE_BCMT;
                    } else if (c == '\n') {
                        token->ID = TOKEN_ID_LCMT2;
                        DS_Free(ptr_Str);
                        ungetc(c, source_file);
                        return ERROR_PASSED;
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
                        DS_Free(ptr_Str);
                        return ERROR_PASSED;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_DIV:
                    if (c == '/') {
                        token->ID = TOKEN_ID_DIV2;
                        DS_Free(ptr_Str);
                        return ERROR_PASSED;
                    } else {
                        token->ID = TOKEN_ID_DIV;
                        DS_Free(ptr_Str);
                        ungetc(c, source_file);
                        return ERROR_PASSED;
                    }
                case STATE_NEQ:
                    if (c == '=') {
                        token->ID = TOKEN_ID_NEQ2;
                        DS_Free(ptr_Str);
                        return ERROR_PASSED;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_LT:
                    if (c == '=') {
                        token->ID = TOKEN_ID_LTE;
                        DS_Free(ptr_Str);
                        return ERROR_PASSED;
                    } else {
                        token->ID = TOKEN_ID_LT;
                        DS_Free(ptr_Str);
                        ungetc(c, source_file);
                        return ERROR_PASSED;
                    }
                case STATE_GT:
                    if (c == '=') {
                        token->ID = TOKEN_ID_GTE;
                        DS_Free(ptr_Str);
                        return ERROR_PASSED;
                    } else {
                        token->ID = TOKEN_ID_GT;
                        DS_Free(ptr_Str);
                        ungetc(c, source_file);
                        return ERROR_PASSED;
                    }
                case STATE_ASSIGN:
                    if (c == '=') {
                        token->ID = TOKEN_ID_EQ;
                        DS_Free(ptr_Str);
                        return ERROR_PASSED;
                    } else {
                        token->ID = TOKEN_ID_ASSIGN;
                        DS_Free(ptr_Str);
                        ungetc(c, source_file);
                        return ERROR_PASSED;
                    }
                case STATE_STR:
                    if (c == '"') {
                        token->ID = TOKEN_ID_FSTR;
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        if (!(DS_Copy(ptr_Str, token->Value.string))) {return ERROR_COMPILER;}
                        DS_Free(ptr_Str);
                        return ERROR_PASSED;
                    } else if (c == '\\') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_STR2;
                    } else if (c >= 32) {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_STR;
                    } else if (c == '\n'){
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_STR;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_STR2:
                    if (c == '0' || c == '1') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_STR3;
                    } else if (c == '2') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_STR6;
                    } else if (c == '\\' || c == '"' || c == 'n' || c == 't') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_STR;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_STR3:
                    if (c == '0') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_STR4;
                    } else if (c >= '1' && c <= '9') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_STR5;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_STR4:
                    if (c >= '1' && c <= '9') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_STR;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_STR5:
                    if (c >= '0' && c <= '9') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_STR;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_STR6:
                    if (c >= '0' && c <= '4') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_STR7;
                    } else if (c == '5') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_STR8;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_STR7:
                    if (c >= '0' && c <= '9') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_STR;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_STR8:
                    if (c >= '0' && c <= '5') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_STR;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_INT0:
                    if (c == '0') {
                        state = STATE_INT0;
                    } else if (c == 'e' || c == 'E') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_EXP;
                    } else if (c == '.') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_DBL;
                    } else if (c >= '1' && c <= '9') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_INT;
                    } else {
                        token->ID = TOKEN_ID_INT0;
                        conversion_Int(ptr_Str, token);
                        DS_Free(ptr_Str);
                        ungetc(c, source_file);
                        return ERROR_PASSED;
                    }
                    break;
                case STATE_ZERO:
                    if (c == 'x') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_HEX;
                    } else if (c == '0') {
                        state = STATE_INT0;
                    } else if (c == 'e' || c == 'E') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_EXP;
                    } else if (c == '.') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_DBL;
                    }else if (c >= '1' && c <= '9') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_INT;
                    } else {
                        token->ID = TOKEN_ID_ZERO;
                        conversion_Int(ptr_Str, token);
                        DS_Free(ptr_Str);
                        ungetc(c, source_file);
                        return ERROR_PASSED;
                    }
                    break;
                case STATE_HEX:
                    if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9')) {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_HEX2;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_HEX2:
                    if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9')) {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_HEX2;
                    } else if (c == '.') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_DHEX;
                    } else {
                        token->ID = TOKEN_ID_HEX2;
                        conversion_Int(ptr_Str, token);
                        DS_Free(ptr_Str);
                        ungetc(c, source_file);
                        return ERROR_PASSED;
                    }
                    break;
                case STATE_DHEX:
                    if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9')) {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_DHEX2;
                    } else if (c == 'p') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_HEXP;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_DHEX2:
                    if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9')) {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_DHEX2;
                    } else if (c == 'p') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_HEXP;
                    } else {
                        token->ID = TOKEN_ID_DHEX2;
                        HexaDecimal(ptr_Str->str, token);
                        DS_Free(ptr_Str);
                        ungetc(c, source_file);
                        return ERROR_PASSED;
                    }
                    break;
                case STATE_HEXP:
                    if (c == '+' || c == '-') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_HEXP2;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_HEXP2:
                    if (c >= '0' && c <= '9') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_HEXP3;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_HEXP3:
                    if (c >= '1' && c <= '9') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_HEXP3;
                    } else {
                        token->ID = TOKEN_ID_HEXP3;
                        conversion_Double(ptr_Str, token);
                        DS_Free(ptr_Str);
                        ungetc(c, source_file);
                        return ERROR_PASSED;
                    }
                    break;
                case STATE_EXP:
                    if (c >= '0' && c <= '9') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_EXP3;
                    } else if (c == '+' || c == '-') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_EXP2;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_EXP2:
                    if (c >= '0' && c <= '9') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_EXP3;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_EXP3:
                    if (c >= '0' && c <= '9') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_EXP3;
                    } else {
                        token->ID = TOKEN_ID_EXP3;
                        conversion_Double(ptr_Str, token);
                        DS_Free(ptr_Str);
                        ungetc(c, source_file);
                        return ERROR_PASSED;
                    }
                    break;
                case STATE_DBL:
                    if (c >= '0' && c <= '9') {
                        if (!(DS_Add_Tester(ptr_Str, c))) { return ERROR_COMPILER; }
                        state = STATE_DBL2;

                    } else if (c == 'e' || c == 'E') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_EXP;
                    } else {
                        state = STATE_ERROR;
                    }
                    break;
                case STATE_DBL2:
                    if (c == 'e' || c == 'E') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_EXP;
                    } else if (c >= '0' && c <= '9') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_DBL2;
                    } else {
                        token->ID = TOKEN_ID_DBL2;
                        conversion_Double(ptr_Str, token);
                        DS_Free(ptr_Str);
                        ungetc(c, source_file);
                        return ERROR_PASSED;
                    }
                    break;
                case STATE_INT:
                    if (c == 'e' || c == 'E') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_EXP;
                    } else if (c == '.') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_DBL;
                    } else if (c >= '0' && c <= '9') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_INT;
                    } else {
                        token->ID = TOKEN_ID_INT;
                        conversion_Int(ptr_Str, token);
                        DS_Free(ptr_Str);
                        ungetc(c, source_file);
                        return ERROR_PASSED;
                    }
                    break;
                case STATE_ID:
                    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_') {
                        if (!(DS_Add_Tester(ptr_Str, c))) {return ERROR_COMPILER;}
                        state = STATE_ID;
                    } else {
                        KW_ID_Cmp(ptr_Str, token);
                        DS_Free(ptr_Str);
                        ungetc(c, source_file);
                        return ERROR_PASSED;
                    }
                    break;
                default:
                    state = STATE_ERROR;
                    break;
            }

            if (state == STATE_ERROR) {
                DS_Free(ptr_Str);
                return ERROR_LEX;
            }
        } while ((c != EOF) || (state == STATE_STR));

        return ERROR_PASSED;
    }
