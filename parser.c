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
 * @return Typ erroru generovany analyzou
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
 * @brief Neterminal start.
 *
 * Implementuje pravidlo 1.
 *
 * @param token Token, ktery bude naplnen scannerem
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int start(Token *token, FILE *sourceFile) {
  int error;
	// require
  if((error = get_non_white_token(token, sourceFile)))
	  // lexikalni nebo kompilatorova chyba
    return error;

	if(token->ID != TOKEN_ID_KEYWORD)
		return ERROR_SYNTAX;
  else if(token->Value.keyword != KEYWORD_REQUIRE)
		return ERROR_SYNTAX;

	// "ifj21"
  if((error = get_non_white_token(token, sourceFile)))
	  // lexikalni nebo kompilatorova chyba
    return error;

	if(token->ID != TOKEN_ID_FSTR)
		return ERROR_SYNTAX;
	else if(strcmp(token->Value.string->str, "\"ifj21\""))
		return ERROR_SYNTAX;
  // vse korektni - uplatnuju pravidlo a rozsiruju dalsi neterminal
  return program(token, sourceFile); // aplikace pravidla 1
}

/**
 * @brief Neterminal program.
 *
 * Implementuje pravidla 2, 3, 4, 5.
 *
 * @param token Token, ktery bude naplni scanner
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int program(Token *token, FILE *sourceFile) {
  int error;
  if((error = get_non_white_token(token, sourceFile)))
	  // lexikalni nebo kompilatorova chyba
    return error;
  
  // rozvetveni na ruzna pravidla podle hodnoty tokenu
  switch(token->ID) {
    case TOKEN_ID_KEYWORD: // global nebo function
      if(token->Value.keyword == KEYWORD_GLOBAL) { // global
        error = fnc_dec(token, sourceFile); // aplikace pravidla 2
        if(error)
           return error;
      }
      else if(token->Value.keyword == KEYWORD_FUNCTION) { // function
        error = fnc_def(token, sourceFile); // aplikace pravidla 4
        if(error)
          return error;
      }
      else { // private keyword nelze pouzit - pravidlo neexistuje
        return ERROR_SYNTAX;
      }
    break;

    case TOKEN_ID_ID: // id_fnc
      //TODO potrebuju symtable
      Node *idFce = search(token->Value.string->str);
      if (idFce == NULL) // id takove funkce neexistuje
        return ERROR_SYNTAX;
      else if(!isVar(idFce) && isDeclared(idFce)) { // id_fnce
        error = fnc_call(token, sourceFile); // aplikace pravidla 3
        if(error)
          return error;
      }
      else // id_var neni dovoleno => error
        return ERROR_SYNTAX;
      break;

    case TOKEN_ID_EOF: // konec souboru - syntaxe je korektni, muze se ukoncit
      return ERROR_PASSED; // aplikace pravidla 5

    default: // pro token zadne pravidlo neexistuje
      return ERROR_SYNTAX;
  } // switch

  // rekurzivni volani neterminalu program
  return program(token, sourceFile);
} //program


/**
 * @brief Neterminal fnc_dec.
 *
 * Implementuje pravidlo 6.
 *
 * @param token Token, ktery bude naplni scanner
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int fnc_dec(Token *token, FILE *sourceFile) {
  // token global byl prijaty o uroven vyse => pokracuju dale
  // aplikuju pravidlo 6
  int error;

  // id_fnc
  if((error = get_non_white_token(token, sourceFile)))
	  // lexikalni nebo kompilatorova chyba
    return error;

  if(token->ID != TOKEN_ID_ID)
    return ERROR_SYNTAX;
  // TODO pridat tento identifikator do symtable
  symTable_Add_Fnc(token->Value.string->str);

  
  // ':'
  if((error = get_non_white_token(token, sourceFile)))
	  // lexikalni nebo kompilatorova chyba
    return error;

  if(token->ID != TOKEN_ID_CLN)
    return ERROR_SYNTAX;


  // function
  if((error = get_non_white_token(token, sourceFile)))
	  // lexikalni nebo kompilatorova chyba
    return error;

  if(token->ID != TOKEN_ID_KEYWORD)
    return ERROR_SYNTAX;
  else if(token->Value.keyword != KEYWORD_FUNCTION)
    return ERROR_SYNTAX;

  
  // '('
  if((error = get_non_white_token(token, sourceFile)))
	  // lexikalni nebo kompilatorova chyba
    return error;

  if(token->ID != TOKEN_ID_LBR)
    return ERROR_SYNTAX;


  // rozvinuti neterminalu params_dec
  if((error = params_dec(token, sourceFile)))
    return error;


  // ')'
  if((error = get_non_white_token(token, sourceFile)))
	  // lexikalni nebo kompilatorova chyba
    return error;

  if(token->ID != TOKEN_ID_RBR)
    return ERROR_SYNTAX;


  // rozvinuti neterminalu return_type
  if((error = return_type(token, sourceFile)))
    return error;
  
  return ERROR_PASSED;
} // fnc_dec


/**
 * @brief Neterminal params_dec.
 *
 * Implementuje pravidla 7,8.
 *
 * @param token Token, ktery bude naplni scanner
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int params_dec(Token *token, FILE *sourceFile) {
  int error;

  // promenne pro pripadne vraceni cteni pred zavorkovy token
  fpos_t lastReadPos;
  fgetpos(sourceFile, &lastReadPos);
  if((error = get_non_white_token(token, sourceFile)))
	  // lexikalni nebo kompilatorova chyba
    return error;

  // rozvetveni na ruzna pravidla podle hodnoty tokenu
  switch(token->ID) {
    case TOKEN_ID_RBR: // )
      // vratim cteni pred zavorku, aby ji mohl precist volajici
      fsetpos(sourceFile, &lastReadPos);
      return ERROR_PASSED; // aplikace pravidla 7

    case TOKEN_ID_KEYWORD: // integer, number nebo string
      if(token->Value.keyword == KEYWORD_INTEGER || token->Value.keyword == KEYWORD_NUMBER ||
              token->Value.keyword == KEYWORD_STRING) {

        // vratim cteni pred keyword, aby ji mohl precist volany
        fsetpos(sourceFile, &lastReadPos);

        // rozvinuti neterminalu data_type
        if((error = data_type(token, sourceFile))) // aplikace pravidla 8
          return error;
      }
      else // prijate keyword nelze pouzit - pravidlo neexistuje
        return ERROR_SYNTAX;
    break;

    default: // pro token zadne pravidlo neexistuje
      return ERROR_SYNTAX;
  }

  return params_dec2(token, sourceFile);
} // params_dec


/**
 * @brief Neterminal params_dec2.
 *
 * Implementuje pravidla 9, 10.
 *
 * @param token Token, ktery bude naplni scanner
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int params_dec2(Token *token, FILE *sourceFile) {
  int error;

  // promenne pro pripadne vraceni cteni pred zavorkovy token
  fpos_t lastReadPos;
  fgetpos(sourceFile, &lastReadPos);
  if((error = get_non_white_token(token, sourceFile)))
	  // lexikalni nebo kompilatorova chyba
    return error;

  if(token->ID == TOKEN_ID_RBR) { // ')'
    // vratim cteni pred zavorku, aby ji mohl precist volajici
    fsetpos(sourceFile, &lastReadPos);
    return ERROR_PASSED; // aplikace pravidla 9
  }
  else if(token->ID == TOKEN_ID_CMA) { // ','
    if((error = data_type(token, sourceFile))) // aplikace pravidla 10
      return error;
  }
  else {
    return ERROR_SYNTAX;
  }

  return params_dec2(token, sourceFile);
}

/**
 * @brief Neterminal return_type.
 *
 * Implementuje pravidla 11, 12.
 *
 * @param token Token, ktery bude naplni scanner
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int return_type(Token *token, FILE *sourceFile) {
  int error;

  // promenne pro pripadne vraceni cteni pred zavorkovy token
  fpos_t lastReadPos;
  fgetpos(sourceFile, &lastReadPos);
  if((error = get_non_white_token(token, sourceFile)))
	  // lexikalni nebo kompilatorova chyba
    return error;

  // rozvetveni na ruzna pravidla podle hodnoty tokenu
  switch(token->ID) {
    case TOKEN_ID_CLN: // ':'
    break;

    case TOKEN_ID_KEYWORD: // global nebo function
      if(token->Value.keyword == KEYWORD_GLOBAL || token->Value.keyword == KEYWORD_FUNCTION) {
        // vratim cteni pred keyword, aby ho mohl znovu precist volajici
        fsetpos(sourceFile, &lastReadPos);
        return ERROR_PASSED; // aplikace pravidla 12
      }
      else { // prijate keyword nelze pouzit - pravidlo neexistuje
        return ERROR_SYNTAX;
      }

    case TOKEN_ID_EOF: // konec souboru
      // vratim cteni pred keyword, aby ho mohl znovu precist volajici
      fsetpos(sourceFile, &lastReadPos);
      return ERROR_PASSED; // aplikace pravidla 12

    case TOKEN_ID_ID: // id_var
      // vratim cteni pred keyword, aby ho mohl znovu precist volajici
      fsetpos(sourceFile, &lastReadPos);
      return ERROR_PASSED; // aplikace pravidla 12

    default: // pro token zadne pravidlo neexistuje
      return ERROR_SYNTAX;
  }

  return data_type(token, sourceFile); // aplikace pravidla 11
} // return_type


/**
 * @brief Neterminal data_type.
 *
 * Implementuje pravidel 13, 14, 15.
 *
 * @param token Token, ktery bude naplni scanner
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
int data_type(Token *token, FILE *sourceFile) {
  int error;
  if((error = get_non_white_token(token, sourceFile)))
	  // lexikalni nebo kompilatorova chyba
    return error;
  
  if(token->ID != TOKEN_ID_KEYWORD) // nevalidni token
    return ERROR_SYNTAX;

  // rozvetveni na ruzna pravidla podle hodnoty tokenu
  switch(token->Value.keyword) {
    case KEYWORD_INTEGER: // aplikace pravidla 13
    case KEYWORD_NUMBER: // aplikace pravidla 14
    case KEYWORD_STRING: // aplikace pravidla 15
    break;

    default:
      return ERROR_SYNTAX; // nevalidni klicove slovo
  }

  return ERROR_PASSED;
}

/**
 * @brief Parser
 *
 * @param sourceFile Zdrojovy soubor cteny scannerem
 * @return Typ erroru generovany analyzou
*/
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
