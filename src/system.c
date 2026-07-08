#include "system.h"
#include <stdlib.h>
#include <string.h>
#include <float.h>
#define _USE_MATH_DEFINES
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define MARGEM 9.0

struct Sistema {
    TabelaQuadra *tabela;
    Grafo *grafo;
    double min_x, min_y, max_x, max_y;
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
    s->tabela = tabelaQuadra_criar(256);
    s->grafo  = NULL;
    s->min_x      = DBL_MAX;
    s->min_y      = DBL_MAX;
    s->max_x      = -DBL_MAX;
    s->max_y      = -DBL_MAX;
    s->svg        = NULL;
    s->txt        = NULL;
    return s;
}

void sistema_destruir(Sistema **s) {
    if (s == NULL || *s == NULL)
        return;
    Sistema *sys = *s;
    tabelaQuadra_destruir(&sys->tabela);
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
            atualizar_bbox(s, x, y);
            atualizar_bbox(s, x + w, y + h);
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
    double vx = (s->max_x >= s->min_x) ? s->min_x - MARGEM : 0.0;
    double vy = (s->max_y >= s->min_y) ? s->min_y - MARGEM : 0.0;
    double vw = (s->max_x >= s->min_x) ? s->max_x - s->min_x + 2.0 * MARGEM : 800.0;
    double vh = (s->max_y >= s->min_y) ? s->max_y - s->min_y + 2.0 * MARGEM : 600.0;
    s->svg = svg_criar(caminho_svg, vx, vy, vw, vh);
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

static void desenhar_aresta_via(FILE *svg, double x1, double y1,
                                double x2, double y2,
                                char *ldir, char *lesq,
                                char *ori_id, char *dst_id) {
    double dx = x2 - x1, dy = y2 - y1;
    double len = sqrt(dx*dx + dy*dy);
    if (len < 1e-9) return;

    double ux = dx/len, uy = dy/len;
    double px = uy * 4.0, py = -ux * 4.0;

    double sx = x1 + ux*4.0, sy = y1 + uy*4.0;
    double ex = x2 - ux*4.0, ey = y2 - uy*4.0;
    double mx = (sx+ex)/2.0 + px, my = (sy+ey)/2.0 + py;

    double angle = atan2(dy, dx) * 180.0 / M_PI;

    fprintf(svg,
        "   <svg:path id=\"%s_%s\" d=\"M%.6f,%.6f L%.6f,%.6f L%.6f,%.6f\" "
        "stroke=\"black\" fill=\"none\" stroke-width=\"1\" marker-end=\"url(#mArrow)\" />\n",
        ori_id, dst_id, sx, sy, mx, my, ex, ey);

    if (ldir)
        fprintf(svg,
            "   <svg:text x=\"%.6f\" y=\"%.6f\" fill=\"red\" font-size=\"4\" "
            "text-anchor=\"middle\" transform=\"rotate(%.0f %.6f %.6f)\">%s</svg:text>\n",
            mx, my, angle, mx, my, ldir);

    if (lesq) {
        double lx = mx - 2.0*px, ly = my - 2.0*py;
        fprintf(svg,
            "   <svg:text x=\"%.6f\" y=\"%.6f\" fill=\"green\" font-size=\"4\" "
            "text-anchor=\"middle\" transform=\"rotate(%.0f %.6f %.6f)\">%s</svg:text>\n",
            lx, ly, angle, lx, ly, lesq);
    }
}

void sistema_desenharMapa(Sistema *s) {
    if (s == NULL || s->svg == NULL)
        return;

    fprintf(s->svg,
        "<svg:defs>\n"
        "     <svg:marker id=\"mArrow\" markerWidth=\"4\" markerHeight=\"4\" "
        "refX=\"4.000000\" refY=\"2.000000\" orient=\"auto\">\n"
        "       <svg:path d=\"M0,0 L0,4.000000 L4.000000,2.000000 z\" "
        "style=\"fill: #000000;\" />\n"
        "     </svg:marker> \n"
        "\n"
        "   </svg:defs><svg:g id=\"via\">\n");

    if (s->grafo != NULL) {
        int nv = grafo_nVertices(s->grafo);

        /* 1. vertices (circulo + texto) */
        for (int i = 0; i < nv; i++) {
            Vertice *v = grafo_getVertice(s->grafo, i);
            double vx = vertice_getX(v);
            double vy = vertice_getY(v);
            char *vid = vertice_getId(v);
            fprintf(s->svg,
                "   <svg:circle id=\"%s\" cx=\"%.6f\" cy=\"%.6f\" r=\"4.000000\" "
                "fill=\"blue\" stroke=\"black\" fill-opacity=\"0.5\" />\n",
                vid, vx, vy);
            fprintf(s->svg, "\n");
            fprintf(s->svg,
                "   <svg:text x=\"%.6f\" y=\"%.6f\" fill=\"blue\" font-size=\"4\" "
                "text-anchor=\"middle\" transform=\"rotate(0 %.6f %.6f)\">%s</svg:text>\n",
                vx, vy, vx, vy, vid);
        }

        /* 2. arestas com V-shape, seta e rotulos ldir/lesq */
        for (int i = 0; i < nv; i++) {
            Vertice *u = grafo_getVertice(s->grafo, i);
            double ux = vertice_getX(u);
            double uy = vertice_getY(u);
            for (Aresta *a = grafo_primeiraAresta(s->grafo, vertice_getId(u)); a != NULL; a = aresta_proxima(a)) {
                Vertice *v = grafo_buscarVertice(s->grafo, aresta_getDst(a));
                if (v == NULL) continue;
                desenhar_aresta_via(s->svg, ux, uy, vertice_getX(v), vertice_getY(v),
                                    aresta_getLdir(a), aresta_getLesq(a),
                                    vertice_getId(u), aresta_getDst(a));
            }
        }
    }

    /* 3. quadras (ret + texto) */
    int ncap = tabelaQuadra_capacidade(s->tabela);
    for (int i = 0; i < ncap; i++) {
        Quadra *q = tabelaQuadra_getSlot(s->tabela, i);
        if (q == NULL) continue;
        double rx = quadra_getX(q);
        double ry = quadra_getY(q);
        double rw = quadra_getW(q);
        double rh = quadra_getH(q);
        fprintf(s->svg,
            "   <svg:rect id=\"%s\" x=\"%.6f\" y=\"%.6f\" width=\"%.6f\" height=\"%.6f\" "
            "fill=\"%s\" stroke=\"%s\" fill-opacity=\"0.8\" stroke-width=\"%.1fpx\" />\n",
            quadra_getCep(q), rx, ry, rw, rh,
            quadra_getCfill(q), quadra_getCstrk(q), quadra_getSw(q));
        fprintf(s->svg, "\n");
        fprintf(s->svg,
            "   <svg:text x=\"%.6f\" y=\"%.6f\" fill=\"%s\" stroke=\"black\" font-size=\"12\">%s</svg:text>\n",
            rx + 5.0, ry + 12.0, quadra_getCstrk(q), quadra_getCep(q));
    }
}

Grafo        *sistema_getGrafo(Sistema *s)        { return s ? s->grafo  : NULL; }
TabelaQuadra *sistema_getTabelaQuadra(Sistema *s) { return s ? s->tabela : NULL; }
FILE         *sistema_getSvg(Sistema *s)           { return s ? s->svg   : NULL; }
FILE         *sistema_getTxt(Sistema *s)           { return s ? s->txt   : NULL; }

double sistema_getLargura(Sistema *s) {
    if (s == NULL || s->max_x < s->min_x)
        return 800.0;
    return s->max_x - s->min_x + 2.0 * MARGEM;
}

double sistema_getAltura(Sistema *s) {
    if (s == NULL || s->max_y < s->min_y)
        return 600.0;
    return s->max_y - s->min_y + 2.0 * MARGEM;
}

double sistema_getDx(Sistema *s) { (void)s; return 0.0; }
double sistema_getDy(Sistema *s) { (void)s; return 0.0; }

double sistema_getViewMinX(Sistema *s) {
    return (s && s->max_x >= s->min_x) ? s->min_x - MARGEM : 0.0;
}

double sistema_getViewMinY(Sistema *s) {
    return (s && s->max_y >= s->min_y) ? s->min_y - MARGEM : 0.0;
}
