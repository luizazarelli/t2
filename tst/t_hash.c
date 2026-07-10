#include "unity/unity.h"
#include "../src/hash.h"
#include <stdlib.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

void test_hash_criar_retorna_nao_nulo(void) {
    Hash *h = hash_criar(8);
    TEST_ASSERT_NOT_NULL(h);
    hash_destruir(&h);
}

void test_hash_destruir_seta_null(void) {
    Hash *h = hash_criar(8);
    hash_destruir(&h);
    TEST_ASSERT_NULL(h);
}

void test_hash_inserir_e_buscar(void) {
    Hash *h = hash_criar(8);
    int val = 42;
    hash_inserir(h, "chave1", &val);
    TEST_ASSERT_EQUAL_PTR(&val, hash_buscar(h, "chave1"));
    hash_destruir(&h);
}

void test_hash_buscar_inexistente_retorna_null(void) {
    Hash *h = hash_criar(8);
    TEST_ASSERT_NULL(hash_buscar(h, "naoexiste"));
    hash_destruir(&h);
}

void test_hash_buscar_null_nao_crasha(void) {
    TEST_ASSERT_NULL(hash_buscar(NULL, "x"));
}

void test_hash_rehash_encontra_todos(void) {
    Hash *h = hash_criar(4);
    int vals[20];
    char chave[16];
    for (int i = 0; i < 20; i++) {
        vals[i] = i;
        snprintf(chave, sizeof(chave), "k%d", i);
        hash_inserir(h, chave, &vals[i]);
    }
    for (int i = 0; i < 20; i++) {
        snprintf(chave, sizeof(chave), "k%d", i);
        int *p = (int *)hash_buscar(h, chave);
        TEST_ASSERT_NOT_NULL(p);
        TEST_ASSERT_EQUAL_INT(i, *p);
    }
    hash_destruir(&h);
}

void test_hash_capacidade_retorna_valor(void) {
    Hash *h = hash_criar(16);
    TEST_ASSERT_EQUAL_INT(16, hash_capacidade(h));
    hash_destruir(&h);
}

void test_hash_capacidade_null_retorna_zero(void) {
    TEST_ASSERT_EQUAL_INT(0, hash_capacidade(NULL));
}

void test_hash_getSlot_itera_valores(void) {
    Hash *h = hash_criar(8);
    int a = 1, b = 2;
    hash_inserir(h, "aaa", &a);
    hash_inserir(h, "bbb", &b);
    int encontrados = 0;
    int cap = hash_capacidade(h);
    for (int i = 0; i < cap; i++)
        if (hash_getSlot(h, i) != NULL)
            encontrados++;
    TEST_ASSERT_EQUAL_INT(2, encontrados);
    hash_destruir(&h);
}

void test_hash_getSlot_indice_invalido_retorna_null(void) {
    Hash *h = hash_criar(8);
    TEST_ASSERT_NULL(hash_getSlot(h, -1));
    TEST_ASSERT_NULL(hash_getSlot(h, 8));
    hash_destruir(&h);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_hash_criar_retorna_nao_nulo);
    RUN_TEST(test_hash_destruir_seta_null);
    RUN_TEST(test_hash_inserir_e_buscar);
    RUN_TEST(test_hash_buscar_inexistente_retorna_null);
    RUN_TEST(test_hash_buscar_null_nao_crasha);
    RUN_TEST(test_hash_rehash_encontra_todos);
    RUN_TEST(test_hash_capacidade_retorna_valor);
    RUN_TEST(test_hash_capacidade_null_retorna_zero);
    RUN_TEST(test_hash_getSlot_itera_valores);
    RUN_TEST(test_hash_getSlot_indice_invalido_retorna_null);
    return UNITY_END();
}
