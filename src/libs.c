#include "libs.h"
#include <stdlib.h>
#include <string.h>

char *newString(char *s) {
    if (s == NULL)
        return NULL;
    char *copia = malloc(strlen(s) + 1);
    strcpy(copia, s);
    return copia;
}
