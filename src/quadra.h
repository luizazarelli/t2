#ifndef QUADRA_H
#define QUADRA_H

#include "libs.h"

typedef struct Quadra Quadra;
typedef struct TabelaQuadra TabelaQuadra;

TabelaQuadra *tabelaQuadra_criar(int capacidade);
/*
    aloca e retorna uma tabela hash vazia para armazenar quadras.
    usa endereçamento aberto com sondagem linear e rehash automatico
    quando o fator de carga ultrapassa 70%.
    entrada: capacidade: numero inicial de buckets da tabela hash.
    saida:   ponteiro para a TabelaQuadra criada, ou NULL em caso de falha.
*/

void tabelaQuadra_destruir(TabelaQuadra **tabela);
/*
    libera toda a memoria da tabela e de todas as quadras nela contidas.
    entrada: tabela: endereco do ponteiro para a TabelaQuadra a destruir.
    saida:   *tabela e atribuido NULL apos a liberacao.
*/

Quadra *quadra_criar(char *cep, double x, double y, double w, double h);
/*
    aloca e retorna uma nova quadra com os dados geometricos informados.
    o ponto de ancoragem (x,y) e o canto sudeste da quadra.
    entrada: cep:  identificador alfanumerico da quadra.
             x, y: coordenadas do canto sudeste (ponto de ancoragem).
             w:    largura da quadra (eixo x).
             h:    altura da quadra (eixo y).
    saida:   ponteiro para a Quadra criada, ou NULL em caso de falha.
*/

void quadra_definirCores(Quadra *q, double sw, char *cfill, char *cstrk);
/*
    atualiza as cores e espessura de borda da quadra.
    entrada: q:     ponteiro para a quadra a atualizar.
             sw:    espessura da borda (stroke-width).
             cfill: cor de preenchimento (string CSS/SVG).
             cstrk: cor da borda (string CSS/SVG).
    saida:   nenhuma (modifica q in-place). nao faz nada se q for NULL.
*/

void quadra_destruir(Quadra **q);
/*
    libera a memoria da quadra apontada por *q.
    entrada: q: endereco do ponteiro para a Quadra a destruir.
    saida:   *q e atribuido NULL apos a liberacao.
             nao faz nada se q ou *q for NULL.
*/

void tabelaQuadra_inserir(TabelaQuadra *tabela, Quadra *q);
/*
    insere a quadra na tabela hash indexada pelo CEP da quadra.
    usa sondagem linear para resolver colisoes. redimensiona a tabela
    automaticamente se o fator de carga ultrapassar 70%.
    entrada: tabela: tabela hash destino.
             q:      quadra a inserir.
    saida:   nenhuma. nao faz nada se tabela ou q for NULL.
*/

Quadra *tabelaQuadra_buscar(TabelaQuadra *tabela, char *cep);
/*
    busca e retorna a quadra com o CEP informado usando sondagem linear.
    entrada: tabela: tabela hash onde buscar.
             cep:    identificador da quadra buscada.
    saida:   ponteiro para a Quadra encontrada, ou NULL se nao existir.
*/

int tabelaQuadra_capacidade(TabelaQuadra *tabela);
/*
    retorna o numero total de slots da tabela (incluindo vazios).
    usado para iterar sobre todas as quadras sem estrutura auxiliar.
    entrada: tabela: tabela hash.
    saida:   numero de slots, ou 0 se tabela for NULL.
*/

Quadra *tabelaQuadra_getSlot(TabelaQuadra *tabela, int i);
/*
    retorna a quadra no slot i, ou NULL se o slot estiver vazio.
    permite iterar sobre todas as quadras: for i in [0, capacidade).
    entrada: tabela: tabela hash.
             i:      indice do slot (0 <= i < capacidade).
    saida:   ponteiro para a Quadra no slot, ou NULL se vazio ou invalido.
*/

char *quadra_getCep(Quadra *q);
/*
    entrada: q: ponteiro para a quadra.
    saida:   string com o CEP da quadra, ou NULL se q for NULL.
*/

double quadra_getX(Quadra *q);
double quadra_getY(Quadra *q);
double quadra_getW(Quadra *q);
double quadra_getH(Quadra *q);
/*
    retornam, respectivamente, x, y, w e h da quadra.
    entrada: q: ponteiro para a quadra.
    saida:   valor do atributo, ou 0.0 se q for NULL.
*/

double quadra_getSw(Quadra *q);
char *quadra_getCfill(Quadra *q);
char *quadra_getCstrk(Quadra *q);
/*
    retornam, respectivamente, sw, cfill e cstrk da quadra.
    entrada: q: ponteiro para a quadra.
    saida:   sw como double (0.0 se q NULL); cfill/cstrk como string (NULL se q NULL).
*/

void quadra_calcularPosicao(Quadra *q, char face, double num, double *px, double *py);
/*
    calcula a coordenada geografica do endereco cep/face/num.
    o numero e a distancia da frente do imovel ate a projecao do ponto de ancoragem (canto SE).
    entrada: q:    quadra de referencia.
             face: face da quadra: 'N', 'S', 'L' ou 'O'.
             num:  numero do imovel na face.
             px:   ponteiro onde escrever a coordenada x calculada.
             py:   ponteiro onde escrever a coordenada y calculada.
    saida:   *px e *py recebem as coordenadas. nao faz nada se q, px ou py for NULL.
*/

#endif
