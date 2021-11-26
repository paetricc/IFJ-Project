#include "scanner.h"
#include "parser.h"
#include "error.h"
#include <stdlib.h>
#include <string.h>

/**
 * Pravidlo 1
 *
 * @param token Token, ktery bude naplni scanner
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru predany scannerem
*/
int start(Token *token, FILE *sourceFile) {
	int error = get_token(token, sourceFile);
	if(error != ERROR_PASSED)	// lexikalni nebo kompilatorova chyba
		return error;

	// require
	if(token->ID != TOKEN_ID_KEYWORD)
		return ERROR_SYNTAX;
  else if(token->Value.keyword != KEYWORD_REQUIRE)
		return ERROR_SYNTAX;

	error = get_token(token, sourceFile);
	if(error != ERROR_PASSED)	// lexikalni nebo kompilatorova chyba
		return error;

	// "ifj21"
	if(token->ID != TOKEN_ID_FSTR)
		return ERROR_SYNTAX;
	else if(!strcmp(token->Value.string->str, "ifj21"))
		return ERROR_SYNTAX;

  return program(token, sourceFile);
}

/**
 * Pravidlo 2
 *
 * @param token Token, ktery bude naplni scanner
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru predany scannerem
*/
int program(Token *token, FILE *sourceFile) {
  return 0;
}

int parser(FILE *sourceFile) {
	// vytvoreni struktury token, kterou bude scanner plnit
	Token *token = (Token *) malloc(sizeof(Token));
	if(token == NULL)
		return ERROR_COMPILER;

	// volani prvniho pravidla a nahrazovani prvniho neterminalu
	int error = start(token, sourceFile);

  free(token);
	return error;
}   
