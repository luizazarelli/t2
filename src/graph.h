#ifndef GRAPH_H
#define GRAPH_H

#include "libs.h"

/*
    modulo graph

    implementa um grafo direcionado com listas de adjacencia.
    vertices representam intersecoes do sistema viario; arestas representam
    segmentos de rua com sentido de trafego definido.

    cada aresta pode ser ativada ou desativada sem ser removida, o que
    permite calcular componentes fortemente conexos ignorando trechos lentos.
    o modulo expoe iteracao sobre arestas de saida de cada vertice, suficiente
    para dijkstra (paths) e tarjan (qry/regs).
*/

typedef struct Grafo   Grafo;
typedef struct Vertice Vertice;
typedef struct Aresta  Aresta;

Grafo *grafo_criar(int capacidade);
/*
    aloca e retorna um grafo vazio com capacidade inicial para 'capacidade' vertices.
    entrada: capacidade: numero estimado de vertices (usado para pre-alocar array interno).
    saida:   ponteiro para o Grafo criado, ou NULL em caso de falha.
*/

void grafo_destruir(Grafo **g);
/*
    libera toda a memoria do grafo, incluindo vertices, arestas e strings internas.
    entrada: g: endereco do ponteiro para o Grafo a destruir.
    saida:   *g e atribuido NULL apos a liberacao. nao faz nada se g ou *g for NULL.
*/

void grafo_inserirVertice(Grafo *g, char *id, double x, double y);
/*
    insere um novo vertice no grafo.
    entrada: g:  grafo destino.
             id: identificador unico do vertice (string alfanumerica).
             x:  coordenada x da intersecao.
             y:  coordenada y da intersecao.
    saida:   nenhuma. nao faz nada se g ou id for NULL, ou se id ja existir.
*/

Vertice *grafo_buscarVertice(Grafo *g, char *id);
/*
    busca e retorna o vertice com o identificador dado.
    entrada: g:  grafo onde buscar.
             id: identificador do vertice.
    saida:   ponteiro para o Vertice encontrado, ou NULL se nao existir.
*/

int grafo_nVertices(Grafo *g);
/*
    entrada: g: grafo consultado.
    saida:   numero de vertices inseridos, ou 0 se g for NULL.
*/

Vertice *grafo_getVertice(Grafo *g, int i);
/*
    retorna o vertice de indice i (0-based, ordem de insercao).
    entrada: g: grafo consultado.
             i: indice do vertice (0 <= i < grafo_nVertices(g)).
    saida:   ponteiro para o Vertice, ou NULL se i estiver fora do intervalo.
*/

void grafo_inserirAresta(Grafo *g, char *ori, char *dst,
                         char *ldir, char *lesq,
                         double cmp, double vm, char *nome);
/*
    insere uma aresta direcionada de 'ori' para 'dst' na lista de adjacencia de 'ori'.
    entrada: g:    grafo destino.
             ori:  id do vertice de origem.
             dst:  id do vertice de destino.
             ldir: cep da quadra a direita do segmento de rua (ou hifen se ausente).
             lesq: cep da quadra a esquerda do segmento de rua (ou hifen se ausente).
             cmp:  comprimento do segmento em metros.
             vm:   velocidade media de trafego em m/s.
             nome: nome da rua a qual o segmento pertence.
    saida:   nenhuma. nao faz nada se g, ori ou dst forem NULL ou nao existirem no grafo.
*/

Aresta *grafo_primeiraAresta(Grafo *g, char *id);
/*
    retorna a primeira aresta de saida do vertice com o id dado.
    entrada: g:  grafo.
             id: id do vertice de origem.
    saida:   ponteiro para a primeira Aresta da lista de adjacencia, ou NULL se
             o vertice nao existir ou nao tiver arestas de saida.
*/

Aresta *aresta_proxima(Aresta *a);
/*
    retorna a proxima aresta de saida na lista de adjacencia do mesmo vertice de origem.
    entrada: a: aresta atual.
    saida:   ponteiro para a proxima Aresta, ou NULL se 'a' for a ultima da lista.
*/

char   *vertice_getId(Vertice *v);
/*
    entrada: v: ponteiro para o vertice.
    saida:   string com o id do vertice, ou NULL se v for NULL.
*/

double  vertice_getX(Vertice *v);
double  vertice_getY(Vertice *v);
/*
    retornam as coordenadas x e y do vertice.
    entrada: v: ponteiro para o vertice.
    saida:   valor da coordenada, ou 0.0 se v for NULL.
*/

int     vertice_getIdx(Vertice *v);
/*
    retorna o indice inteiro do vertice (0-based, ordem de insercao).
    util para arrays auxiliares em algoritmos de grafo (dijkstra, tarjan).
    entrada: v: ponteiro para o vertice.
    saida:   indice do vertice, ou -1 se v for NULL.
*/

char   *aresta_getOri(Aresta *a);
/*
    entrada: a: ponteiro para a aresta.
    saida:   id do vertice de origem, ou NULL se a for NULL.
*/

char   *aresta_getDst(Aresta *a);
/*
    entrada: a: ponteiro para a aresta.
    saida:   id do vertice de destino, ou NULL se a for NULL.
*/

char   *aresta_getNome(Aresta *a);
char   *aresta_getLdir(Aresta *a);
char   *aresta_getLesq(Aresta *a);
/*
    retornam, respectivamente, o nome da rua, o cep a direita e o cep a esquerda.
    entrada: a: ponteiro para a aresta.
    saida:   string correspondente, ou NULL se a for NULL.
*/

double  aresta_getCmp(Aresta *a);
/*
    entrada: a: ponteiro para a aresta.
    saida:   comprimento do segmento em metros, ou 0.0 se a for NULL.
*/

double  aresta_getVm(Aresta *a);
/*
    entrada: a: ponteiro para a aresta.
    saida:   velocidade media em m/s, ou 0.0 se a for NULL.
*/

void    aresta_setVm(Aresta *a, double vm);
/*
    atualiza a velocidade media da aresta.
    usado pelo comando 'exp' para aumentar vm das arestas da agm em 50%.
    entrada: a:  ponteiro para a aresta.
             vm: novo valor de velocidade media em m/s.
    saida:   nenhuma. nao faz nada se a for NULL.
*/

bool    aresta_isAtiva(Aresta *a);
/*
    indica se a aresta esta ativa (considerada no grafo).
    arestas inativas sao ignoradas em dijkstra e tarjan.
    entrada: a: ponteiro para a aresta.
    saida:   true se ativa, false se inativa ou se a for NULL.
*/

void    aresta_setAtiva(Aresta *a, bool ativa);
/*
    ativa ou desativa a aresta sem remove-la do grafo.
    usado pelo comando 'regs' para ignorar trechos com vm abaixo do limiar.
    entrada: a:     ponteiro para a aresta.
             ativa: true para ativar, false para desativar.
    saida:   nenhuma. nao faz nada se a for NULL.
*/

#endif
