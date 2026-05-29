#ifndef SVG_H
#define SVG_H

#include "libs.h"

FILE *svg_criar(const char *caminho, double largura, double altura);
/*
    cria e abre um arquivo SVG no caminho informado, escrevendo o cabecalho XML e a tag <svg>.
    entrada: caminho: caminho do arquivo de saida.
             largura: largura do canvas SVG.
             altura:  altura do canvas SVG.
    saida:   ponteiro para o FILE aberto, ou NULL em caso de falha.
*/

void svg_fechar(FILE *svg);
/*
    escreve a tag de fechamento </svg> e fecha o arquivo.
    entrada: svg: ponteiro para o FILE do SVG.
    saida:   nenhuma. nao faz nada se svg for NULL.
*/

void svg_retangulo(FILE *svg, double x, double y, double w, double h,
                   const char *fill, const char *stroke, double stroke_w);
/*
    escreve um elemento <rect> no SVG.
    entrada: svg:      arquivo SVG destino.
             x, y:     coordenadas do canto superior esquerdo.
             w, h:     largura e altura do retangulo.
             fill:     cor de preenchimento (string CSS/SVG).
             stroke:   cor da borda.
             stroke_w: espessura da borda.
    saida:   nenhuma.
*/

void svg_circulo(FILE *svg, double cx, double cy, double r,
                 const char *fill, const char *stroke, double stroke_w);
/*
    escreve um elemento <circle> no SVG.
    entrada: svg:      arquivo SVG destino.
             cx, cy:   coordenadas do centro.
             r:        raio do circulo.
             fill:     cor de preenchimento.
             stroke:   cor da borda.
             stroke_w: espessura da borda.
    saida:   nenhuma.
*/

void svg_linha(FILE *svg, double x1, double y1, double x2, double y2,
               const char *stroke, double stroke_w);
/*
    escreve um elemento <line> no SVG.
    entrada: svg:      arquivo SVG destino.
             x1, y1:   coordenadas do ponto inicial.
             x2, y2:   coordenadas do ponto final.
             stroke:   cor da linha.
             stroke_w: espessura da linha.
    saida:   nenhuma.
*/

void svg_texto(FILE *svg, double x, double y,
               const char *texto, const char *fill, double font_size);
/*
    escreve um elemento <text> no SVG.
    entrada: svg:       arquivo SVG destino.
             x, y:      coordenadas da ancora do texto.
             texto:     conteudo textual a exibir.
             fill:      cor do texto.
             font_size: tamanho da fonte em pixels.
    saida:   nenhuma.
*/

void svg_percursoAnimado(FILE *svg, const char *id,
                         double *xs, double *ys, int n,
                         const char *cor_rota, double stroke_w,
                         const char *cor_marcador, double dur);
/*
    desenha o percurso como um <path> e adiciona dois marcadores animados (I e F)
    que percorrem o caminho usando <animateMotion>.
    entrada: svg:          arquivo SVG destino.
             id:           identificador unico do path no SVG.
             xs, ys:       arrays com as coordenadas x e y dos pontos do percurso.
             n:            numero de pontos nos arrays.
             cor_rota:     cor da linha do percurso.
             stroke_w:     espessura da linha do percurso.
             cor_marcador: cor dos marcadores I e F.
             dur:          duracao da animacao em segundos.
    saida:   nenhuma. nao faz nada se svg, xs, ys ou id forem NULL, ou se n < 2.
*/

#endif
