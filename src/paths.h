#ifndef PATHS_H
#define PATHS_H

#include "libs.h"
#include "graph.h"

/*
    modulo paths

    implementa os algoritmos de grafos utilizados pelo sistema:
      - dijkstra para caminho mais curto (peso: cmp) e mais rapido (peso: cmp/vm).
      - kruskal para arvore geradora minima sobre arestas lentas (comando exp).
      - tarjan para componentes fortemente conexos sobre arestas ativas (comando regs).

    todos os algoritmos respeitam o flag ativo de cada aresta:
    arestas inativas sao ignoradas durante a execucao.
*/

typedef struct Caminho     Caminho;
typedef struct AGM         AGM;
typedef struct CompConexos CompConexos;

Caminho *caminho_calcularDistancia(Grafo *g, char *ori, char *dst);
/*
    executa dijkstra de 'ori' a 'dst' minimizando distancia (peso = cmp).
    entrada: g:   grafo sobre o qual executar o algoritmo.
             ori: id do vertice de origem.
             dst: id do vertice de destino.
    saida:   ponteiro para o Caminho calculado, ou NULL em caso de falha de alocacao.
             use caminho_existePercurso para verificar se o destino e alcancavel.
*/

Caminho *caminho_calcularTempo(Grafo *g, char *ori, char *dst);
/*
    executa dijkstra de 'ori' a 'dst' minimizando tempo de percurso (peso = cmp / vm).
    entrada: g:   grafo sobre o qual executar o algoritmo.
             ori: id do vertice de origem.
             dst: id do vertice de destino.
    saida:   ponteiro para o Caminho calculado, ou NULL em caso de falha de alocacao.
             use caminho_existePercurso para verificar se o destino e alcancavel.
*/

void caminho_destruir(Caminho **c);
/*
    libera toda a memoria associada ao Caminho.
    entrada: c: endereco do ponteiro para o Caminho a destruir.
    saida:   *c e atribuido NULL apos a liberacao. nao faz nada se c ou *c for NULL.
*/

bool caminho_existePercurso(Caminho *c);
/*
    indica se o destino foi alcancado pelo algoritmo de dijkstra.
    entrada: c: Caminho retornado por caminho_calcularDistancia ou caminho_calcularTempo.
    saida:   true se existe percurso, false se o destino e inacessivel ou c for NULL.
*/

double caminho_getCusto(Caminho *c);
/*
    retorna o custo total do percurso (distancia em metros ou tempo em segundos).
    entrada: c: Caminho com percurso existente.
    saida:   custo total, ou 0.0 se c for NULL ou nao houver percurso.
*/

int caminho_nVertices(Caminho *c);
/*
    retorna o numero de vertices no percurso (incluindo origem e destino).
    entrada: c: Caminho com percurso existente.
    saida:   numero de vertices, ou 0 se c for NULL ou nao houver percurso.
*/

Vertice *caminho_getVertice(Caminho *c, int i);
/*
    retorna o i-esimo vertice do percurso (0-based, da origem ao destino).
    entrada: c: Caminho com percurso existente.
             i: indice do vertice no percurso (0 <= i < caminho_nVertices(c)).
    saida:   ponteiro para o Vertice, ou NULL se i estiver fora do intervalo.
*/

AGM *agm_calcular(Grafo *g, double limiar);
/*
    executa kruskal sobre as arestas ativas com vm < limiar, tratando o grafo como
    nao-direcionado. retorna a arvore geradora minima desse subgrafo.
    entrada: g:      grafo de entrada.
             limiar: velocidade limite; so arestas com vm < limiar sao consideradas.
    saida:   ponteiro para a AGM calculada, ou NULL em caso de falha.
             pode retornar uma AGM vazia se nenhuma aresta satisfaz o criterio.
*/

void agm_destruir(AGM **a);
/*
    libera toda a memoria associada a AGM.
    entrada: a: endereco do ponteiro para a AGM a destruir.
    saida:   *a e atribuido NULL. nao faz nada se a ou *a for NULL.
*/

int agm_nArestas(AGM *a);
/*
    entrada: a: AGM calculada.
    saida:   numero de arestas na arvore geradora minima, ou 0 se a for NULL.
*/

Aresta *agm_getAresta(AGM *a, int i);
/*
    retorna a i-esima aresta da AGM (0-based).
    entrada: a: AGM calculada.
             i: indice da aresta (0 <= i < agm_nArestas(a)).
    saida:   ponteiro para a Aresta, ou NULL se i estiver fora do intervalo.
*/

CompConexos *scc_calcular(Grafo *g);
/*
    executa tarjan sobre as arestas ativas do grafo e identifica os componentes
    fortemente conexos.
    entrada: g: grafo de entrada.
    saida:   ponteiro para o resultado CompConexos, ou NULL em caso de falha.
*/

void scc_destruir(CompConexos **s);
/*
    libera toda a memoria associada ao resultado de scc_calcular.
    entrada: s: endereco do ponteiro para o CompConexos a destruir.
    saida:   *s e atribuido NULL. nao faz nada se s ou *s for NULL.
*/

int scc_nComponentes(CompConexos *s);
/*
    entrada: s: resultado de scc_calcular.
    saida:   numero de componentes fortemente conexos, ou 0 se s for NULL.
*/

int scc_getComponente(CompConexos *s, int verticeIdx);
/*
    retorna o identificador do componente ao qual o vertice de indice verticeIdx pertence.
    vertices no mesmo componente tem o mesmo identificador.
    entrada: s:          resultado de scc_calcular.
             verticeIdx: indice do vertice (vertice_getIdx).
    saida:   id do componente (inteiro >= 0), ou -1 se s for NULL ou verticeIdx invalido.
*/

#endif
