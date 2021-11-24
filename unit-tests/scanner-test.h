#include "../scanner.h"

/**
 * Prelozi enum hodnotu keyword na string obsahujici jeho nazev.                
 *                                                                              
 * @param keyword Hodnota udavajici o jake klicove slovo se jedna               
 * @return String literal odpovidajici klicovemu slovu                          
 */
char *keyword_to_string(Keyword keyword); 

/**
 * Prelozi enum hodnotu Token_ID na string obsahujici jeho nazev.
 *
 * @param keyword Hodnota udavajici o jake ID tokenu se jedna
 * @return String literal odpovidajici id tokenu
 */
char *Token_ID_to_string(Token_ID id);

/**
 * Vypise obsah struktury token.
 *
 * @param *token Ukazatel na strukturu tokenu
 */
void print_token(Token *token);
