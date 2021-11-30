/**
 * Projekt: IFJ2021
 *
 * @brief Implementace dynamickeho stringu pro ukladani v scanneru
 *
 * @author Tony Pham xphamt00@stud.fit.vutbr.cz
 * @author Vít Janeček xjanec30@stud.fit.vutbr.cz
 */

#ifndef _DYNAMIC_STRING_H
#define _DYNAMIC_STRING_H

#include <stdbool.h>

/**
 * @struct Struktura reprezentujici dynamicky string
 */

typedef struct {
    char *str;
    unsigned int length;
    unsigned int alloc;
} Dynamic_string;

/**
 * Funkce inicializuje dynamicky string pro scanner.
 *
 * @param string Ukazatel na string.
 * @return Vraci true pokud se povede inicializace a malloc jinak vraci false.
 */
bool DS_Init(Dynamic_string *string);

/**
 * Funkce prida zank do dynamickyho stringu.
 *
 * @param string Ukazatel na string.
 * @param c Znak, ktery je vkladan do stringu.
 * @return Vraci true pokud se povede vlozit znak do stringu a malloc jinak vraci false.
 */
bool DS_Add(Dynamic_string *string, char c);

/**
 * Funkce prekopiruje jeden string do druheho.
 *
 * @param string Ukazatel na string.
 * @param s_new Ukazatel na string, do kterého bude nakopirovan predchovi parametr string.
 * @return Vraci true pokud se povede nakopirovat string do stringu a malloc jinak vraci false.
 */
bool DS_Copy(Dynamic_string *string,Dynamic_string *s_new);

/**
 * Funkce porovná 2 stringy.
 *
 * @param string Ukazatel na string.
 * @param c_string Ukazatel na string, se kterym se bude porovnavat 1. string.
 * @return Vraci 0 pokud se rovnají, jinak vrací ASCI hodnotu podle toho jestli je dany char vetsi nebo mensi.
 */
int DS_Cmp(Dynamic_string *string, const char *c_string);

/**
 * Funkce odstrani dynamicky string.
 *
 * @param string Ukazatel na string.
 */
void DS_Free(Dynamic_string *string);

#endif
