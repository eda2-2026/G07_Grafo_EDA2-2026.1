#include "precos.h"

#include <string.h>

void precos_calcular_por_estado(Grafo *g, const Registro *registros,
                                int total, const char *produto) {
    double soma[MAX_ESTADOS] = {0};
    long contagem[MAX_ESTADOS] = {0};

    double soma_geral = 0.0;
    long contagem_geral = 0;

    for (int i = 0; i < total; i++) {
        const Registro *r = &registros[i];
        if (strcmp(r->produto, produto) != 0) continue;

        int idx = grafo_indice(g, r->estado);
        if (idx < 0) continue;

        soma[idx] += r->valor_venda;
        contagem[idx]++;
        soma_geral += r->valor_venda;
        contagem_geral++;
    }

    double media_geral = contagem_geral > 0 ? soma_geral / contagem_geral : 0.0;

    for (int i = 0; i < g->total; i++) {
        if (contagem[i] > 0) {
            g->estados[i].preco = soma[i] / contagem[i];
            g->estados[i].amostras = contagem[i];
        } else {
            /* sem amostra: usa a media nacional para o estado continuar
               navegavel na busca de rota */
            g->estados[i].preco = media_geral;
            g->estados[i].amostras = 0;
        }
    }
}
