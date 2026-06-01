#include "unity/unity.h"
#include "../src/graph.h"
#include <stdlib.h>

void setUp(void) {}
void tearDown(void) {}

void test_grafo_criar_retorna_nao_nulo(void) {
    Grafo *g = grafo_criar(10);
    TEST_ASSERT_NOT_NULL(g);
    grafo_destruir(&g);
}

void test_grafo_destruir_seta_null(void) {
    Grafo *g = grafo_criar(10);
    grafo_destruir(&g);
    TEST_ASSERT_NULL(g);
}

void test_grafo_inserir_e_buscar_vertice(void) {
    Grafo *g = grafo_criar(10);
    grafo_inserirVertice(g, "v1", 10.0, 20.0);
    TEST_ASSERT_NOT_NULL(grafo_buscarVertice(g, "v1"));
    grafo_destruir(&g);
}

void test_grafo_buscar_vertice_inexistente(void) {
    Grafo *g = grafo_criar(10);
    TEST_ASSERT_NULL(grafo_buscarVertice(g, "vx"));
    grafo_destruir(&g);
}

void test_grafo_nVertices(void) {
    Grafo *g = grafo_criar(10);
    grafo_inserirVertice(g, "v1", 0.0, 0.0);
    grafo_inserirVertice(g, "v2", 1.0, 1.0);
    grafo_inserirVertice(g, "v3", 2.0, 2.0);
    TEST_ASSERT_EQUAL_INT(3, grafo_nVertices(g));
    grafo_destruir(&g);
}

void test_grafo_getVertice_por_indice(void) {
    Grafo *g = grafo_criar(10);
    grafo_inserirVertice(g, "v1", 0.0, 0.0);
    grafo_inserirVertice(g, "v2", 1.0, 1.0);
    Vertice *v = grafo_getVertice(g, 0);
    TEST_ASSERT_NOT_NULL(v);
    grafo_destruir(&g);
}

void test_grafo_getVertice_fora_do_intervalo(void) {
    Grafo *g = grafo_criar(10);
    grafo_inserirVertice(g, "v1", 0.0, 0.0);
    TEST_ASSERT_NULL(grafo_getVertice(g, 5));
    TEST_ASSERT_NULL(grafo_getVertice(g, -1));
    grafo_destruir(&g);
}

void test_vertice_getters(void) {
    Grafo *g = grafo_criar(10);
    grafo_inserirVertice(g, "v1", 10.0, 20.0);
    Vertice *v = grafo_buscarVertice(g, "v1");
    TEST_ASSERT_EQUAL_STRING("v1", vertice_getId(v));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 10.0, vertice_getX(v));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 20.0, vertice_getY(v));
    TEST_ASSERT_EQUAL_INT(0, vertice_getIdx(v));
    grafo_destruir(&g);
}

void test_grafo_inserir_e_iterar_aresta(void) {
    Grafo *g = grafo_criar(10);
    grafo_inserirVertice(g, "v1", 0.0, 0.0);
    grafo_inserirVertice(g, "v2", 1.0, 0.0);
    grafo_inserirAresta(g, "v1", "v2", "cep1", "cep2", 100.0, 3.5, "Rua_A");
    Aresta *a = grafo_primeiraAresta(g, "v1");
    TEST_ASSERT_NOT_NULL(a);
    grafo_destruir(&g);
}

void test_aresta_getters(void) {
    Grafo *g = grafo_criar(10);
    grafo_inserirVertice(g, "v1", 0.0, 0.0);
    grafo_inserirVertice(g, "v2", 1.0, 0.0);
    grafo_inserirAresta(g, "v1", "v2", "cep1", "cep2", 100.0, 3.5, "Rua_A");
    Aresta *a = grafo_primeiraAresta(g, "v1");
    TEST_ASSERT_EQUAL_STRING("v1",   aresta_getOri(a));
    TEST_ASSERT_EQUAL_STRING("v2",   aresta_getDst(a));
    TEST_ASSERT_EQUAL_STRING("cep1", aresta_getLdir(a));
    TEST_ASSERT_EQUAL_STRING("cep2", aresta_getLesq(a));
    TEST_ASSERT_EQUAL_STRING("Rua_A", aresta_getNome(a));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 100.0, aresta_getCmp(a));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 3.5,   aresta_getVm(a));
    grafo_destruir(&g);
}

void test_aresta_proxima(void) {
    Grafo *g = grafo_criar(10);
    grafo_inserirVertice(g, "v1", 0.0, 0.0);
    grafo_inserirVertice(g, "v2", 1.0, 0.0);
    grafo_inserirVertice(g, "v3", 2.0, 0.0);
    grafo_inserirAresta(g, "v1", "v2", "-", "-", 50.0, 2.0, "Rua_A");
    grafo_inserirAresta(g, "v1", "v3", "-", "-", 80.0, 4.0, "Rua_B");
    int count = 0;
    for (Aresta *a = grafo_primeiraAresta(g, "v1"); a != NULL; a = aresta_proxima(a))
        count++;
    TEST_ASSERT_EQUAL_INT(2, count);
    grafo_destruir(&g);
}

void test_grafo_primeiraAresta_sem_arestas(void) {
    Grafo *g = grafo_criar(10);
    grafo_inserirVertice(g, "v1", 0.0, 0.0);
    TEST_ASSERT_NULL(grafo_primeiraAresta(g, "v1"));
    grafo_destruir(&g);
}

void test_aresta_isAtiva_padrao(void) {
    Grafo *g = grafo_criar(10);
    grafo_inserirVertice(g, "v1", 0.0, 0.0);
    grafo_inserirVertice(g, "v2", 1.0, 0.0);
    grafo_inserirAresta(g, "v1", "v2", "-", "-", 50.0, 2.0, "Rua_A");
    Aresta *a = grafo_primeiraAresta(g, "v1");
    TEST_ASSERT_TRUE(aresta_isAtiva(a));
    grafo_destruir(&g);
}

void test_aresta_setAtiva(void) {
    Grafo *g = grafo_criar(10);
    grafo_inserirVertice(g, "v1", 0.0, 0.0);
    grafo_inserirVertice(g, "v2", 1.0, 0.0);
    grafo_inserirAresta(g, "v1", "v2", "-", "-", 50.0, 2.0, "Rua_A");
    Aresta *a = grafo_primeiraAresta(g, "v1");
    aresta_setAtiva(a, false);
    TEST_ASSERT_FALSE(aresta_isAtiva(a));
    aresta_setAtiva(a, true);
    TEST_ASSERT_TRUE(aresta_isAtiva(a));
    grafo_destruir(&g);
}

void test_aresta_setVm(void) {
    Grafo *g = grafo_criar(10);
    grafo_inserirVertice(g, "v1", 0.0, 0.0);
    grafo_inserirVertice(g, "v2", 1.0, 0.0);
    grafo_inserirAresta(g, "v1", "v2", "-", "-", 50.0, 2.0, "Rua_A");
    Aresta *a = grafo_primeiraAresta(g, "v1");
    aresta_setVm(a, 3.0);
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 3.0, aresta_getVm(a));
    grafo_destruir(&g);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_grafo_criar_retorna_nao_nulo);
    RUN_TEST(test_grafo_destruir_seta_null);
    RUN_TEST(test_grafo_inserir_e_buscar_vertice);
    RUN_TEST(test_grafo_buscar_vertice_inexistente);
    RUN_TEST(test_grafo_nVertices);
    RUN_TEST(test_grafo_getVertice_por_indice);
    RUN_TEST(test_grafo_getVertice_fora_do_intervalo);
    RUN_TEST(test_vertice_getters);
    RUN_TEST(test_grafo_inserir_e_iterar_aresta);
    RUN_TEST(test_aresta_getters);
    RUN_TEST(test_aresta_proxima);
    RUN_TEST(test_grafo_primeiraAresta_sem_arestas);
    RUN_TEST(test_aresta_isAtiva_padrao);
    RUN_TEST(test_aresta_setAtiva);
    RUN_TEST(test_aresta_setVm);
    return UNITY_END();
}
