#ifndef QRY_H
#define QRY_H

#include "libs.h"
#include "system.h"

/*
    modulo qry

    processa os comandos do arquivo .qry, produzindo saida em svg e txt.
    mantem os registradores geograficos R0..R10 entre comandos.

    comandos suportados:
      @o? reg cep face num  — armazena a posicao geografica do endereco no registrador.
      mvm v x y w h         — atualiza a velocidade media das arestas na regiao.
      regs vl               — calcula componentes fortemente conexos (arestas lentas desativadas).
      exp vl                — calcula agm das arestas lentas e aumenta vm em 50%.
      p? reg1 reg2 cc cr    — determina o melhor percurso entre dois registradores.
*/

typedef struct QryEstado QryEstado;

QryEstado *qry_criar(void);
/*
    aloca e retorna um estado de consulta com todos os registradores invalidos.
    saida: ponteiro para o QryEstado criado, ou NULL em caso de falha.
*/

void qry_destruir(QryEstado **q);
/*
    libera toda a memoria do estado de consulta.
    entrada: q: endereco do ponteiro para o QryEstado a destruir.
    saida:   *q e atribuido NULL. nao faz nada se q ou *q for NULL.
*/

void qry_processar(QryEstado *q, Sistema *s, char *caminho);
/*
    le o arquivo .qry e executa cada comando na ordem em que aparece.
    entrada: q:       estado com os registradores geograficos.
             s:       sistema com grafo, tabela de quadras e arquivos de saida abertos.
             caminho: caminho completo do arquivo .qry.
    saida:   nenhuma. nao faz nada se q, s ou caminho forem NULL.
*/

void qry_registrar(QryEstado *q, Sistema *s, int reg, char *cep, char face, double num);
/*
    comando @o?: calcula a posicao geografica do endereco cep/face/num e armazena
    no registrador reg (0..10).
    svg: desenha linha vertical pontilhada vermelha na posicao e o numero do registrador no topo.
    txt: reporta as coordenadas calculadas.
    entrada: q:    estado dos registradores.
             s:    sistema (tabela de quadras, svg e txt abertos).
             reg:  indice do registrador (0..10).
             cep:  identificador da quadra de referencia.
             face: face da quadra ('N', 'S', 'L' ou 'O').
             num:  numero do imovel (distancia do canto SE).
    saida:   nenhuma. nao faz nada se a quadra nao existir ou reg estiver fora do intervalo.
*/

void qry_mvm(Sistema *s, double v, double x, double y, double w, double h);
/*
    comando mvm: atualiza para v a velocidade media das arestas ativas cujo ponto
    medio esta dentro da regiao definida por (x, y, w, h) — ancora SE, largura w, altura h.
    entrada: s:         sistema (grafo).
             v:         nova velocidade media em m/s.
             x, y:      coordenadas do canto sudeste da regiao.
             w, h:      largura e altura da regiao.
    saida:   nenhuma. nao faz nada se s for NULL.
*/

void qry_regs(Sistema *s, double vl);
/*
    comando regs: desativa arestas com vm < vl, calcula os componentes fortemente
    conexos com tarjan e reativa as arestas desativadas.
    svg: para cada componente, desenha o retangulo envolvente com cor unica e 50% de opacidade.
    txt: reporta o numero de componentes fortemente conexos.
    entrada: s:  sistema (grafo, svg e txt abertos).
             vl: limiar de velocidade em m/s.
    saida:   nenhuma. nao faz nada se s for NULL.
*/

void qry_exp(Sistema *s, double vl);
/*
    comando exp: calcula a agm das arestas ativas com vm < vl (tratando o grafo como
    nao-direcionado) e aumenta em 50% a velocidade media de cada aresta da agm.
    svg: desenha as arestas da agm como linhas grossas vermelhas.
    entrada: s:  sistema (grafo, svg e txt abertos).
             vl: limiar de velocidade em m/s.
    saida:   nenhuma. nao faz nada se s for NULL.
*/

void qry_percurso(QryEstado *q, Sistema *s, int reg1, int reg2, char *cc, char *cr);
/*
    comando p?: encontra os vertices mais proximos aos registradores reg1 e reg2 e
    calcula o caminho mais curto (por distancia, cor cc) e o mais rapido (por tempo, cor cr).
    svg: desenha ambos os percursos animados com marcadores I e F.
    txt: descreve textualmente o percurso rua a rua. reporta inacessivel se nao houver caminho.
    entrada: q:    estado dos registradores.
             s:    sistema (grafo, svg e txt abertos).
             reg1: registrador de origem (0..10).
             reg2: registrador de destino (0..10).
             cc:   cor do percurso mais curto (string CSS/SVG).
             cr:   cor do percurso mais rapido (string CSS/SVG).
    saida:   nenhuma. nao faz nada se q ou s forem NULL ou os registradores forem invalidos.
*/

double qry_getRegX(QryEstado *q, int reg);
/*
    retorna a coordenada x armazenada no registrador reg.
    entrada: q:   estado dos registradores.
             reg: indice do registrador (0..10).
    saida:   coordenada x, ou 0.0 se q for NULL, reg invalido ou registrador nao inicializado.
*/

double qry_getRegY(QryEstado *q, int reg);
/*
    retorna a coordenada y armazenada no registrador reg.
    entrada: q:   estado dos registradores.
             reg: indice do registrador (0..10).
    saida:   coordenada y, ou 0.0 se q for NULL, reg invalido ou registrador nao inicializado.
*/

bool qry_regValido(QryEstado *q, int reg);
/*
    indica se o registrador reg foi inicializado por um comando @o?.
    entrada: q:   estado dos registradores.
             reg: indice do registrador (0..10).
    saida:   true se o registrador contem uma posicao valida, false caso contrario.
*/

#endif
