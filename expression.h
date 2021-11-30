#include "term_stack.h"
#include "error.h"
#include "./scanner-test.h"

TermsAndNonTerms convertTokenType_To_TermsAndNonTerms( Token_ID );

int exprSyntaxCheck( Token *, FILE * );

void skipNonPrintChar(Token *, FILE * );

int checkRulesAndApply( TermStack * );

int SA_isOK( TermStack * );