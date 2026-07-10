#include "quadra.h"
#include "hash.h"
#include <stdlib.h>

struct Quadra {
    char *cep;
    double x, y, w, h;
    double sw;
    char *cfill;
    char *cstrk;
};

struct TabelaQuadra {
    Hash *h;
};

TabelaQuadra *tabelaQuadra_criar(int capacidade) {
    TabelaQuadra *t = malloc(sizeof(TabelaQuadra));
    if (t == NULL)
        return NULL;
    t->h = hash_criar(capacidade);
    if (t->h == NULL) {
        free(t);
        return NULL;
    }
    return t;
}

void tabelaQuadra_destruir(TabelaQuadra **tabela) {
    if (tabela == NULL || *tabela == NULL)
        return;
    TabelaQuadra *t = *tabela;
    int cap = hash_capacidade(t->h);
    for (int i = 0; i < cap; i++) {
        Quadra *q = (Quadra *)hash_getSlot(t->h, i);
        if (q != NULL)
            quadra_destruir(&q);
    }
    hash_destruir(&t->h);
    free(t);
    *tabela = NULL;
}

Quadra *quadra_criar(char *cep, double x, double y, double w, double h) {
    Quadra *q = malloc(sizeof(Quadra));
    if (q == NULL)
        return NULL;
    q->cep   = newString(cep);
    q->x     = x;
    q->y     = y;
    q->w     = w;
    q->h     = h;
    q->sw    = 0.0;
    q->cfill = NULL;
    q->cstrk = NULL;
    return q;
}

void quadra_definirCores(Quadra *q, double sw, char *cfill, char *cstrk) {
    if (q == NULL)
        return;
    q->sw = sw;
    free(q->cfill);
    free(q->cstrk);
    q->cfill = newString(cfill);
    q->cstrk = newString(cstrk);
}

void quadra_destruir(Quadra **q) {
    if (q == NULL || *q == NULL)
        return;
    free((*q)->cep);
    free((*q)->cfill);
    free((*q)->cstrk);
    free(*q);
    *q = NULL;
}

void tabelaQuadra_inserir(TabelaQuadra *tabela, Quadra *q) {
    if (tabela == NULL || q == NULL)
        return;
    hash_inserir(tabela->h, q->cep, q);
}

Quadra *tabelaQuadra_buscar(TabelaQuadra *tabela, char *cep) {
    if (tabela == NULL || cep == NULL)
        return NULL;
    return (Quadra *)hash_buscar(tabela->h, cep);
}

int tabelaQuadra_capacidade(TabelaQuadra *tabela) {
    return tabela ? hash_capacidade(tabela->h) : 0;
}

Quadra *tabelaQuadra_getSlot(TabelaQuadra *tabela, int i) {
    if (tabela == NULL)
        return NULL;
    return (Quadra *)hash_getSlot(tabela->h, i);
}

char *quadra_getCep(Quadra *q)   { return q ? q->cep   : NULL; }
double quadra_getX(Quadra *q)    { return q ? q->x     : 0.0;  }
double quadra_getY(Quadra *q)    { return q ? q->y     : 0.0;  }
double quadra_getW(Quadra *q)    { return q ? q->w     : 0.0;  }
double quadra_getH(Quadra *q)    { return q ? q->h     : 0.0;  }
double quadra_getSw(Quadra *q)   { return q ? q->sw    : 0.0;  }
char *quadra_getCfill(Quadra *q) { return q ? q->cfill : NULL; }
char *quadra_getCstrk(Quadra *q) { return q ? q->cstrk : NULL; }

void quadra_calcularPosicao(Quadra *q, char face, double num, double *px, double *py) {
    if (q == NULL || px == NULL || py == NULL)
        return;
    switch (face) {
        case 'S': *px = q->x + num;  *py = q->y;        break;
        case 'N': *px = q->x + num;  *py = q->y + q->h; break;
        case 'L': *px = q->x;        *py = q->y + num;  break;
        case 'O': *px = q->x + q->w; *py = q->y + num;  break;
    }
}
