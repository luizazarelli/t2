#include "unity/unity.h"
#include "../src/system.h"
#include "../src/quadra.h"
#include "../src/graph.h"
#include <stdio.h>
#include <stdlib.h>

#define GEO_TMP "/tmp/t_system.geo"
#define VIA_TMP "/tmp/t_system.via"
#define SVG_TMP "/tmp/t_system.svg"
#define TXT_TMP "/tmp/t_system.txt"

void setUp(void) {
    FILE *f = fopen(GEO_TMP, "w");
    fprintf(f, "cq 1.0px coral Moccasin\n");
    fprintf(f, "q cep1 100.0 100.0 80.0 60.0\n");
    fprintf(f, "q cep2 220.0 100.0 80.0 60.0\n");
    fclose(f);

    f = fopen(VIA_TMP, "w");
    fprintf(f, "4\n");
    fprintf(f, "v v1 10.0 10.0\n");
    fprintf(f, "v v2 110.0 10.0\n");
    fprintf(f, "v v3 10.0 110.0\n");
    fprintf(f, "v v4 110.0 110.0\n");
    fprintf(f, "e v1 v2 cep1 # 100.0 5.0 Rua_A\n");
    fprintf(f, "e v2 v1 # cep1 100.0 5.0 Rua_A\n");
    fclose(f);
}

void tearDown(void) {
    remove(GEO_TMP);
    remove(VIA_TMP);
    remove(SVG_TMP);
    remove(TXT_TMP);
}

void test_sistema_criar_retorna_nao_nulo(void) {
    Sistema *s = sistema_criar();
    TEST_ASSERT_NOT_NULL(s);
    sistema_destruir(&s);
}

void test_sistema_destruir_seta_null(void) {
    Sistema *s = sistema_criar();
    sistema_destruir(&s);
    TEST_ASSERT_NULL(s);
}

void test_sistema_lerGeo_tabela_nao_nula(void) {
    Sistema *s = sistema_criar();
    sistema_lerGeo(s, GEO_TMP);
    TEST_ASSERT_NOT_NULL(sistema_getTabelaQuadra(s));
    sistema_destruir(&s);
}

void test_sistema_lerGeo_quadra_encontrada(void) {
    Sistema *s = sistema_criar();
    sistema_lerGeo(s, GEO_TMP);
    TabelaQuadra *t = sistema_getTabelaQuadra(s);
    TEST_ASSERT_NOT_NULL(tabelaQuadra_buscar(t, "cep1"));
    TEST_ASSERT_NOT_NULL(tabelaQuadra_buscar(t, "cep2"));
    sistema_destruir(&s);
}

void test_sistema_lerGeo_quadra_coordenadas(void) {
    Sistema *s = sistema_criar();
    sistema_lerGeo(s, GEO_TMP);
    Quadra *q = tabelaQuadra_buscar(sistema_getTabelaQuadra(s), "cep1");
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 100.0, quadra_getX(q));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 100.0, quadra_getY(q));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 80.0,  quadra_getW(q));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 60.0,  quadra_getH(q));
    sistema_destruir(&s);
}

void test_sistema_lerGeo_quadra_cores(void) {
    Sistema *s = sistema_criar();
    sistema_lerGeo(s, GEO_TMP);
    Quadra *q = tabelaQuadra_buscar(sistema_getTabelaQuadra(s), "cep1");
    TEST_ASSERT_EQUAL_STRING("coral",    quadra_getCfill(q));
    TEST_ASSERT_EQUAL_STRING("Moccasin", quadra_getCstrk(q));
    sistema_destruir(&s);
}

void test_sistema_lerGeo_atualiza_dimensoes(void) {
    Sistema *s = sistema_criar();
    sistema_lerGeo(s, GEO_TMP);
    TEST_ASSERT_GREATER_THAN(0.0, sistema_getLargura(s));
    TEST_ASSERT_GREATER_THAN(0.0, sistema_getAltura(s));
    sistema_destruir(&s);
}

void test_sistema_lerVia_grafo_nao_nulo(void) {
    Sistema *s = sistema_criar();
    sistema_lerVia(s, VIA_TMP);
    TEST_ASSERT_NOT_NULL(sistema_getGrafo(s));
    sistema_destruir(&s);
}

void test_sistema_lerVia_n_vertices(void) {
    Sistema *s = sistema_criar();
    sistema_lerVia(s, VIA_TMP);
    TEST_ASSERT_EQUAL_INT(4, grafo_nVertices(sistema_getGrafo(s)));
    sistema_destruir(&s);
}

void test_sistema_lerVia_vertices_coordenadas(void) {
    Sistema *s = sistema_criar();
    sistema_lerVia(s, VIA_TMP);
    Vertice *v = grafo_buscarVertice(sistema_getGrafo(s), "v1");
    TEST_ASSERT_NOT_NULL(v);
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 10.0, vertice_getX(v));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 10.0, vertice_getY(v));
    sistema_destruir(&s);
}

void test_sistema_lerVia_arestas_inseridas(void) {
    Sistema *s = sistema_criar();
    sistema_lerVia(s, VIA_TMP);
    Grafo *g = sistema_getGrafo(s);
    TEST_ASSERT_NOT_NULL(grafo_primeiraAresta(g, "v1"));
    sistema_destruir(&s);
}

void test_sistema_lerVia_atualiza_dimensoes(void) {
    Sistema *s = sistema_criar();
    sistema_lerVia(s, VIA_TMP);
    TEST_ASSERT_GREATER_THAN(0.0, sistema_getLargura(s));
    TEST_ASSERT_GREATER_THAN(0.0, sistema_getAltura(s));
    sistema_destruir(&s);
}

void test_sistema_abrirSaidas_svg_nao_nulo(void) {
    Sistema *s = sistema_criar();
    sistema_lerGeo(s, GEO_TMP);
    sistema_lerVia(s, VIA_TMP);
    sistema_abrirSaidas(s, SVG_TMP, TXT_TMP);
    TEST_ASSERT_NOT_NULL(sistema_getSvg(s));
    sistema_fecharSaidas(s);
    sistema_destruir(&s);
}

void test_sistema_abrirSaidas_txt_nao_nulo(void) {
    Sistema *s = sistema_criar();
    sistema_lerGeo(s, GEO_TMP);
    sistema_lerVia(s, VIA_TMP);
    sistema_abrirSaidas(s, SVG_TMP, TXT_TMP);
    TEST_ASSERT_NOT_NULL(sistema_getTxt(s));
    sistema_fecharSaidas(s);
    sistema_destruir(&s);
}

void test_sistema_fecharSaidas_svg_null(void) {
    Sistema *s = sistema_criar();
    sistema_lerGeo(s, GEO_TMP);
    sistema_lerVia(s, VIA_TMP);
    sistema_abrirSaidas(s, SVG_TMP, TXT_TMP);
    sistema_fecharSaidas(s);
    TEST_ASSERT_NULL(sistema_getSvg(s));
    sistema_destruir(&s);
}

void test_sistema_desenharMapa_nao_crasha(void) {
    Sistema *s = sistema_criar();
    sistema_lerGeo(s, GEO_TMP);
    sistema_lerVia(s, VIA_TMP);
    sistema_abrirSaidas(s, SVG_TMP, TXT_TMP);
    sistema_desenharMapa(s);
    sistema_fecharSaidas(s);
    sistema_destruir(&s);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_sistema_criar_retorna_nao_nulo);
    RUN_TEST(test_sistema_destruir_seta_null);
    RUN_TEST(test_sistema_lerGeo_tabela_nao_nula);
    RUN_TEST(test_sistema_lerGeo_quadra_encontrada);
    RUN_TEST(test_sistema_lerGeo_quadra_coordenadas);
    RUN_TEST(test_sistema_lerGeo_quadra_cores);
    RUN_TEST(test_sistema_lerGeo_atualiza_dimensoes);
    RUN_TEST(test_sistema_lerVia_grafo_nao_nulo);
    RUN_TEST(test_sistema_lerVia_n_vertices);
    RUN_TEST(test_sistema_lerVia_vertices_coordenadas);
    RUN_TEST(test_sistema_lerVia_arestas_inseridas);
    RUN_TEST(test_sistema_lerVia_atualiza_dimensoes);
    RUN_TEST(test_sistema_abrirSaidas_svg_nao_nulo);
    RUN_TEST(test_sistema_abrirSaidas_txt_nao_nulo);
    RUN_TEST(test_sistema_fecharSaidas_svg_null);
    RUN_TEST(test_sistema_desenharMapa_nao_crasha);
    return UNITY_END();
}
