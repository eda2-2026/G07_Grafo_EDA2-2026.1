#include "analise.h"

#include <stdio.h>
#include <string.h>

static int mesmo_caminho(const Rota *a, const Rota *b) {
    if (a->tamanho != b->tamanho) return 0;
    for (int i = 0; i < a->tamanho; i++) {
        if (a->caminho[i] != b->caminho[i]) return 0;
    }
    return 1;
}

AnaliseRotas analise_rotas(const Grafo *g) {
    AnaliseRotas r;
    memset(&r, 0, sizeof(r));

    for (int o = 0; o < g->total; o++) {
        for (int d = 0; d < g->total; d++) {
            if (o == d) continue;

            Rota bfs = grafo_bfs(g, o, d);
            Rota barato = grafo_dfs_mais_barato(g, o, d);
            if (!bfs.encontrou || !barato.encontrou) continue;

            r.total_pares++;

            if (mesmo_caminho(&bfs, &barato)) {
                r.pares_iguais++;
                continue;
            }

            r.pares_diferentes++;
            double economia = bfs.custo - barato.custo; /* >= 0: barato e o minimo */
            r.economia_total += economia;

            if (economia > r.economia_maxima) {
                r.economia_maxima = economia;
                r.estados_extra_maior = barato.tamanho - bfs.tamanho;
                snprintf(r.par_maior_economia, sizeof(r.par_maior_economia),
                         "%s -> %s",
                         g->estados[o].sigla, g->estados[d].sigla);
            }
        }
    }

    if (r.pares_diferentes > 0) {
        r.economia_media = r.economia_total / r.pares_diferentes;
    }
    return r;
}

void analise_imprimir(const AnaliseRotas *a) {
    printf("\nAnalise das rotas (BFS x DFS) em todos os pares:\n");
    printf("  Pares analisados.................: %d\n", a->total_pares);
    printf("  Rota mais curta = mais barata....: %d\n", a->pares_iguais);
    printf("  Rota mais barata desvia da curta.: %d", a->pares_diferentes);
    if (a->total_pares > 0) {
        printf(" (%.1f%%)", 100.0 * a->pares_diferentes / a->total_pares);
    }
    printf("\n");
    printf("  Economia media nos desvios.......: R$ %.2f\n", a->economia_media);
    printf("  Maior economia individual........: R$ %.2f (%s, %d estado(s) a mais)\n",
           a->economia_maxima, a->par_maior_economia, a->estados_extra_maior);
}
