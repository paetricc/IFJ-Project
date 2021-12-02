#include "term_stack.h"
#include "error.h"
#include "symtable.h"

TermsAndNonTerms convertTokenType_To_TermsAndNonTerms( Token_ID );

int exprSyntaxCheck( Token *, FILE *, SLList_Frame * );

int skipNonPrintChar(Token *, FILE * );

int checkRulesAndApply( TermStack * );

int SA_isOK( TermStack * );