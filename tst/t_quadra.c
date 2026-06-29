#include "unity/unity.h"
#include "../src/quadra.h"
#include <stdlib.h>

void setUp(void) {}
void tearDown(void) {}

void test_quadra_criar_getters(void) {
    Quadra *q = quadra_criar("cep1", 100.0, 200.0, 50.0, 30.0);
    TEST_ASSERT_NOT_NULL(q);
    TEST_ASSERT_EQUAL_STRING("cep1", quadra_getCep(q));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 100.0, quadra_getX(q));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 200.0, quadra_getY(q));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 50.0,  quadra_getW(q));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 30.0,  quadra_getH(q));
    quadra_destruir(&q);
}

void test_quadra_destruir_seta_null(void) {
    Quadra *q = quadra_criar("cep1", 0.0, 0.0, 10.0, 10.0);
    quadra_destruir(&q);
    TEST_ASSERT_NULL(q);
}

void test_quadra_definirCores(void) {
    Quadra *q = quadra_criar("cep1", 0.0, 0.0, 10.0, 10.0);
    quadra_definirCores(q, 2.5, "red", "blue");
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 2.5, quadra_getSw(q));
    TEST_ASSERT_EQUAL_STRING("red",  quadra_getCfill(q));
    TEST_ASSERT_EQUAL_STRING("blue", quadra_getCstrk(q));
    quadra_destruir(&q);
}

void test_tabela_inserir_e_buscar(void) {
    TabelaQuadra *t = tabelaQuadra_criar(10);
    Quadra *q = quadra_criar("cep42", 10.0, 20.0, 5.0, 5.0);
    tabelaQuadra_inserir(t, q);
    TEST_ASSERT_EQUAL_PTR(q, tabelaQuadra_buscar(t, "cep42"));
    tabelaQuadra_destruir(&t);
}

void test_tabela_buscar_inexistente(void) {
    TabelaQuadra *t = tabelaQuadra_criar(10);
    TEST_ASSERT_NULL(tabelaQuadra_buscar(t, "naoexiste"));
    tabelaQuadra_destruir(&t);
}

void test_tabela_destruir_seta_null(void) {
    TabelaQuadra *t = tabelaQuadra_criar(10);
    tabelaQuadra_destruir(&t);
    TEST_ASSERT_NULL(t);
}

void test_calcularPosicao_face_S(void) {
    Quadra *q = quadra_criar("cep1", 100.0, 200.0, 50.0, 30.0);
    double px, py;
    quadra_calcularPosicao(q, 'S', 10.0, &px, &py);
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 110.0, px);
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 200.0, py);
    quadra_destruir(&q);
}

void test_calcularPosicao_face_N(void) {
    Quadra *q = quadra_criar("cep1", 100.0, 200.0, 50.0, 30.0);
    double px, py;
    quadra_calcularPosicao(q, 'N', 10.0, &px, &py);
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 110.0, px);
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 230.0, py);
    quadra_destruir(&q);
}

void test_calcularPosicao_face_L(void) {
    Quadra *q = quadra_criar("cep1", 100.0, 200.0, 50.0, 30.0);
    double px, py;
    quadra_calcularPosicao(q, 'L', 5.0, &px, &py);
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 100.0, px);
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 205.0, py);
    quadra_destruir(&q);
}

void test_calcularPosicao_face_O(void) {
    Quadra *q = quadra_criar("cep1", 100.0, 200.0, 50.0, 30.0);
    double px, py;
    quadra_calcularPosicao(q, 'O', 5.0, &px, &py);
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 150.0, px);
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 205.0, py);
    quadra_destruir(&q);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_quadra_criar_getters);
    RUN_TEST(test_quadra_destruir_seta_null);
    RUN_TEST(test_quadra_definirCores);
    RUN_TEST(test_tabela_inserir_e_buscar);
    RUN_TEST(test_tabela_buscar_inexistente);
    RUN_TEST(test_tabela_destruir_seta_null);
    RUN_TEST(test_calcularPosicao_face_S);
    RUN_TEST(test_calcularPosicao_face_N);
    RUN_TEST(test_calcularPosicao_face_L);
    RUN_TEST(test_calcularPosicao_face_O);
    return UNITY_END();
}
