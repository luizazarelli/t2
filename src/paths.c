#include "paths.h"
#include <stdlib.h>
#include <float.h>

struct Caminho {
    bool existe;
    double custo;
    int n;
    Vertice **path;
};

struct AGM {
    int n;
    int cap;
    Aresta **arestas;
};

struct CompConexos {
    int nv;
    int *comp;
    int ncomp;
};

/* --- dijkstra --- */

static double peso_dist(Aresta *a)  { return aresta_getCmp(a); }
static double peso_tempo(Aresta *a) { return aresta_getCmp(a) / aresta_getVm(a); }

static Caminho *dijkstra(Grafo *g, char *ori, char *dst, double (*peso)(Aresta *)) {
    Vertice *vsrc = grafo_buscarVertice(g, ori);
    Vertice *vdst = grafo_buscarVertice(g, dst);
    if (vsrc == NULL || vdst == NULL)
        return NULL;

    int nv      = grafo_nVertices(g);
    int src     = vertice_getIdx(vsrc);
    int dst_idx = vertice_getIdx(vdst);

    double *dist = malloc(nv * sizeof(double));
    int    *pred = malloc(nv * sizeof(int));
    bool   *vis  = calloc(nv, sizeof(bool));
    if (dist == NULL || pred == NULL || vis == NULL) {
        free(dist); free(pred); free(vis);
        return NULL;
    }

    for (int i = 0; i < nv; i++) {
        dist[i] = DBL_MAX;
        pred[i] = -1;
    }
    dist[src] = 0.0;

    for (int iter = 0; iter < nv; iter++) {
        int u = -1;
        for (int i = 0; i < nv; i++) {
            if (!vis[i] && (u == -1 || dist[i] < dist[u]))
                u = i;
        }
        if (u == -1 || dist[u] == DBL_MAX)
            break;
        vis[u] = true;

        Vertice *vu = grafo_getVertice(g, u);
        for (Aresta *a = grafo_primeiraAresta(g, vertice_getId(vu)); a != NULL; a = aresta_proxima(a)) {
            if (!aresta_isAtiva(a))
                continue;
            Vertice *vv = grafo_buscarVertice(g, aresta_getDst(a));
            if (vv == NULL)
                continue;
            int v    = vertice_getIdx(vv);
            double w = peso(a);
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                pred[v] = u;
            }
        }
    }

    Caminho *c = malloc(sizeof(Caminho));
    if (c == NULL) {
        free(dist); free(pred); free(vis);
        return NULL;
    }

    if (dist[dst_idx] == DBL_MAX) {
        c->existe = false;
        c->custo  = 0.0;
        c->n      = 0;
        c->path   = NULL;
    } else {
        c->existe = true;
        c->custo  = dist[dst_idx];

        int len = 0;
        for (int cur = dst_idx; cur != -1; cur = pred[cur])
            len++;

        c->path = malloc(len * sizeof(Vertice *));
        c->n    = len;
        if (c->path != NULL) {
            int cur = dst_idx;
            for (int i = len - 1; i >= 0; i--) {
                c->path[i] = grafo_getVertice(g, cur);
                cur = pred[cur];
            }
        }
    }

    free(dist); free(pred); free(vis);
    return c;
}

Caminho *caminho_calcularDistancia(Grafo *g, char *ori, char *dst) {
    return dijkstra(g, ori, dst, peso_dist);
}

Caminho *caminho_calcularTempo(Grafo *g, char *ori, char *dst) {
    return dijkstra(g, ori, dst, peso_tempo);
}

void caminho_destruir(Caminho **c) {
    if (c == NULL || *c == NULL)
        return;
    free((*c)->path);
    free(*c);
    *c = NULL;
}

bool    caminho_existePercurso(Caminho *c) { return c ? c->existe : false; }
double  caminho_getCusto(Caminho *c)       { return c ? c->custo  : 0.0;  }
int     caminho_nVertices(Caminho *c)      { return c ? c->n      : 0;    }

Vertice *caminho_getVertice(Caminho *c, int i) {
    if (c == NULL || !c->existe || i < 0 || i >= c->n)
        return NULL;
    return c->path[i];
}

/* --- kruskal (agm) --- */

typedef struct {
    Aresta *a;
    int u, v;
    double cmp;
} EntradaAresta;

static int cmp_aresta(const void *x, const void *y) {
    double da = ((EntradaAresta *)x)->cmp;
    double db = ((EntradaAresta *)y)->cmp;
    return (da > db) - (da < db);
}

static int find(int *parent, int x) {
    while (parent[x] != x)
        x = parent[x] = parent[parent[x]];
    return x;
}

static bool unite(int *parent, int *rank, int x, int y) {
    x = find(parent, x);
    y = find(parent, y);
    if (x == y) return false;
    if (rank[x] < rank[y]) { int t = x; x = y; y = t; }
    parent[y] = x;
    if (rank[x] == rank[y]) rank[x]++;
    return true;
}

AGM *agm_calcular(Grafo *g, double limiar) {
    if (g == NULL)
        return NULL;
    int nv = grafo_nVertices(g);

    int ne = 0, necap = 64;
    EntradaAresta *edges = malloc(necap * sizeof(EntradaAresta));
    if (edges == NULL)
        return NULL;

    for (int i = 0; i < nv; i++) {
        Vertice *vu = grafo_getVertice(g, i);
        for (Aresta *a = grafo_primeiraAresta(g, vertice_getId(vu)); a != NULL; a = aresta_proxima(a)) {
            if (!aresta_isAtiva(a) || aresta_getVm(a) >= limiar)
                continue;
            Vertice *vv = grafo_buscarVertice(g, aresta_getDst(a));
            if (vv == NULL)
                continue;
            if (ne == necap) {
                necap *= 2;
                EntradaAresta *tmp = realloc(edges, necap * sizeof(EntradaAresta));
                if (tmp == NULL) { free(edges); return NULL; }
                edges = tmp;
            }
            edges[ne].a   = a;
            edges[ne].u   = i;
            edges[ne].v   = vertice_getIdx(vv);
            edges[ne].cmp = aresta_getCmp(a);
            ne++;
        }
    }

    qsort(edges, ne, sizeof(EntradaAresta), cmp_aresta);

    int *parent = malloc(nv * sizeof(int));
    int *rank   = calloc(nv, sizeof(int));
    for (int i = 0; i < nv; i++) parent[i] = i;

    AGM *agm     = malloc(sizeof(AGM));
    agm->n       = 0;
    agm->cap     = nv > 1 ? nv - 1 : 1;
    agm->arestas = malloc(agm->cap * sizeof(Aresta *));

    for (int i = 0; i < ne && agm->n < nv - 1; i++) {
        if (unite(parent, rank, edges[i].u, edges[i].v))
            agm->arestas[agm->n++] = edges[i].a;
    }

    free(edges); free(parent); free(rank);
    return agm;
}

void agm_destruir(AGM **a) {
    if (a == NULL || *a == NULL)
        return;
    free((*a)->arestas);
    free(*a);
    *a = NULL;
}

int agm_nArestas(AGM *a) { return a ? a->n : 0; }

Aresta *agm_getAresta(AGM *a, int i) {
    if (a == NULL || i < 0 || i >= a->n)
        return NULL;
    return a->arestas[i];
}

/* --- tarjan (scc) --- */

typedef struct {
    int  timer;
    int *dfs_num;
    int *low;
    bool *on_stk;
    int *stk;
    int  stk_top;
    int *comp;
    int  ncomp;
} TarjanCtx;

static void tarjan_dfs(Grafo *g, int u, TarjanCtx *ctx) {
    ctx->dfs_num[u] = ctx->low[u] = ctx->timer++;
    ctx->stk[ctx->stk_top++] = u;
    ctx->on_stk[u] = true;

    Vertice *vu = grafo_getVertice(g, u);
    for (Aresta *a = grafo_primeiraAresta(g, vertice_getId(vu)); a != NULL; a = aresta_proxima(a)) {
        if (!aresta_isAtiva(a))
            continue;
        Vertice *vv = grafo_buscarVertice(g, aresta_getDst(a));
        if (vv == NULL)
            continue;
        int v = vertice_getIdx(vv);
        if (ctx->dfs_num[v] == -1) {
            tarjan_dfs(g, v, ctx);
            if (ctx->low[v] < ctx->low[u]) ctx->low[u] = ctx->low[v];
        } else if (ctx->on_stk[v]) {
            if (ctx->dfs_num[v] < ctx->low[u]) ctx->low[u] = ctx->dfs_num[v];
        }
    }

    if (ctx->low[u] == ctx->dfs_num[u]) {
        int w;
        do {
            w = ctx->stk[--ctx->stk_top];
            ctx->on_stk[w] = false;
            ctx->comp[w]   = ctx->ncomp;
        } while (w != u);
        ctx->ncomp++;
    }
}

CompConexos *scc_calcular(Grafo *g) {
    if (g == NULL)
        return NULL;
    int nv = grafo_nVertices(g);

    TarjanCtx ctx;
    ctx.timer   = 0;
    ctx.stk_top = 0;
    ctx.ncomp   = 0;
    ctx.dfs_num = malloc(nv * sizeof(int));
    ctx.low     = malloc(nv * sizeof(int));
    ctx.on_stk  = calloc(nv, sizeof(bool));
    ctx.stk     = malloc(nv * sizeof(int));
    ctx.comp    = malloc(nv * sizeof(int));

    if (ctx.dfs_num == NULL || ctx.low == NULL || ctx.on_stk == NULL ||
        ctx.stk == NULL || ctx.comp == NULL) {
        free(ctx.dfs_num); free(ctx.low); free(ctx.on_stk);
        free(ctx.stk); free(ctx.comp);
        return NULL;
    }

    for (int i = 0; i < nv; i++) {
        if (ctx.dfs_num[i] == -1)
            tarjan_dfs(g, i, &ctx);
    }

    CompConexos *s = malloc(sizeof(CompConexos));
    if (s == NULL) {
        free(ctx.dfs_num); free(ctx.low); free(ctx.on_stk);
        free(ctx.stk); free(ctx.comp);
        return NULL;
    }

    s->nv    = nv;
    s->comp  = ctx.comp;
    s->ncomp = ctx.ncomp;

    free(ctx.dfs_num); free(ctx.low); free(ctx.on_stk); free(ctx.stk);
    return s;
}

void scc_destruir(CompConexos **s) {
    if (s == NULL || *s == NULL)
        return;
    free((*s)->comp);
    free(*s);
    *s = NULL;
}

int scc_nComponentes(CompConexos *s) { return s ? s->ncomp : 0; }

int scc_getComponente(CompConexos *s, int idx) {
    if (s == NULL || idx < 0 || idx >= s->nv)
        return -1;
    return s->comp[idx];
}
