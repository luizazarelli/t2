#include "qry.h"
#include "paths.h"
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>

#define N_REGS 11

static const char *CORES_SCC[] = {
    "#FF6B6B", "#4ECDC4", "#45B7D1", "#96CEB4",
    "#FECA57", "#FF9FF3", "#54A0FF"
};
#define N_CORES 7

struct QryEstado {
    double x[N_REGS];
    double y[N_REGS];
    bool   valido[N_REGS];
};

QryEstado *qry_criar(void) {
    QryEstado *q = malloc(sizeof(QryEstado));
    if (q == NULL)
        return NULL;
    for (int i = 0; i < N_REGS; i++)
        q->valido[i] = false;
    return q;
}

void qry_destruir(QryEstado **q) {
    if (q == NULL || *q == NULL)
        return;
    free(*q);
    *q = NULL;
}

/* --- helpers --- */

static int reg_idx(char *s) {
    if (s == NULL) return -1;
    int i = (*s == 'R' || *s == 'r') ? atoi(s + 1) : atoi(s);
    return (i >= 0 && i < N_REGS) ? i : -1;
}

static Vertice *vertice_mais_proximo(Grafo *g, double x, double y) {
    int nv = grafo_nVertices(g);
    Vertice *melhor = NULL;
    double melhor_d = DBL_MAX;
    for (int i = 0; i < nv; i++) {
        Vertice *v = grafo_getVertice(g, i);
        double dx = vertice_getX(v) - x;
        double dy = vertice_getY(v) - y;
        double d = dx * dx + dy * dy;
        if (d < melhor_d) { melhor_d = d; melhor = v; }
    }
    return melhor;
}

static Aresta *achar_aresta(Grafo *g, char *ori, char *dst) {
    for (Aresta *a = grafo_primeiraAresta(g, ori); a != NULL; a = aresta_proxima(a))
        if (strcmp(aresta_getDst(a), dst) == 0)
            return a;
    return NULL;
}

static const char *direcao(double dx, double dy) {
    if (fabs(dx) >= fabs(dy))
        return dx >= 0 ? "Leste" : "Oeste";
    return dy >= 0 ? "Sul" : "Norte";
}

/* --- comandos --- */

void qry_registrar(QryEstado *q, Sistema *s, int reg, char *cep, char face, double num) {
    if (q == NULL || s == NULL || reg < 0 || reg >= N_REGS)
        return;
    TabelaQuadra *t = sistema_getTabelaQuadra(s);
    Quadra *quadra = tabelaQuadra_buscar(t, cep);
    if (quadra == NULL)
        return;

    double px, py;
    quadra_calcularPosicao(quadra, face, num, &px, &py);
    q->x[reg]     = px;
    q->y[reg]     = py;
    q->valido[reg] = true;

    FILE *svg = sistema_getSvg(s);
    FILE *txt = sistema_getTxt(s);
    double dx  = sistema_getLargura(s) - (sistema_getAltura(s) - py);
    double dy  = sistema_getLargura(s);

    if (svg != NULL) {
        double altura = sistema_getAltura(s);
        double desl_x = px + (sistema_getLargura(s) - sistema_getLargura(s));
        fprintf(svg,
            "<line x1=\"%.2f\" y1=\"0\" x2=\"%.2f\" y2=\"%.2f\" "
            "stroke=\"red\" stroke-width=\"1\" stroke-dasharray=\"4,4\"/>\n",
            px, px, altura);
        fprintf(svg,
            "<text x=\"%.2f\" y=\"12\" font-size=\"10\" fill=\"red\" "
            "text-anchor=\"middle\">R%d</text>\n",
            px, reg);
        (void)dx; (void)dy; (void)desl_x;
    }
    if (txt != NULL)
        fprintf(txt, "R%d: (%.2f, %.2f)\n", reg, px, py);
}

void qry_mvm(Sistema *s, double v, double x, double y, double w, double h) {
    if (s == NULL)
        return;
    Grafo *g = sistema_getGrafo(s);
    if (g == NULL)
        return;
    double xmin = x - w, xmax = x;
    double ymin = y - h, ymax = y;
    int nv = grafo_nVertices(g);
    for (int i = 0; i < nv; i++) {
        Vertice *u = grafo_getVertice(g, i);
        for (Aresta *a = grafo_primeiraAresta(g, vertice_getId(u)); a != NULL; a = aresta_proxima(a)) {
            if (!aresta_isAtiva(a))
                continue;
            Vertice *vv = grafo_buscarVertice(g, aresta_getDst(a));
            if (vv == NULL)
                continue;
            double mx = (vertice_getX(u) + vertice_getX(vv)) / 2.0;
            double my = (vertice_getY(u) + vertice_getY(vv)) / 2.0;
            if (mx >= xmin && mx <= xmax && my >= ymin && my <= ymax)
                aresta_setVm(a, v);
        }
    }
}

void qry_regs(Sistema *s, double vl) {
    if (s == NULL)
        return;
    Grafo *g = sistema_getGrafo(s);
    if (g == NULL)
        return;

    int nv = grafo_nVertices(g);
    for (int i = 0; i < nv; i++) {
        Vertice *u = grafo_getVertice(g, i);
        for (Aresta *a = grafo_primeiraAresta(g, vertice_getId(u)); a != NULL; a = aresta_proxima(a))
            if (aresta_getVm(a) < vl)
                aresta_setAtiva(a, false);
    }

    CompConexos *scc = scc_calcular(g);
    int ncomp = scc_nComponentes(scc);

    FILE *txt = sistema_getTxt(s);
    if (txt != NULL)
        fprintf(txt, "Componentes fortemente conexos: %d\n", ncomp);

    FILE *svg = sistema_getSvg(s);
    if (svg != NULL) {
        for (int c = 0; c < ncomp; c++) {
            double xmin = DBL_MAX, ymin = DBL_MAX;
            double xmax = -DBL_MAX, ymax = -DBL_MAX;
            for (int i = 0; i < nv; i++) {
                if (scc_getComponente(scc, i) != c)
                    continue;
                Vertice *v = grafo_getVertice(g, i);
                double vx = vertice_getX(v);
                double vy = vertice_getY(v);
                if (vx < xmin) xmin = vx;
                if (vy < ymin) ymin = vy;
                if (vx > xmax) xmax = vx;
                if (vy > ymax) ymax = vy;
            }
            if (xmax < xmin)
                continue;
            double margem = 10.0;
            double larg = sistema_getLargura(s);
            double alt  = sistema_getAltura(s);
            double ox = larg - (sistema_getLargura(s));
            double oy = alt  - (sistema_getAltura(s));
            (void)ox; (void)oy;
            const char *cor = CORES_SCC[c % N_CORES];
            fprintf(svg,
                "<rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" "
                "fill=\"%s\" fill-opacity=\"0.5\" stroke=\"%s\" stroke-width=\"1\"/>\n",
                xmin - margem, ymin - margem,
                xmax - xmin + 2 * margem, ymax - ymin + 2 * margem,
                cor, cor);
        }
    }

    scc_destruir(&scc);
}

void qry_exp(Sistema *s, double vl) {
    if (s == NULL)
        return;
    Grafo *g = sistema_getGrafo(s);
    if (g == NULL)
        return;

    AGM *agm = agm_calcular(g, vl);
    int n = agm_nArestas(agm);

    FILE *svg = sistema_getSvg(s);
    for (int i = 0; i < n; i++) {
        Aresta *a = agm_getAresta(agm, i);
        aresta_setVm(a, aresta_getVm(a) * 1.5);
        if (svg != NULL) {
            Vertice *ori = grafo_buscarVertice(g, aresta_getOri(a));
            Vertice *dst = grafo_buscarVertice(g, aresta_getDst(a));
            if (ori != NULL && dst != NULL)
                svg_linha(svg,
                          vertice_getX(ori), vertice_getY(ori),
                          vertice_getX(dst), vertice_getY(dst),
                          "red", 3.0);
        }
    }
    agm_destruir(&agm);
}

static void descrever_percurso(FILE *txt, Grafo *g, Caminho *c, const char *tipo) {
    if (txt == NULL || c == NULL || !caminho_existePercurso(c))
        return;
    int n = caminho_nVertices(c);
    fprintf(txt, "Percurso %s:\n", tipo);
    int i = 0;
    while (i < n - 1) {
        Vertice *u = caminho_getVertice(c, i);
        Vertice *v = caminho_getVertice(c, i + 1);
        Aresta *a = achar_aresta(g, vertice_getId(u), vertice_getId(v));
        if (a == NULL) { i++; continue; }
        const char *rua = aresta_getNome(a);
        double dx = vertice_getX(v) - vertice_getX(u);
        double dy = vertice_getY(v) - vertice_getY(u);
        double dist = 0.0;
        int j = i;
        while (j < n - 1) {
            Vertice *uj = caminho_getVertice(c, j);
            Vertice *vj = caminho_getVertice(c, j + 1);
            Aresta *aj = achar_aresta(g, vertice_getId(uj), vertice_getId(vj));
            if (aj == NULL || strcmp(aresta_getNome(aj), rua) != 0)
                break;
            dist += aresta_getCmp(aj);
            j++;
        }
        fprintf(txt, "  Siga na direcao %s na %s por %.0f metros.\n",
                direcao(dx, dy), rua, dist);
        i = j;
    }
}

void qry_percurso(QryEstado *q, Sistema *s, int reg1, int reg2, char *cc, char *cr) {
    if (q == NULL || s == NULL)
        return;
    if (!qry_regValido(q, reg1) || !qry_regValido(q, reg2))
        return;

    Grafo *g = sistema_getGrafo(s);
    Vertice *vori = vertice_mais_proximo(g, q->x[reg1], q->y[reg1]);
    Vertice *vdst = vertice_mais_proximo(g, q->x[reg2], q->y[reg2]);
    if (vori == NULL || vdst == NULL)
        return;

    char *id_ori = vertice_getId(vori);
    char *id_dst = vertice_getId(vdst);

    Caminho *curto  = caminho_calcularDistancia(g, id_ori, id_dst);
    Caminho *rapido = caminho_calcularTempo(g, id_ori, id_dst);

    FILE *txt = sistema_getTxt(s);
    FILE *svg = sistema_getSvg(s);

    if (txt != NULL) {
        if (!caminho_existePercurso(curto) && !caminho_existePercurso(rapido)) {
            fprintf(txt, "Destino inacessivel.\n");
        } else {
            descrever_percurso(txt, g, curto,  "mais curto");
            descrever_percurso(txt, g, rapido, "mais rapido");
        }
    }

    if (svg != NULL) {
        static int id_percurso = 0;
        if (caminho_existePercurso(curto) && caminho_nVertices(curto) >= 2) {
            int n = caminho_nVertices(curto);
            double *xs = malloc(n * sizeof(double));
            double *ys = malloc(n * sizeof(double));
            for (int i = 0; i < n; i++) {
                xs[i] = vertice_getX(caminho_getVertice(curto, i));
                ys[i] = vertice_getY(caminho_getVertice(curto, i));
            }
            char id_path[32];
            snprintf(id_path, sizeof(id_path), "curto%d", id_percurso);
            svg_percursoAnimado(svg, id_path, xs, ys, n, cc, 2.0, cc, 6.0);
            free(xs); free(ys);
        }
        if (caminho_existePercurso(rapido) && caminho_nVertices(rapido) >= 2) {
            int n = caminho_nVertices(rapido);
            double *xs = malloc(n * sizeof(double));
            double *ys = malloc(n * sizeof(double));
            for (int i = 0; i < n; i++) {
                xs[i] = vertice_getX(caminho_getVertice(rapido, i));
                ys[i] = vertice_getY(caminho_getVertice(rapido, i));
            }
            char id_path[32];
            snprintf(id_path, sizeof(id_path), "rapido%d", id_percurso);
            svg_percursoAnimado(svg, id_path, xs, ys, n, cr, 2.0, cr, 6.0);
            free(xs); free(ys);
        }
        id_percurso++;
    }

    caminho_destruir(&curto);
    caminho_destruir(&rapido);
}

double qry_getRegX(QryEstado *q, int reg) {
    if (q == NULL || reg < 0 || reg >= N_REGS || !q->valido[reg])
        return 0.0;
    return q->x[reg];
}

double qry_getRegY(QryEstado *q, int reg) {
    if (q == NULL || reg < 0 || reg >= N_REGS || !q->valido[reg])
        return 0.0;
    return q->y[reg];
}

bool qry_regValido(QryEstado *q, int reg) {
    if (q == NULL || reg < 0 || reg >= N_REGS)
        return false;
    return q->valido[reg];
}

void qry_processar(QryEstado *q, Sistema *s, char *caminho) {
    if (q == NULL || s == NULL || caminho == NULL)
        return;
    FILE *f = fopen(caminho, "r");
    if (f == NULL)
        return;

    char cmd[8];
    while (fscanf(f, "%7s", cmd) == 1) {
        if (strcmp(cmd, "@o?") == 0) {
            char reg_s[8], cep[64], face_s[4];
            double num;
            fscanf(f, "%7s %63s %3s %lf", reg_s, cep, face_s, &num);
            qry_registrar(q, s, reg_idx(reg_s), cep, face_s[0], num);
        } else if (strcmp(cmd, "mvm") == 0) {
            double v, x, y, w, h;
            fscanf(f, "%lf %lf %lf %lf %lf", &v, &x, &y, &w, &h);
            qry_mvm(s, v, x, y, w, h);
        } else if (strcmp(cmd, "regs") == 0) {
            double vl;
            fscanf(f, "%lf", &vl);
            qry_regs(s, vl);
        } else if (strcmp(cmd, "exp") == 0) {
            double vl;
            fscanf(f, "%lf", &vl);
            qry_exp(s, vl);
        } else if (strcmp(cmd, "p?") == 0) {
            char r1[8], r2[8], cc[32], cr[32];
            fscanf(f, "%7s %7s %31s %31s", r1, r2, cc, cr);
            qry_percurso(q, s, reg_idx(r1), reg_idx(r2), cc, cr);
        }
    }
    fclose(f);
}
