#ifndef GRAFO_H
#define GRAFO_H

/*
 * Grafo dos estados brasileiros (27 UFs).
 * - Vertices  = unidades federativas.
 * - Arestas   = fronteiras geograficas (nao direcionadas).
 * - Peso      = preco medio do combustivel no estado (vem da base CSV).
 *
 * Implementacao por LISTA DE ADJACENCIA, conforme o conteudo de
 * representacao de grafos visto em aula.
 */

#define MAX_ESTADOS 27
#define MAX_VIZINHOS 8   /* a Bahia faz fronteira com 8 estados (maior grau) */

typedef struct {
    char sigla[4];        /* "SP", "DF", ... */
    double preco;         /* preco medio do combustivel no estado */
    long amostras;        /* quantos registros do CSV entraram na media */
    int vizinhos[MAX_VIZINHOS];
    int grau;
} Estado;

typedef struct {
    Estado estados[MAX_ESTADOS];
    int total;
} Grafo;

/* Resultado de uma busca de rota (BFS ou DFS). */
typedef struct {
    int caminho[MAX_ESTADOS]; /* sequencia de indices de estados */
    int tamanho;              /* quantos estados no caminho */
    double custo;             /* soma dos precos dos estados do caminho */
    long exploracoes;         /* metrica: nos/caminhos visitados na busca */
    int encontrou;
} Rota;

/* Monta o grafo com as 27 UFs e as arestas de fronteira (topologia fixa). */
void grafo_inicializar(Grafo *g);

/* Indice do estado pela sigla, ou -1 se nao existir. */
int grafo_indice(const Grafo *g, const char *sigla);

/* 1 se os estados a e b sao vizinhos (ha aresta), 0 caso contrario. */
int grafo_sao_vizinhos(const Grafo *g, int a, int b);

/*
 * Imprime no terminal a MATRIZ DE ADJACENCIA do grafo (1 = fronteira).
 * Complementa a lista de adjacencia mostrando a outra forma classica de
 * representacao de grafos vista em aula.
 */
void grafo_imprimir_matriz(const Grafo *g);

/*
 * BFS: caminho com o MENOR NUMERO DE ESTADOS entre origem e destino.
 * (caminho minimo em arestas num grafo sem peso)
 */
Rota grafo_bfs(const Grafo *g, int origem, int destino);

/*
 * DFS com poda (branch-and-bound): enumera caminhos simples e devolve o de
 * MENOR CUSTO total de combustivel. Como os precos sao positivos, podamos
 * qualquer ramo cujo custo parcial ja supere a melhor rota encontrada.
 */
Rota grafo_dfs_mais_barato(const Grafo *g, int origem, int destino);

#endif
