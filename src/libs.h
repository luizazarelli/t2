#ifndef LIBS_H
#define LIBS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

/*
    Modulo de funcoes utilitarias gerais utilizadas pelos demais modulos do sistema.
    Fornece operacoes auxiliares de manipulacao de strings e alocacao de memoria.
*/

char *newString(char *s);
/*
    Aloca e retorna uma copia da string apontada por "s" na heap.
    Retorna NULL caso "s" seja NULL.
*/

#endif
