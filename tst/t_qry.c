#include "unity/unity.h"
#include "../src/qry.h"
#include "../src/system.h"
#include "../src/graph.h"
#include "../src/quadra.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GEO_TMP "/tmp/t_qry.geo"
#define VIA_TMP "/tmp/t_qry.via"
#define SVG_TMP "/tmp/t_qry.svg"
#define TXT_TMP "/tmp/t_qry.txt"

/*
    grafo auxiliar:
      v1(50,50)  v2(150,50)  v3(250,50)

      v1 -> v2: cmp=100, vm=2.0  (lento)
      v2 -> v1: cmp=100, vm=5.0  (rapido)
      v2 -> v3: cmp=100, vm=5.0  (rapido)
      v3 -> v2: cmp=100, vm=5.0  (rapido)

    com vl=3.0: v1->v2 desativada; v2 e v3 formam 1 scc; v1 fica isolado.
    agm das arestas lentas: apenas v1->v2 (vm=2.0 -> 3.0 apos exp).

    quadra para @o?:
      cep1: ancora (200,200), w=100, h=80
      face 'S', num=10  ->  px=190, py=200
*/

static Sistema *s;
static QryEstado *q;

void setUp(void) {
    FILE *f = fopen(GEO_TMP, "w");
    fprintf(f, "cq 1.0px coral Moccasin\n");
    fprintf(f, "q cep1 200.0 200.0 100.0 80.0\n");
    fclose(f);

    f = fopen(VIA_TMP, "w");
    fprintf(f, "3\n");
    fprintf(f, "v v1 50.0 50.0\n");
    fprintf(f, "v v2 150.0 50.0\n");
    fprintf(f, "v v3 250.0 50.0\n");
    fprintf(f, "e v1 v2 cep1 # 100.0 2.0 Rua_A\n");
    fprintf(f, "e v2 v1 # cep1 100.0 5.0 Rua_A\n");
    fprintf(f, "e v2 v3 # # 100.0 5.0 Rua_B\n");
    fprintf(f, "e v3 v2 # # 100.0 5.0 Rua_B\n");
    fclose(f);

    s = sistema_criar();
    sistema_lerGeo(s, GEO_TMP);
    sistema_lerVia(s, VIA_TMP);
    sistema_abrirSaidas(s, SVG_TMP, TXT_TMP);
    sistema_desenharMapa(s);

    q = qry_criar();
}

void tearDown(void) {
    qry_destruir(&q);
    sistema_fecharSaidas(s);
    sistema_destruir(&s);
    remove(GEO_TMP);
    remove(VIA_TMP);
    remove(SVG_TMP);
    remove(TXT_TMP);
}

/* --- ciclo de vida --- */

void test_qry_criar_destruir(void) {
    QryEstado *estado = qry_criar();
    TEST_ASSERT_NOT_NULL(estado);
    qry_destruir(&estado);
    TEST_ASSERT_NULL(estado);
}

/* --- registradores --- */

void test_qry_reg_invalido_inicial(void) {
    TEST_ASSERT_FALSE(qry_regValido(q, 0));
    TEST_ASSERT_FALSE(qry_regValido(q, 5));
    TEST_ASSERT_FALSE(qry_regValido(q, 10));
}

void test_qry_regValido_fora_do_intervalo(void) {
    TEST_ASSERT_FALSE(qry_regValido(q, -1));
    TEST_ASSERT_FALSE(qry_regValido(q, 11));
}

void test_qry_registrar_valido(void) {
    qry_registrar(q, s, 0, "cep1", 'S', 10.0);
    TEST_ASSERT_TRUE(qry_regValido(q, 0));
}

void test_qry_registrar_coordenadas(void) {
    qry_registrar(q, s, 0, "cep1", 'S', 10.0);
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 190.0, qry_getRegX(q, 0));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 200.0, qry_getRegY(q, 0));
}

void test_qry_registrar_cep_inexistente_nao_valida(void) {
    qry_registrar(q, s, 0, "naoexiste", 'S', 10.0);
    TEST_ASSERT_FALSE(qry_regValido(q, 0));
}

/* --- mvm --- */

void test_qry_mvm_atualiza_vm(void) {
    /* regiao que contem o ponto medio de v1->v2 = (100, 50)
       bbox padrao: xmin=20, ymin=30, w=120, h=40 -> cobre [20..140] x [30..70] */
    qry_mvm(s, 8.0, 20.0, 30.0, 120.0, 40.0);
    Aresta *a = grafo_primeiraAresta(sistema_getGrafo(s), "v1");
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 8.0, aresta_getVm(a));
}

void test_qry_mvm_fora_regiao_preserva_vm(void) {
    /* regiao que NAO contem v1->v2 (ponto medio (100,50) fora de [300..400]x[40..80]) */
    qry_mvm(s, 8.0, 300.0, 40.0, 100.0, 40.0);
    Aresta *a = grafo_primeiraAresta(sistema_getGrafo(s), "v1");
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 2.0, aresta_getVm(a));
}

/* --- regs --- */

void test_qry_regs_escreve_txt(void) {
    qry_regs(s, 3.0);
    fflush(sistema_getTxt(s));
    sistema_fecharSaidas(s);

    FILE *f = fopen(TXT_TMP, "r");
    TEST_ASSERT_NOT_NULL(f);
    char buf[256] = {0};
    fread(buf, 1, sizeof(buf) - 1, f);
    fclose(f);
    TEST_ASSERT_GREATER_THAN(0, (int)strlen(buf));

    sistema_abrirSaidas(s, SVG_TMP, TXT_TMP);
}

void test_qry_regs_reativa_arestas(void) {
    qry_regs(s, 3.0);
    Aresta *a = grafo_primeiraAresta(sistema_getGrafo(s), "v1");
    TEST_ASSERT_TRUE(aresta_isAtiva(a));
}

/* --- exp --- */

void test_qry_exp_aumenta_vm(void) {
    qry_exp(s, 3.0);
    Aresta *a = grafo_primeiraAresta(sistema_getGrafo(s), "v1");
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 3.0, aresta_getVm(a));
}

void test_qry_exp_nao_altera_arestas_rapidas(void) {
    qry_exp(s, 3.0);
    Aresta *a = grafo_primeiraAresta(sistema_getGrafo(s), "v2");
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 5.0, aresta_getVm(a));
}

/* --- percurso --- */

void test_qry_percurso_escreve_txt(void) {
    qry_registrar(q, s, 0, "cep1", 'S', 10.0);
    qry_registrar(q, s, 1, "cep1", 'N', 10.0);
    qry_percurso(q, s, 0, 1, "yellow", "blue");
    fflush(sistema_getTxt(s));
    sistema_fecharSaidas(s);

    FILE *f = fopen(TXT_TMP, "r");
    char buf[512] = {0};
    fread(buf, 1, sizeof(buf) - 1, f);
    fclose(f);
    TEST_ASSERT_GREATER_THAN(0, (int)strlen(buf));

    sistema_abrirSaidas(s, SVG_TMP, TXT_TMP);
}

void test_qry_percurso_reg_invalido_nao_crasha(void) {
    qry_percurso(q, s, 0, 1, "yellow", "blue");
    TEST_PASS();
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_qry_criar_destruir);
    RUN_TEST(test_qry_reg_invalido_inicial);
    RUN_TEST(test_qry_regValido_fora_do_intervalo);
    RUN_TEST(test_qry_registrar_valido);
    RUN_TEST(test_qry_registrar_coordenadas);
    RUN_TEST(test_qry_registrar_cep_inexistente_nao_valida);
    RUN_TEST(test_qry_mvm_atualiza_vm);
    RUN_TEST(test_qry_mvm_fora_regiao_preserva_vm);
    RUN_TEST(test_qry_regs_escreve_txt);
    RUN_TEST(test_qry_regs_reativa_arestas);
    RUN_TEST(test_qry_exp_aumenta_vm);
    RUN_TEST(test_qry_exp_nao_altera_arestas_rapidas);
    RUN_TEST(test_qry_percurso_escreve_txt);
    RUN_TEST(test_qry_percurso_reg_invalido_nao_crasha);
    return UNITY_END();
}
