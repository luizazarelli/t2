#include "unity/unity.h"
#include "../src/libs.h"
#include <stdlib.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

void test_newString_conteudo(void) {
    char *s = newString("bitnopolis");
    TEST_ASSERT_EQUAL_STRING("bitnopolis", s);
    free(s);
}

void test_newString_ponteiro_diferente(void) {
    char *original = "bitnopolis";
    char *copia = newString(original);
    TEST_ASSERT_NOT_EQUAL(original, copia);
    free(copia);
}

void test_newString_null(void) {
    TEST_ASSERT_NULL(newString(NULL));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_newString_conteudo);
    RUN_TEST(test_newString_ponteiro_diferente);
    RUN_TEST(test_newString_null);
    return UNITY_END();
}
