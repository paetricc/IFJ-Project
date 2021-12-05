#include "term_stack.h"
#include "type_stack.h"
#include "error.h"
#include "symtable.h"
#include <string.h>

TermsAndNonTerms convertTokenType_To_TermsAndNonTerms( Token *, TypeStack *);

int exprSyntaxCheck( Token *, FILE *, SLList_Frame *, Data_type, char * );

int skipNonPrintChar(Token *, FILE * );

int checkRulesAndApply( TermStack *, TypeStack * );

int checkDataTypes_ADD_SUB_MUL_DIV( TypeStack * );

int checkDataTypes_DIV2( TypeStack * );

int checkDataTypes_LT_GT( TypeStack * );

int checkDataTypes_EQ_NEQ(TypeStack *);

int checkDataTypes_LTE_GTE(TypeStack *);

int checkDataTypes_DDOT(TypeStack *);

int SA_isOK( TermStack * );

char *converString(char *);

void freeStacks( TermStack *, TypeStack * );