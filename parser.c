#include "scanner.h"
#include "parser.h"
#include <stdlib.h>

void parse(FILE *source_file) {
    Token *test = (Token *) malloc(sizeof(Token));
    do {
        get_token(test, source_file);
        //token basic test
        switch (test->ID)
        {
            case TOKEN_ID_KEYWORD:
                switch (test->Value.keyword) {
                    case 0:
                        printf("TOKEN_ID_KEYWORD: KEYWORD_DO\n");
                        break;
                    case 1:
                        printf("TOKEN_ID_KEYWORD: KEYWORD_ELSE\n");
                        break;
                    case 2:
                        printf("TOKEN_ID_KEYWORD: KEYWORD_END\n");
                        break;
                    case 3:
                        printf("TOKEN_ID_KEYWORD: KEYWORD_FUNCTION\n");
                        break;
                    case 4:
                        printf("TOKEN_ID_KEYWORD: KEYWORD_GLOBAL\n");
                        break;
                    case 5:
                        printf("TOKEN_ID_KEYWORD: KEYWORD_IF\n");
                        break;
                    case 6:
                        printf("TOKEN_ID_KEYWORD: KEYWORD_INTEGER\n");
                        break;
                    case 7:
                        printf("TOKEN_ID_KEYWORD: KEYWORD_LOCAL\n");
                        break;
                    case 8:
                        printf("TOKEN_ID_KEYWORD: KEYWORD_NIL\n");
                        break;
                    case 9:
                        printf("TOKEN_ID_KEYWORD: KEYWORD_NUMBER\n");
                        break;
                    case 10:
                        printf("TOKEN_ID_KEYWORD: KEYWORD_REQUIRE\n");
                        break;
                    case 11:
                        printf("TOKEN_ID_KEYWORD: KEYWORD_RETURN\n");
                        break;
                    case 12:
                        printf("TOKEN_ID_KEYWORD: KEYWORD_STRING\n");
                        break;
                    case 13:
                        printf("TOKEN_ID_KEYWORD: KEYWORD_THEN\n");
                        break;
                    case 14:
                        printf("TOKEN_ID_KEYWORD: KEYWORD_WHILE\n");
                        break;
                    default:
                        break;
                }
                break;
            case TOKEN_ID_EOL:
                printf("TOKEN_ID_EOL\n");
                break;
            case TOKEN_ID_TAB:
                printf("TOKEN_ID_TAB\n");
                break;
            case TOKEN_ID_CLN:
                printf("TOKEN_ID_CLN\n");
                break;
            case TOKEN_ID_CMA:
                printf("TOKEN_ID_CMA\n");
                break;
            case TOKEN_ID_SPACE:
                printf("TOKEN_ID_SPACE\n");
                break;
            case TOKEN_ID_RBR:
                printf("TOKEN_ID_RBR\n");
                break;
            case TOKEN_ID_LBR:
                printf("TOKEN_ID_LBR\n");
                break;
            case TOKEN_ID_ADD:
                printf("TOKEN_ID_ADD\n");
                break;
            case TOKEN_ID_MUL:
                printf("TOKEN_ID_MUL\n");
                break;
            case TOKEN_ID_SUB:
                printf("TOKEN_ID_SUB\n");
                break;
            case TOKEN_ID_LCMT2:
                printf("TOKEN_ID_LCMT2\n");
                break;
            case TOKEN_ID_BCMT4:
                printf("TOKEN_ID_BCMT4\n");
                break;
            case TOKEN_ID_DIV2:
                printf("TOKEN_ID_DIV2\n");
                break;
            case TOKEN_ID_DIV:
                printf("TOKEN_ID_DIV\n");
                break;
            case TOKEN_ID_NEQ2:
                printf("TOKEN_ID_NEQ2\n");
                break;
            case TOKEN_ID_LTE:
                printf("TOKEN_ID_LTE\n");
                break;
            case TOKEN_ID_LT:
                printf("TOKEN_ID_LT\n");
                break;
            case TOKEN_ID_GTE:
                printf("TOKEN_ID_GTE\n");
                break;
            case TOKEN_ID_GT:
                printf("TOKEN_ID_GT\n");
                break;
            case TOKEN_ID_EQ:
                printf("TOKEN_ID_EQ\n");
                break;
            case TOKEN_ID_ASSIGN:
                printf("TOKEN_ID_ASSIGN\n");
                break;
            case TOKEN_ID_FSTR:
                printf("TOKEN_ID_FSTR: %s\n", test->Value.string->str);
                break;
            case TOKEN_ID_INT0:
                printf("TOKEN_ID_INT0: %d\n", test->Value.Integer);
                break;
            case TOKEN_ID_HEX2:
                printf("TOKEN_ID_HEX2: %d\n", test->Value.Integer);
                break;
            case TOKEN_ID_DHEX2:
                printf("TOKEN_ID_DHEX2: %lf\n", test->Value.Double);
                break;
            case TOKEN_ID_HEXP3:
                printf("TOKEN_ID_HEXP3: %lf\n", test->Value.Double);
                break;
            case TOKEN_ID_EXP3I:
                printf("TOKEN_ID_HEX2: %d\n", test->Value.Integer);
                break;
            case TOKEN_ID_EXP3D:
                printf("TOKEN_ID_HEXP3: %lf\n", test->Value.Double);
                break;
            case TOKEN_ID_DBL2:
                printf("TOKEN_ID_DBL2: %f\n", test->Value.Double);
                break;
            case TOKEN_ID_INT:
                printf("TOKEN_ID_INT: %d\n", test->Value.Integer);
                break;
            case TOKEN_ID_ID:
                printf("TOKEN_ID_ID: %s\n", test->Value.string->str);
                break;
            case TOKEN_ID_ERROR:
                printf("TOKEN_ID_ERROR\n");
                break;
            case TOKEN_ID_EOF:
                printf("TOKEN_ID_EOF\n");
                break;
            default:
                break;
        }

        // par casu -> po otestovani jednoho bych pak pridal dalsi a ten opet otestoval
        // a asi bude chyba pri TOKEN_ID_ID v (file: Dynamic_string.c, row 32, s_new->str => saha nikam)**/

    //printf("Token:{ Double -> %f, Integer -> %d, Keyword -> %d, String -> %s \n", test->Value.Double, test->Value.Integer, test->Value.keyword, test->Value.string->str);
    } while(test->ID != TOKEN_ID_EOF);
}   