#include "unity/unity.h"
#include "../src/svg.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define TMP_PATH "/tmp/t_svg_test.svg"

static int contem(FILE *f, const char *trecho) {
    rewind(f);
    char buf[8192] = {0};
    fread(buf, 1, sizeof(buf) - 1, f);
    return strstr(buf, trecho) != NULL;
}

void setUp(void) {}
void tearDown(void) { remove(TMP_PATH); }

void test_svg_criar_retorna_nao_nulo(void) {
    FILE *f = svg_criar(TMP_PATH, 800.0, 600.0);
    TEST_ASSERT_NOT_NULL(f);
    svg_fechar(f);
}

void test_svg_criar_escreve_cabecalho(void) {
    FILE *f = svg_criar(TMP_PATH, 800.0, 600.0);
    TEST_ASSERT_TRUE(contem(f, "<?xml"));
    TEST_ASSERT_TRUE(contem(f, "<svg"));
    svg_fechar(f);
}

void test_svg_fechar_escreve_fechamento(void) {
    FILE *f = svg_criar(TMP_PATH, 800.0, 600.0);
    svg_fechar(f);
    f = fopen(TMP_PATH, "r");
    TEST_ASSERT_TRUE(contem(f, "</svg>"));
    fclose(f);
}

void test_svg_fechar_null_nao_crasha(void) {
    svg_fechar(NULL);
    TEST_PASS();
}

void test_svg_retangulo(void) {
    FILE *f = tmpfile();
    svg_retangulo(f, 10.0, 20.0, 50.0, 30.0, "red", "blue", 1.5);
    TEST_ASSERT_TRUE(contem(f, "<rect"));
    TEST_ASSERT_TRUE(contem(f, "red"));
    TEST_ASSERT_TRUE(contem(f, "blue"));
    fclose(f);
}

void test_svg_circulo(void) {
    FILE *f = tmpfile();
    svg_circulo(f, 100.0, 200.0, 5.0, "green", "black", 1.0);
    TEST_ASSERT_TRUE(contem(f, "<circle"));
    TEST_ASSERT_TRUE(contem(f, "green"));
    fclose(f);
}

void test_svg_linha(void) {
    FILE *f = tmpfile();
    svg_linha(f, 0.0, 0.0, 100.0, 100.0, "black", 2.0);
    TEST_ASSERT_TRUE(contem(f, "<line"));
    TEST_ASSERT_TRUE(contem(f, "100"));
    fclose(f);
}

void test_svg_texto(void) {
    FILE *f = tmpfile();
    svg_texto(f, 50.0, 50.0, "Bitnopolis", "black", 12.0);
    TEST_ASSERT_TRUE(contem(f, "<text"));
    TEST_ASSERT_TRUE(contem(f, "Bitnopolis"));
    fclose(f);
}

void test_svg_percursoAnimado_escreve_path_e_animacao(void) {
    FILE *f = tmpfile();
    double xs[] = {0.0, 100.0, 200.0};
    double ys[] = {0.0,  50.0, 100.0};
    svg_percursoAnimado(f, "rota1", xs, ys, 3, "blue", 2.0, "red", 5.0);
    TEST_ASSERT_TRUE(contem(f, "<path"));
    TEST_ASSERT_TRUE(contem(f, "animateMotion"));
    fclose(f);
}

void test_svg_percursoAnimado_marcadores_I_e_F(void) {
    FILE *f = tmpfile();
    double xs[] = {10.0, 20.0};
    double ys[] = {10.0, 20.0};
    svg_percursoAnimado(f, "rota2", xs, ys, 2, "blue", 2.0, "red", 3.0);
    TEST_ASSERT_TRUE(contem(f, ">I<"));
    TEST_ASSERT_TRUE(contem(f, ">F<"));
    fclose(f);
}

void test_svg_percursoAnimado_n_menor_que_2_nao_escreve(void) {
    FILE *f = tmpfile();
    double xs[] = {0.0};
    double ys[] = {0.0};
    svg_percursoAnimado(f, "rota3", xs, ys, 1, "blue", 2.0, "red", 5.0);
    rewind(f);
    char buf[256] = {0};
    fread(buf, 1, sizeof(buf) - 1, f);
    TEST_ASSERT_EQUAL_INT(0, (int)strlen(buf));
    fclose(f);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_svg_criar_retorna_nao_nulo);
    RUN_TEST(test_svg_criar_escreve_cabecalho);
    RUN_TEST(test_svg_fechar_escreve_fechamento);
    RUN_TEST(test_svg_fechar_null_nao_crasha);
    RUN_TEST(test_svg_retangulo);
    RUN_TEST(test_svg_circulo);
    RUN_TEST(test_svg_linha);
    RUN_TEST(test_svg_texto);
    RUN_TEST(test_svg_percursoAnimado_escreve_path_e_animacao);
    RUN_TEST(test_svg_percursoAnimado_marcadores_I_e_F);
    RUN_TEST(test_svg_percursoAnimado_n_menor_que_2_nao_escreve);
    return UNITY_END();
}
