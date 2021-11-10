#include "scanner.h"
#include "parser.h"

int main(void) {
    FILE *source_file = stdin;
    parse(source_file);
    return 0;
}