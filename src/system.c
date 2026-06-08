#include "system.h"
#include <stdlib.h>
#include <string.h>
#include <float.h>

#define MARGEM 50.0

struct Sistema {
    TabelaQuadra *tabela;
    Quadra **quadras;
    int nquadras, capquadras;
    Grafo *grafo;
    double min_x, min_y, max_x, max_y;
    double dx, dy;
    FILE *svg;
    FILE *txt;
};

static void atualizar_bbox(Sistema *s, double x, double y) {
    if (x < s->min_x) s->min_x = x;
    if (y < s->min_y) s->min_y = y;
    if (x > s->max_x) s->max_x = x;
    if (y > s->max_y) s->max_y = y;
}

Sistema *sistema_criar(void) {
    Sistema *s = malloc(sizeof(Sistema));
    if (s == NULL)
        return NULL;
    s->tabela     = tabelaQuadra_criar(256);
    s->quadras    = malloc(16 * sizeof(Quadra *));
    s->nquadras   = 0;
    s->capquadras = 16;
    s->grafo      = NULL;
    s->min_x      = DBL_MAX;
    s->min_y      = DBL_MAX;
    s->max_x      = -DBL_MAX;
    s->max_y      = -DBL_MAX;
    s->dx         = 0.0;
    s->dy         = 0.0;
    s->svg        = NULL;
    s->txt        = NULL;
    return s;
}

void sistema_destruir(Sistema **s) {
    if (s == NULL || *s == NULL)
        return;
    Sistema *sys = *s;
    tabelaQuadra_destruir(&sys->tabela);
    free(sys->quadras);
    grafo_destruir(&sys->grafo);
    sistema_fecharSaidas(sys);
    free(sys);
    *s = NULL;
}

void sistema_lerGeo(Sistema *s, char *caminho) {
    if (s == NULL || caminho == NULL)
        return;
    FILE *f = fopen(caminho, "r");
    if (f == NULL)
        return;

    double sw = 1.0;
    char cfill[64] = "white";
    char cstrk[64] = "black";
    char cmd[16];

    while (fscanf(f, "%15s", cmd) == 1) {
        if (strcmp(cmd, "cq") == 0) {
            char sw_str[32];
            fscanf(f, "%31s %63s %63s", sw_str, cfill, cstrk);
            sw = atof(sw_str);
        } else if (strcmp(cmd, "q") == 0) {
            char cep[64];
            double x, y, w, h;
            fscanf(f, "%63s %lf %lf %lf %lf", cep, &x, &y, &w, &h);
            Quadra *q = quadra_criar(cep, x, y, w, h);
            quadra_definirCores(q, sw, cfill, cstrk);
            tabelaQuadra_inserir(s->tabela, q);
            if (s->nquadras == s->capquadras) {
                s->capquadras *= 2;
                s->quadras = realloc(s->quadras, s->capquadras * sizeof(Quadra *));
            }
            s->quadras[s->nquadras++] = q;
            atualizar_bbox(s, x - w, y - h);
            atualizar_bbox(s, x, y);
        }
    }
    fclose(f);
}

void sistema_lerVia(Sistema *s, char *caminho) {
    if (s == NULL || caminho == NULL)
        return;
    FILE *f = fopen(caminho, "r");
    if (f == NULL)
        return;

    int nv;
    fscanf(f, "%d", &nv);
    if (s->grafo == NULL)
        s->grafo = grafo_criar(nv);

    char cmd[4];
    while (fscanf(f, "%3s", cmd) == 1) {
        if (strcmp(cmd, "v") == 0) {
            char id[256];
            double x, y;
            fscanf(f, "%255s %lf %lf", id, &x, &y);
            grafo_inserirVertice(s->grafo, id, x, y);
            atualizar_bbox(s, x, y);
        } else if (strcmp(cmd, "e") == 0) {
            char ori[256], dst[256], ldir[64], lesq[64], nome[256];
            double cmp, vm;
            fscanf(f, "%255s %255s %63s %63s %lf %lf %255s",
                   ori, dst, ldir, lesq, &cmp, &vm, nome);
            grafo_inserirAresta(s->grafo, ori, dst, ldir, lesq, cmp, vm, nome);
        }
    }
    fclose(f);
}

void sistema_abrirSaidas(Sistema *s, char *caminho_svg, char *caminho_txt) {
    if (s == NULL || caminho_svg == NULL || caminho_txt == NULL)
        return;
    if (s->max_x >= s->min_x) {
        s->dx = -s->min_x + MARGEM;
        s->dy = -s->min_y + MARGEM;
    }
    s->svg = svg_criar(caminho_svg, sistema_getLargura(s), sistema_getAltura(s));
    s->txt = fopen(caminho_txt, "w");
}

void sistema_fecharSaidas(Sistema *s) {
    if (s == NULL)
        return;
    if (s->svg != NULL) {
        svg_fechar(s->svg);
        s->svg = NULL;
    }
    if (s->txt != NULL) {
        fclose(s->txt);
        s->txt = NULL;
    }
}

void sistema_desenharMapa(Sistema *s) {
    if (s == NULL || s->svg == NULL)
        return;

    for (int i = 0; i < s->nquadras; i++) {
        Quadra *q = s->quadras[i];
        svg_retangulo(s->svg,
                      quadra_getX(q) - quadra_getW(q) + s->dx,
                      quadra_getY(q) - quadra_getH(q) + s->dy,
                      quadra_getW(q), quadra_getH(q),
                      quadra_getCfill(q), quadra_getCstrk(q), quadra_getSw(q));
    }

    if (s->grafo == NULL)
        return;

    int nv = grafo_nVertices(s->grafo);
    for (int i = 0; i < nv; i++) {
        Vertice *u = grafo_getVertice(s->grafo, i);
        for (Aresta *a = grafo_primeiraAresta(s->grafo, vertice_getId(u)); a != NULL; a = aresta_proxima(a)) {
            Vertice *v = grafo_buscarVertice(s->grafo, aresta_getDst(a));
            if (v == NULL)
                continue;
            svg_linha(s->svg,
                      vertice_getX(u) + s->dx, vertice_getY(u) + s->dy,
                      vertice_getX(v) + s->dx, vertice_getY(v) + s->dy,
                      "gray", 0.5);
        }
    }

    for (int i = 0; i < nv; i++) {
        Vertice *v = grafo_getVertice(s->grafo, i);
        svg_circulo(s->svg,
                    vertice_getX(v) + s->dx, vertice_getY(v) + s->dy,
                    3.0, "orange", "none", 0.0);
    }
}

Grafo        *sistema_getGrafo(Sistema *s)        { return s ? s->grafo  : NULL; }
TabelaQuadra *sistema_getTabelaQuadra(Sistema *s) { return s ? s->tabela : NULL; }
FILE         *sistema_getSvg(Sistema *s)           { return s ? s->svg   : NULL; }
FILE         *sistema_getTxt(Sistema *s)           { return s ? s->txt   : NULL; }

double sistema_getLargura(Sistema *s) {
    if (s == NULL || s->max_x < s->min_x)
        return 800.0;
    return s->max_x - s->min_x + 2 * MARGEM;
}

double sistema_getAltura(Sistema *s) {
    if (s == NULL || s->max_y < s->min_y)
        return 600.0;
    return s->max_y - s->min_y + 2 * MARGEM;
}
