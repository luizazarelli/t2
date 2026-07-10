#include "graph.h"
#include "hash.h"
#include <stdlib.h>
#include <string.h>

struct Aresta {
    char *ori;
    char *dst;
    char *ldir;
    char *lesq;
    char *nome;
    double cmp;
    double vm;
    bool ativo;
    struct Aresta *prox;
};

struct Vertice {
    char *id;
    double x, y;
    int idx;
    Aresta *arestas;
};

struct Grafo {
    int n;
    int cap;
    Vertice **arr;
    Hash *htab;
};

Grafo *grafo_criar(int capacidade) {
    Grafo *g = malloc(sizeof(Grafo));
    if (g == NULL)
        return NULL;
    g->n   = 0;
    g->cap = capacidade > 0 ? capacidade : 16;
    g->arr  = calloc(g->cap, sizeof(Vertice *));
    g->htab = hash_criar(g->cap * 2 + 1);
    if (g->arr == NULL || g->htab == NULL) {
        free(g->arr);
        hash_destruir(&g->htab);
        free(g);
        return NULL;
    }
    return g;
}

void grafo_destruir(Grafo **g) {
    if (g == NULL || *g == NULL)
        return;
    Grafo *gr = *g;
    for (int i = 0; i < gr->n; i++) {
        Vertice *v = gr->arr[i];
        Aresta *a = v->arestas;
        while (a != NULL) {
            Aresta *prox = a->prox;
            free(a->ori);
            free(a->dst);
            free(a->ldir);
            free(a->lesq);
            free(a->nome);
            free(a);
            a = prox;
        }
        free(v->id);
        free(v);
    }
    hash_destruir(&gr->htab);
    free(gr->arr);
    free(gr);
    *g = NULL;
}

void grafo_inserirVertice(Grafo *g, char *id, double x, double y) {
    if (g == NULL || id == NULL)
        return;
    if (grafo_buscarVertice(g, id) != NULL)
        return;
    if (g->n == g->cap) {
        int novocap = g->cap * 2;
        Vertice **novo = realloc(g->arr, novocap * sizeof(Vertice *));
        if (novo == NULL)
            return;
        g->arr = novo;
        g->cap = novocap;
    }
    Vertice *v = malloc(sizeof(Vertice));
    if (v == NULL)
        return;
    v->id     = newString(id);
    v->x      = x;
    v->y      = y;
    v->idx    = g->n;
    v->arestas = NULL;
    g->arr[g->n++] = v;
    hash_inserir(g->htab, id, v);
}

Vertice *grafo_buscarVertice(Grafo *g, char *id) {
    if (g == NULL || id == NULL)
        return NULL;
    return (Vertice *)hash_buscar(g->htab, id);
}

int grafo_nVertices(Grafo *g) {
    return g ? g->n : 0;
}

Vertice *grafo_getVertice(Grafo *g, int i) {
    if (g == NULL || i < 0 || i >= g->n)
        return NULL;
    return g->arr[i];
}

void grafo_inserirAresta(Grafo *g, char *ori, char *dst,
                         char *ldir, char *lesq,
                         double cmp, double vm, char *nome) {
    if (g == NULL || ori == NULL || dst == NULL)
        return;
    Vertice *vo = grafo_buscarVertice(g, ori);
    if (vo == NULL || grafo_buscarVertice(g, dst) == NULL)
        return;
    Aresta *a = malloc(sizeof(Aresta));
    if (a == NULL)
        return;
    a->ori   = newString(ori);
    a->dst   = newString(dst);
    a->ldir  = newString(ldir);
    a->lesq  = newString(lesq);
    a->nome  = newString(nome);
    a->cmp   = cmp;
    a->vm    = vm;
    a->ativo = true;
    a->prox  = vo->arestas;
    vo->arestas = a;
}

Aresta *grafo_primeiraAresta(Grafo *g, char *id) {
    if (g == NULL || id == NULL)
        return NULL;
    Vertice *v = grafo_buscarVertice(g, id);
    return v ? v->arestas : NULL;
}

Aresta *aresta_proxima(Aresta *a) {
    return a ? a->prox : NULL;
}

char   *vertice_getId(Vertice *v)  { return v ? v->id  : NULL; }
double  vertice_getX(Vertice *v)   { return v ? v->x   : 0.0;  }
double  vertice_getY(Vertice *v)   { return v ? v->y   : 0.0;  }
int     vertice_getIdx(Vertice *v) { return v ? v->idx : -1;   }

char   *aresta_getOri(Aresta *a)  { return a ? a->ori  : NULL; }
char   *aresta_getDst(Aresta *a)  { return a ? a->dst  : NULL; }
char   *aresta_getNome(Aresta *a) { return a ? a->nome : NULL; }
char   *aresta_getLdir(Aresta *a) { return a ? a->ldir : NULL; }
char   *aresta_getLesq(Aresta *a) { return a ? a->lesq : NULL; }
double  aresta_getCmp(Aresta *a)  { return a ? a->cmp  : 0.0;  }
double  aresta_getVm(Aresta *a)   { return a ? a->vm   : 0.0;  }

void aresta_setVm(Aresta *a, double vm) {
    if (a != NULL)
        a->vm = vm;
}

bool aresta_isAtiva(Aresta *a) {
    return a ? a->ativo : false;
}

void aresta_setAtiva(Aresta *a, bool ativa) {
    if (a != NULL)
        a->ativo = ativa;
}
