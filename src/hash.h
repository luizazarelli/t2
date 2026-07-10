#ifndef HASH_H
#define HASH_H

typedef struct Hash Hash;

Hash *hash_criar(int capacidade);
/*
    aloca e retorna uma tabela hash vazia com endereçamento aberto e sondagem linear.
    rehash automatico quando fator de carga ultrapassa 70%.
    entrada: capacidade: numero inicial de buckets.
    saida:   ponteiro para Hash criado, ou NULL em caso de falha.
*/

void hash_destruir(Hash **h);
/*
    libera a estrutura da tabela e as copias internas das chaves.
    nao libera os valores armazenados (responsabilidade do chamador).
    entrada: h: endereco do ponteiro para Hash.
    saida:   *h atribuido NULL.
*/

void hash_inserir(Hash *h, char *chave, void *valor);
/*
    insere o par (chave, valor) na tabela usando sondagem linear.
    a chave e copiada internamente; o valor e armazenado por referencia.
    entrada: h: tabela; chave: string identificadora; valor: dado a armazenar.
    saida:   nenhuma. nao faz nada se h, chave ou valor forem NULL.
*/

void *hash_buscar(Hash *h, char *chave);
/*
    busca e retorna o valor associado a chave usando sondagem linear.
    entrada: h: tabela; chave: string identificadora.
    saida:   ponteiro para o valor encontrado, ou NULL se nao existir.
*/

int hash_capacidade(Hash *h);
/*
    retorna o numero total de slots da tabela (incluindo vazios).
    entrada: h: tabela.
    saida:   capacidade, ou 0 se h for NULL.
*/

void *hash_getSlot(Hash *h, int i);
/*
    retorna o valor no slot i, ou NULL se o slot estiver vazio.
    permite iterar sobre todos os valores: for i in [0, hash_capacidade(h)).
    entrada: h: tabela; i: indice do slot (0 <= i < capacidade).
    saida:   ponteiro para o valor, ou NULL se vazio ou indice invalido.
*/

#endif
