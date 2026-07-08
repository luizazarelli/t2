#ifndef SYSTEM_H
#define SYSTEM_H

#include "libs.h"
#include "quadra.h"
#include "graph.h"
#include "svg.h"

/*
    modulo system

    centraliza o estado principal do programa: tabela de quadras, grafo viario
    e os arquivos de saida (svg e txt).

    responsavel por ler os arquivos de entrada (.geo e .via) e gerar o mapa
    base no svg. os modulos de consulta (qry) usam os getters para acessar
    o grafo, a tabela de quadras e os arquivos de saida.
*/

typedef struct Sistema Sistema;

Sistema *sistema_criar(void);
/*
    aloca e retorna um Sistema vazio.
    saida: ponteiro para o Sistema criado, ou NULL em caso de falha.
*/

void sistema_destruir(Sistema **s);
/*
    libera toda a memoria do sistema, incluindo grafo, tabela de quadras e
    arquivos de saida (se ainda abertos).
    entrada: s: endereco do ponteiro para o Sistema a destruir.
    saida:   *s e atribuido NULL. nao faz nada se s ou *s for NULL.
*/

void sistema_lerGeo(Sistema *s, char *caminho);
/*
    le o arquivo .geo e popula a tabela de quadras do sistema.
    processa os comandos 'q' (inserir quadra) e 'cq' (definir cores correntes).
    atualiza as dimensoes do canvas com base nas coordenadas das quadras.
    entrada: s:       sistema destino.
             caminho: caminho completo do arquivo .geo.
    saida:   nenhuma. nao faz nada se s ou caminho forem NULL.
*/

void sistema_lerVia(Sistema *s, char *caminho);
/*
    le o arquivo .via e popula o grafo viario do sistema.
    processa 'v id x y' (inserir vertice) e 'e i j ldir lesq cmp vm nome' (inserir aresta).
    atualiza as dimensoes do canvas com base nas coordenadas dos vertices.
    entrada: s:       sistema destino.
             caminho: caminho completo do arquivo .via.
    saida:   nenhuma. nao faz nada se s ou caminho forem NULL.
*/

void sistema_abrirSaidas(Sistema *s, char *caminho_svg, char *caminho_txt);
/*
    abre os arquivos de saida svg e txt, escrevendo o cabecalho do svg.
    deve ser chamado apos sistema_lerGeo e sistema_lerVia, pois usa as
    dimensoes do canvas calculadas durante a leitura.
    entrada: s:           sistema.
             caminho_svg: caminho do arquivo .svg de saida.
             caminho_txt: caminho do arquivo .txt de saida.
    saida:   nenhuma. nao faz nada se s, caminho_svg ou caminho_txt forem NULL.
*/

void sistema_fecharSaidas(Sistema *s);
/*
    escreve o fechamento do svg e fecha ambos os arquivos de saida.
    entrada: s: sistema.
    saida:   nenhuma. nao faz nada se s for NULL ou os arquivos nao estiverem abertos.
*/

void sistema_desenharMapa(Sistema *s);
/*
    desenha as quadras e os segmentos de rua no svg aberto.
    deve ser chamado apos sistema_abrirSaidas.
    entrada: s: sistema com svg aberto e dados de geo e via carregados.
    saida:   nenhuma. nao faz nada se s for NULL ou o svg nao estiver aberto.
*/

Grafo *sistema_getGrafo(Sistema *s);
/*
    entrada: s: sistema.
    saida:   ponteiro para o Grafo viario, ou NULL se s for NULL.
*/

TabelaQuadra *sistema_getTabelaQuadra(Sistema *s);
/*
    entrada: s: sistema.
    saida:   ponteiro para a TabelaQuadra, ou NULL se s for NULL.
*/

FILE *sistema_getSvg(Sistema *s);
/*
    entrada: s: sistema.
    saida:   ponteiro para o FILE do svg de saida, ou NULL se s for NULL ou
             o arquivo nao estiver aberto.
*/

FILE *sistema_getTxt(Sistema *s);
/*
    entrada: s: sistema.
    saida:   ponteiro para o FILE do txt de saida, ou NULL se s for NULL ou
             o arquivo nao estiver aberto.
*/

double sistema_getLargura(Sistema *s);
double sistema_getAltura(Sistema *s);
/*
    retornam as dimensoes do canvas svg calculadas durante a leitura dos arquivos.
    entrada: s: sistema.
    saida:   dimensao em pixels, ou 0.0 se s for NULL.
*/

double sistema_getDx(Sistema *s);
double sistema_getDy(Sistema *s);
/*
    retornam o deslocamento aplicado a todas as coordenadas no svg para acomodar
    valores negativos. deve ser somado a qualquer coordenada geografica antes de
    desenhar no svg via modulos externos (ex: qry).
    entrada: s: sistema.
    saida:   deslocamento em pixels, ou 0.0 se s for NULL.
*/

double sistema_getViewMinX(Sistema *s);
double sistema_getViewMinY(Sistema *s);
/*
    retornam a coordenada minima do viewBox SVG (min_x - MARGEM e min_y - MARGEM).
    usados por qry para tracing de linhas ate a borda do canvas.
    entrada: s: sistema.
    saida:   coordenada minima do viewBox, ou 0.0 se s for NULL.
*/

#endif
