#include "hash.h"
#include <stdlib.h>
#include <string.h>

struct Slot {
    char *chave;
    void *valor;
};

struct Hash {
    int capacidade;
    int ocupados;
    struct Slot *slots;
};

static int calcular_hash(char *chave, int cap) {
    unsigned int h = 0;
    for (int i = 0; chave[i]; i++)
        h = h * 31 + (unsigned char)chave[i];
    return (int)(h % (unsigned int)cap);
}

static void redimensionar(Hash *h) {
    int nova_cap = h->capacidade * 2;
    struct Slot *novos = calloc(nova_cap, sizeof(struct Slot));
    if (novos == NULL)
        return;
    for (int i = 0; i < h->capacidade; i++) {
        if (h->slots[i].chave == NULL)
            continue;
        int j = calcular_hash(h->slots[i].chave, nova_cap);
        while (novos[j].chave != NULL)
            j = (j + 1) % nova_cap;
        novos[j] = h->slots[i];
    }
    free(h->slots);
    h->slots = novos;
    h->capacidade = nova_cap;
}

Hash *hash_criar(int capacidade) {
    Hash *h = malloc(sizeof(Hash));
    if (h == NULL)
        return NULL;
    h->capacidade = capacidade > 0 ? capacidade : 16;
    h->ocupados   = 0;
    h->slots      = calloc(h->capacidade, sizeof(struct Slot));
    if (h->slots == NULL) {
        free(h);
        return NULL;
    }
    return h;
}

void hash_destruir(Hash **h) {
    if (h == NULL || *h == NULL)
        return;
    Hash *t = *h;
    for (int i = 0; i < t->capacidade; i++)
        free(t->slots[i].chave);
    free(t->slots);
    free(t);
    *h = NULL;
}

void hash_inserir(Hash *h, char *chave, void *valor) {
    if (h == NULL || chave == NULL || valor == NULL)
        return;
    if (h->ocupados * 10 >= h->capacidade * 7)
        redimensionar(h);
    int i = calcular_hash(chave, h->capacidade);
    while (h->slots[i].chave != NULL)
        i = (i + 1) % h->capacidade;
    h->slots[i].chave = malloc(strlen(chave) + 1);
    if (h->slots[i].chave == NULL)
        return;
    strcpy(h->slots[i].chave, chave);
    h->slots[i].valor = valor;
    h->ocupados++;
}

void *hash_buscar(Hash *h, char *chave) {
    if (h == NULL || chave == NULL)
        return NULL;
    int i = calcular_hash(chave, h->capacidade);
    while (h->slots[i].chave != NULL) {
        if (strcmp(h->slots[i].chave, chave) == 0)
            return h->slots[i].valor;
        i = (i + 1) % h->capacidade;
    }
    return NULL;
}

int hash_capacidade(Hash *h) {
    return h ? h->capacidade : 0;
}

void *hash_getSlot(Hash *h, int i) {
    if (h == NULL || i < 0 || i >= h->capacidade)
        return NULL;
    return h->slots[i].valor;
}
