/**
 * Projekt: IFJ2021
 *
 * @brief Main funkce compileru.
 *
 * @author Vít Janeček xjanec30@stud.fit.vutbr.cz
 * @author Tomáš Bártů xbartu11@stud.fit.vutbr.cz
 * @author Šimon Vacek xvacek10@stud.fit.vutbr.cz
 * @author Tony Pham xphamt00@stud.fit.vutbr.cz
*/

#include "scanner.h"
#include "parser.h"

/**
 * Main funkce
 *
 * @return Vraci ERROR_PASSED 0 pokud je vsechno v poradku
 */
int main(void) {
    FILE *source_file = stdin;
    int ERROR_TYPE = parser(source_file);
    return ERROR_TYPE;
}

/** @endcode */