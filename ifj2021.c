#include "scanner.h"
#include "parser.h"

int main(void) {
    FILE *source_file = stdin;
    int ERROR_TYPE = parser(source_file);
    return ERROR_TYPE;
}
