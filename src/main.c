#include "system.h"
#include "qry.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static char *juntar(char *dir, char *arq) {
    if (dir == NULL || arq == NULL)
        return NULL;
    size_t n = strlen(dir) + strlen(arq) + 2;
    char *caminho = malloc(n);
    if (caminho == NULL)
        return NULL;
    snprintf(caminho, n, "%s%s", dir, arq);
    return caminho;
}

static char *nome_base(char *arq) {
    char *ultimo_barra = strrchr(arq, '/');
    char *inicio = ultimo_barra ? ultimo_barra + 1 : arq;
    char *copia = malloc(strlen(inicio) + 1);
    if (copia == NULL)
        return NULL;
    strcpy(copia, inicio);
    char *ponto = strrchr(copia, '.');
    if (ponto != NULL)
        *ponto = '\0';
    return copia;
}

static char *garantir_barra(char *dir) {
    if (dir == NULL)
        return NULL;
    size_t n = strlen(dir);
    if (n == 0 || dir[n - 1] == '/')
        return dir;
    char *novo = malloc(n + 2);
    if (novo == NULL)
        return dir;
    snprintf(novo, n + 2, "%s/", dir);
    free(dir);
    return novo;
}

int main(int argc, char *argv[]) {
    char *bed = NULL, *bsd = NULL;
    char *geo = NULL, *via = NULL, *qry = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-e") == 0 && i + 1 < argc) bed = argv[++i];
        else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) geo = argv[++i];
        else if (strcmp(argv[i], "-v") == 0 && i + 1 < argc) via = argv[++i];
        else if (strcmp(argv[i], "-q") == 0 && i + 1 < argc) qry = argv[++i];
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) bsd = argv[++i];
    }

    if (geo == NULL || bsd == NULL) {
        fprintf(stderr, "uso: ted [-e bed] -f arq.geo [-v arq.via] [-q arq.qry] -o bsd\n");
        return 1;
    }

    char *bed_dir = bed ? juntar(bed, "") : juntar("", "");
    bed_dir = garantir_barra(bed_dir);

    char *bsd_dir = juntar(bsd, "");
    bsd_dir = garantir_barra(bsd_dir);

    char *base     = nome_base(geo);
    char *bedgeo   = juntar(bed_dir, geo);
    char *bedvia   = via ? juntar(bed_dir, via) : NULL;
    char *bedqry   = qry ? juntar(bed_dir, qry) : NULL;
    char *qry_base = qry ? nome_base(qry) : NULL;

    char *svg_path, *txt_path;
    if (qry_base != NULL) {
        size_t n = strlen(bsd_dir) + strlen(base) + strlen(qry_base) + 8;
        svg_path = malloc(n);
        txt_path = malloc(n);
        snprintf(svg_path, n, "%s%s-%s.svg", bsd_dir, base, qry_base);
        snprintf(txt_path, n, "%s%s-%s.txt", bsd_dir, base, qry_base);
    } else {
        size_t n = strlen(bsd_dir) + strlen(base) + 6;
        svg_path = malloc(n);
        txt_path = malloc(n);
        snprintf(svg_path, n, "%s%s.svg", bsd_dir, base);
        snprintf(txt_path, n, "%s%s.txt", bsd_dir, base);
    }

    Sistema *s = sistema_criar();
    sistema_lerGeo(s, bedgeo);
    if (bedvia != NULL)
        sistema_lerVia(s, bedvia);
    sistema_abrirSaidas(s, svg_path, txt_path);
    sistema_desenharMapa(s);

    if (bedqry != NULL) {
        QryEstado *q = qry_criar();
        qry_processar(q, s, bedqry);
        qry_destruir(&q);
    }

    sistema_fecharSaidas(s);
    sistema_destruir(&s);

    free(bed_dir);
    free(bsd_dir);
    free(base);
    free(bedgeo);
    free(bedvia);
    free(bedqry);
    free(qry_base);
    free(svg_path);
    free(txt_path);

    return 0;
}
