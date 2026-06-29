#include "quadra.h"
#include <stdlib.h>

struct Quadra {
    char *cep;
    double x, y, w, h;
    double sw;
    char *cfill;
    char *cstrk;
};

struct No {
    Quadra *quadra;
    struct No *prox;
};

struct TabelaQuadra {
    int capacidade;
    struct No **buckets;
};

static int hash(char *cep, int capacidade) {
    unsigned int h = 0;
    for (int i = 0; cep[i]; i++)
        h = h * 31 + (unsigned char)cep[i];
    return (int)(h % (unsigned int)capacidade);
}

TabelaQuadra *tabelaQuadra_criar(int capacidade) {
    TabelaQuadra *t = malloc(sizeof(TabelaQuadra));
    if (t == NULL)
        return NULL;
    t->capacidade = capacidade;
    t->buckets = calloc(capacidade, sizeof(struct No *));
    if (t->buckets == NULL) {
        free(t);
        return NULL;
    }
    return t;
}

void tabelaQuadra_destruir(TabelaQuadra **tabela) {
    if (tabela == NULL || *tabela == NULL)
        return;
    TabelaQuadra *t = *tabela;
    for (int i = 0; i < t->capacidade; i++) {
        struct No *no = t->buckets[i];
        while (no != NULL) {
            struct No *prox = no->prox;
            quadra_destruir(&no->quadra);
            free(no);
            no = prox;
        }
    }
    free(t->buckets);
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
    struct No *no = malloc(sizeof(struct No));
    if (no == NULL)
        return;
    int i = hash(q->cep, tabela->capacidade);
    no->quadra = q;
    no->prox   = tabela->buckets[i];
    tabela->buckets[i] = no;
}

Quadra *tabelaQuadra_buscar(TabelaQuadra *tabela, char *cep) {
    if (tabela == NULL || cep == NULL)
        return NULL;
    int i = hash(cep, tabela->capacidade);
    struct No *no = tabela->buckets[i];
    while (no != NULL) {
        if (strcmp(no->quadra->cep, cep) == 0)
            return no->quadra;
        no = no->prox;
    }
    return NULL;
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
        case 'S': *px = q->x + num;       *py = q->y;       break;
        case 'N': *px = q->x + num;       *py = q->y + q->h; break;
        case 'L': *px = q->x;             *py = q->y + num;  break;
        case 'O': *px = q->x + q->w;      *py = q->y + num;  break;
    }
}
