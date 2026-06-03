#include "unity/unity.h"
#include "../src/paths.h"
#include "../src/graph.h"
#include <stdlib.h>

void setUp(void) {}
void tearDown(void) {}

/*
    grafo auxiliar para testes de dijkstra:

    v1 -> v2: cmp=10, vm=1.0  (distancia=10, tempo=10)
    v1 -> v3: cmp=6,  vm=3.0  (distancia=6,  tempo=2)
    v3 -> v2: cmp=6,  vm=3.0  (distancia=6,  tempo=2)

    menor distancia v1->v2: direto (10)
    menor tempo     v1->v2: via v3 (2+2=4)
*/
static Grafo *grafo_dijkstra(void) {
    Grafo *g = grafo_criar(3);
    grafo_inserirVertice(g, "v1", 0.0, 0.0);
    grafo_inserirVertice(g, "v2", 10.0, 0.0);
    grafo_inserirVertice(g, "v3", 5.0, 5.0);
    grafo_inserirAresta(g, "v1", "v2", "-", "-", 10.0, 1.0, "R");
    grafo_inserirAresta(g, "v1", "v3", "-", "-", 6.0,  3.0, "R");
    grafo_inserirAresta(g, "v3", "v2", "-", "-", 6.0,  3.0, "R");
    return g;
}

/*
    grafo auxiliar para testes de scc e agm:

    v1 -> v2 -> v3 -> v1  (ciclo: 1 componente)
    todos com cmp=50, vm=1.0 (lentos para agm)
*/
static Grafo *grafo_ciclo(void) {
    Grafo *g = grafo_criar(3);
    grafo_inserirVertice(g, "v1", 0.0, 0.0);
    grafo_inserirVertice(g, "v2", 50.0, 0.0);
    grafo_inserirVertice(g, "v3", 25.0, 50.0);
    grafo_inserirAresta(g, "v1", "v2", "-", "-", 50.0, 1.0, "R");
    grafo_inserirAresta(g, "v2", "v3", "-", "-", 60.0, 1.0, "R");
    grafo_inserirAresta(g, "v3", "v1", "-", "-", 55.0, 1.0, "R");
    return g;
}

/*
    grafo auxiliar linear para scc:

    v1 -> v2 -> v3  (sem ciclos: 3 componentes)
*/
static Grafo *grafo_linear(void) {
    Grafo *g = grafo_criar(3);
    grafo_inserirVertice(g, "v1", 0.0, 0.0);
    grafo_inserirVertice(g, "v2", 50.0, 0.0);
    grafo_inserirVertice(g, "v3", 100.0, 0.0);
    grafo_inserirAresta(g, "v1", "v2", "-", "-", 50.0, 1.0, "R");
    grafo_inserirAresta(g, "v2", "v3", "-", "-", 50.0, 1.0, "R");
    return g;
}

/* --- dijkstra --- */

void test_caminho_destruir_seta_null(void) {
    Grafo *g = grafo_dijkstra();
    Caminho *c = caminho_calcularDistancia(g, "v1", "v2");
    caminho_destruir(&c);
    TEST_ASSERT_NULL(c);
    grafo_destruir(&g);
}

void test_caminho_distancia_existe_percurso(void) {
    Grafo *g = grafo_dijkstra();
    Caminho *c = caminho_calcularDistancia(g, "v1", "v2");
    TEST_ASSERT_TRUE(caminho_existePercurso(c));
    caminho_destruir(&c);
    grafo_destruir(&g);
}

void test_caminho_inacessivel(void) {
    Grafo *g = grafo_dijkstra();
    Caminho *c = caminho_calcularDistancia(g, "v2", "v1");
    TEST_ASSERT_FALSE(caminho_existePercurso(c));
    caminho_destruir(&c);
    grafo_destruir(&g);
}

void test_caminho_distancia_custo(void) {
    Grafo *g = grafo_dijkstra();
    Caminho *c = caminho_calcularDistancia(g, "v1", "v2");
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 10.0, caminho_getCusto(c));
    caminho_destruir(&c);
    grafo_destruir(&g);
}

void test_caminho_distancia_n_vertices(void) {
    Grafo *g = grafo_dijkstra();
    Caminho *c = caminho_calcularDistancia(g, "v1", "v2");
    TEST_ASSERT_EQUAL_INT(2, caminho_nVertices(c));
    caminho_destruir(&c);
    grafo_destruir(&g);
}

void test_caminho_distancia_vertices(void) {
    Grafo *g = grafo_dijkstra();
    Caminho *c = caminho_calcularDistancia(g, "v1", "v2");
    TEST_ASSERT_EQUAL_STRING("v1", vertice_getId(caminho_getVertice(c, 0)));
    TEST_ASSERT_EQUAL_STRING("v2", vertice_getId(caminho_getVertice(c, 1)));
    caminho_destruir(&c);
    grafo_destruir(&g);
}

void test_caminho_tempo_custo(void) {
    Grafo *g = grafo_dijkstra();
    Caminho *c = caminho_calcularTempo(g, "v1", "v2");
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 4.0, caminho_getCusto(c));
    caminho_destruir(&c);
    grafo_destruir(&g);
}

void test_caminho_tempo_n_vertices(void) {
    Grafo *g = grafo_dijkstra();
    Caminho *c = caminho_calcularTempo(g, "v1", "v2");
    TEST_ASSERT_EQUAL_INT(3, caminho_nVertices(c));
    caminho_destruir(&c);
    grafo_destruir(&g);
}

void test_caminho_tempo_vertices(void) {
    Grafo *g = grafo_dijkstra();
    Caminho *c = caminho_calcularTempo(g, "v1", "v2");
    TEST_ASSERT_EQUAL_STRING("v1", vertice_getId(caminho_getVertice(c, 0)));
    TEST_ASSERT_EQUAL_STRING("v3", vertice_getId(caminho_getVertice(c, 1)));
    TEST_ASSERT_EQUAL_STRING("v2", vertice_getId(caminho_getVertice(c, 2)));
    caminho_destruir(&c);
    grafo_destruir(&g);
}

void test_caminho_origem_igual_destino(void) {
    Grafo *g = grafo_dijkstra();
    Caminho *c = caminho_calcularDistancia(g, "v1", "v1");
    TEST_ASSERT_TRUE(caminho_existePercurso(c));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 0.0, caminho_getCusto(c));
    TEST_ASSERT_EQUAL_INT(1, caminho_nVertices(c));
    TEST_ASSERT_EQUAL_STRING("v1", vertice_getId(caminho_getVertice(c, 0)));
    caminho_destruir(&c);
    grafo_destruir(&g);
}

/* --- agm (kruskal) --- */

void test_agm_destruir_seta_null(void) {
    Grafo *g = grafo_ciclo();
    AGM *a = agm_calcular(g, 2.0);
    agm_destruir(&a);
    TEST_ASSERT_NULL(a);
    grafo_destruir(&g);
}

void test_agm_n_arestas(void) {
    Grafo *g = grafo_ciclo();
    AGM *a = agm_calcular(g, 2.0);
    TEST_ASSERT_EQUAL_INT(2, agm_nArestas(a));
    agm_destruir(&a);
    grafo_destruir(&g);
}

void test_agm_sem_arestas_lentas(void) {
    Grafo *g = grafo_ciclo();
    AGM *a = agm_calcular(g, 0.5);
    TEST_ASSERT_EQUAL_INT(0, agm_nArestas(a));
    agm_destruir(&a);
    grafo_destruir(&g);
}

void test_agm_getAresta_fora_do_intervalo(void) {
    Grafo *g = grafo_ciclo();
    AGM *a = agm_calcular(g, 2.0);
    TEST_ASSERT_NULL(agm_getAresta(a, 5));
    agm_destruir(&a);
    grafo_destruir(&g);
}

/* --- scc (tarjan) --- */

void test_scc_destruir_seta_null(void) {
    Grafo *g = grafo_ciclo();
    CompConexos *s = scc_calcular(g);
    scc_destruir(&s);
    TEST_ASSERT_NULL(s);
    grafo_destruir(&g);
}

void test_scc_ciclo_unico(void) {
    Grafo *g = grafo_ciclo();
    CompConexos *s = scc_calcular(g);
    TEST_ASSERT_EQUAL_INT(1, scc_nComponentes(s));
    int c0 = scc_getComponente(s, 0);
    int c1 = scc_getComponente(s, 1);
    int c2 = scc_getComponente(s, 2);
    TEST_ASSERT_EQUAL_INT(c0, c1);
    TEST_ASSERT_EQUAL_INT(c0, c2);
    scc_destruir(&s);
    grafo_destruir(&g);
}

void test_scc_linear(void) {
    Grafo *g = grafo_linear();
    CompConexos *s = scc_calcular(g);
    TEST_ASSERT_EQUAL_INT(3, scc_nComponentes(s));
    int c0 = scc_getComponente(s, 0);
    int c1 = scc_getComponente(s, 1);
    int c2 = scc_getComponente(s, 2);
    TEST_ASSERT_NOT_EQUAL(c0, c1);
    TEST_ASSERT_NOT_EQUAL(c1, c2);
    TEST_ASSERT_NOT_EQUAL(c0, c2);
    scc_destruir(&s);
    grafo_destruir(&g);
}

void test_scc_aresta_inativa_quebra_ciclo(void) {
    Grafo *g = grafo_ciclo();
    Aresta *a = grafo_primeiraAresta(g, "v3");
    aresta_setAtiva(a, false);
    CompConexos *s = scc_calcular(g);
    TEST_ASSERT_EQUAL_INT(3, scc_nComponentes(s));
    scc_destruir(&s);
    grafo_destruir(&g);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_caminho_destruir_seta_null);
    RUN_TEST(test_caminho_distancia_existe_percurso);
    RUN_TEST(test_caminho_inacessivel);
    RUN_TEST(test_caminho_distancia_custo);
    RUN_TEST(test_caminho_distancia_n_vertices);
    RUN_TEST(test_caminho_distancia_vertices);
    RUN_TEST(test_caminho_tempo_custo);
    RUN_TEST(test_caminho_tempo_n_vertices);
    RUN_TEST(test_caminho_tempo_vertices);
    RUN_TEST(test_caminho_origem_igual_destino);
    RUN_TEST(test_agm_destruir_seta_null);
    RUN_TEST(test_agm_n_arestas);
    RUN_TEST(test_agm_sem_arestas_lentas);
    RUN_TEST(test_agm_getAresta_fora_do_intervalo);
    RUN_TEST(test_scc_destruir_seta_null);
    RUN_TEST(test_scc_ciclo_unico);
    RUN_TEST(test_scc_linear);
    RUN_TEST(test_scc_aresta_inativa_quebra_ciclo);
    return UNITY_END();
}
