#ifndef ANALISE_H
#define ANALISE_H

#include "grafo.h"

/*
 * Estatisticas comparando, para todos os pares origem/destino, a rota com
 * menos estados (BFS) com a rota mais barata (DFS). Serve para quantificar a
 * observacao didatica do trabalho: "a rota mais curta nem sempre e a mais
 * barata".
 */
typedef struct {
    int total_pares;                 /* pares com rota valida (origem != destino) */
    int pares_iguais;                /* BFS e DFS encontram o mesmo caminho */
    int pares_diferentes;            /* a rota mais barata difere da mais curta */
    double economia_total;           /* soma das economias dos pares diferentes */
    double economia_media;           /* economia media nos pares diferentes */
    double economia_maxima;          /* maior economia individual */
    char par_maior_economia[16];     /* "XX -> YY" do par de maior economia */
    int estados_extra_maior;         /* estados a mais na rota barata desse par */
} AnaliseRotas;

/* Percorre todos os pares do grafo e calcula as estatisticas acima. */
AnaliseRotas analise_rotas(const Grafo *g);

/* Imprime um resumo das estatisticas no terminal. */
void analise_imprimir(const AnaliseRotas *a);

#endif
