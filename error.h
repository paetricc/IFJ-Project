/**
 * Projekt: IFJ2021
 *
 * @brief Rozdeleni chybnych stavu.
 *
 * @author Tomáš Bártů xbartu11@stud.fit.vutbr.cz
 * @author Tony Pham xphamt00@stud.fit.vutbr.cz
 */

#ifndef _ERROR_H_
#define _ERROR_H_

/** Nedošlo k žádne chybě */
#define ERROR_PASSED 0
/** Chyba v programu v rámci lexikální analýzy (chybná struktura aktuálního lexému) */
#define ERROR_LEX 1
/** Chyba v programu v rámci syntaktické analýzy (chybná syntaxe programu). */
#define ERROR_SYNTAX 2
/** Sémantická chyba v programu – nedefinovaná funkce/proměnná, pokus o redefinici proměnné, atp */
#define ERROR_SEM_UNDEFINED 3
/** Sémantická chyba v příkazu přiřazení (typová nekompatibilita) */
#define ERROR_SEM_ASSIGN 4
/** Sémantická chyba v programu – špatný počet/typ parametrů či návratových hodnot
u volání funkce či návratu z funkce */
#define ERROR_SEM_TYPE_COUNT 5
/** Sémantická chyba typové kompatibility v aritmetických, řetězcových a relačních
výrazech */
#define ERROR_SEM_COMPAT 6
/** Ostatní sémantické chyby */
#define ERROR_SEM_OTHERS 7
/** Běhová chyba při práci s neočekávanou hodnotou nil */
#define ERROR_RUNTIME_NIL 8
/** Běhová chyba celočíselného dělení nulovou konstantou */
#define ERROR_RUNTIME_DIV_ZERO 9
/** Interní chyba překladače tj. neovlivněná vstupním programem (např. chyba alokace paměti atd.) */
#define ERROR_COMPILER 99

#endif

/** @endcode */
