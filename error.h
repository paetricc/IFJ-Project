/**
 * Projekt: IFJ2021
 *
 * @brief Rozdeleni chybnych stavu.
 *
 * @author Tony Pham xphamt00@stud.fit.vutbr.cz
 */

#ifndef _ERROR_H
#define _ERROR_H

/**
* Chybne stavy.
*/
#define ERROR_PASSED 0
#define ERROR_LEX 1
#define ERROR_SYNTAX 2
#define ERROR_SEM_UNDEFINED 3
#define ERROR_SEM_ASSIGN 4
#define ERROR_SEM_TYPE_COUNT 5
#define ERROR_SEM_COMPAT 6
#define ERROR_SEM_OTHERS 7
#define ERROR_RUNTIME_NIL 8
#define ERROR_RUNTIME_DIV_ZERO 9
#define ERROR_COMPILER 99

#endif
