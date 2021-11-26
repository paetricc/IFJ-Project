#include "scanner.h"
#include "parser.h"
#include "error.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief Nacita tokeny dokud je v nich nektery bily znak, nebo az se narazi na EOF.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 *
 * @return Typ erroru predany scannerem
*/
int get_non_white_token(Token *token, FILE *sourceFile) {
  int error;
  do {
    // nacteni noveho tokenu a kontrola vysupu scanneru
    if((error = get_token(token, sourceFile)) != ERROR_PASSED)
	  // lexikalni nebo kompilatorova chyba
      return error;
  } while(token->ID != TOKEN_ID_EOF && ( 
              token->ID == TOKEN_ID_SPACE || token->ID == TOKEN_ID_EOL ||
              token->ID == TOKEN_ID_TAB));
  return error;
}

/**
 * Pravidlo 1
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru predany scannerem
*/
int start(Token *token, FILE *sourceFile) {
  int error;
  if((error = get_non_white_token(token, sourceFile)))
	  // lexikalni nebo kompilatorova chyba
    return error;

	// require
	if(token->ID != TOKEN_ID_KEYWORD)
		return ERROR_SYNTAX;
  else if(token->Value.keyword != KEYWORD_REQUIRE)
		return ERROR_SYNTAX;

  if((error = get_non_white_token(token, sourceFile)))
	  // lexikalni nebo kompilatorova chyba
    return error;

	// "ifj21"
	if(token->ID != TOKEN_ID_FSTR)
		return ERROR_SYNTAX;
	else if(strcmp(token->Value.string->str, "\"ifj21\""))
		return ERROR_SYNTAX;

  // vse korektni - uplatnuju pravidlo a rozsiruju dalsi neterminal
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
