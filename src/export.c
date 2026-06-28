#include "export.h"
#include "precos.h"
#include "analise.h"

#include <stdio.h>

static void escrever_rota(FILE *f, const Grafo *g, const char *chave,
                          const Rota *r) {
    fprintf(f, "\"%s\":{", chave);
    fprintf(f, "\"encontrou\":%s,", r->encontrou ? "true" : "false");
    fprintf(f, "\"saltos\":%d,", r->tamanho > 0 ? r->tamanho - 1 : 0);
    fprintf(f, "\"custo\":%.3f,", r->custo);
    fprintf(f, "\"exploracoes\":%ld,", r->exploracoes);
    fprintf(f, "\"caminho\":[");
    for (int i = 0; i < r->tamanho; i++) {
        fprintf(f, "%s\"%s\"", i ? "," : "", g->estados[r->caminho[i]].sigla);
    }
    fprintf(f, "]}");
}

/* Escreve o objeto JS de um unico produto: estados, arestas, rotas e analise. */
static void escrever_dataset(FILE *f, const Grafo *g, const char *produto) {
    fprintf(f, "{\n");
    fprintf(f, "\"produto\":\"%s\",\n", produto);

    /* --- estados (vertices) --- */
    fprintf(f, "\"estados\":[\n");
    for (int i = 0; i < g->total; i++) {
        const Estado *e = &g->estados[i];
        fprintf(f, "  {\"sigla\":\"%s\",\"preco\":%.3f,\"amostras\":%ld,"
                   "\"grau\":%d}%s\n",
                e->sigla, e->preco, e->amostras, e->grau,
                i + 1 < g->total ? "," : "");
    }
    fprintf(f, "],\n");

    /* --- arestas (fronteiras, cada uma uma vez: a < b) --- */
    fprintf(f, "\"arestas\":[\n");
    int primeira = 1;
    for (int i = 0; i < g->total; i++) {
        const Estado *e = &g->estados[i];
        for (int k = 0; k < e->grau; k++) {
            int viz = e->vizinhos[k];
            if (viz > i) {
                fprintf(f, "%s[\"%s\",\"%s\"]", primeira ? "  " : ",\n  ",
                        e->sigla, g->estados[viz].sigla);
                primeira = 0;
            }
        }
    }
    fprintf(f, "\n],\n");

    /* --- rotas pre-calculadas para todos os pares --- */
    fprintf(f, "\"rotas\":{\n");
    int primeiro_par = 1;
    char chave[16];
    for (int o = 0; o < g->total; o++) {
        for (int d = 0; d < g->total; d++) {
            if (o == d) continue;
            Rota bfs = grafo_bfs(g, o, d);
            Rota barato = grafo_dfs_mais_barato(g, o, d);

            snprintf(chave, sizeof(chave), "%s|%s",
                     g->estados[o].sigla, g->estados[d].sigla);

            fprintf(f, "%s\"%s\":{", primeiro_par ? "  " : ",\n  ",
                    chave);
            escrever_rota(f, g, "bfs", &bfs);
            fprintf(f, ",");
            escrever_rota(f, g, "barato", &barato);
            fprintf(f, "}");
            primeiro_par = 0;
        }
    }
    fprintf(f, "\n},\n");

    /* --- analise estatistica do produto --- */
    AnaliseRotas a = analise_rotas(g);
    fprintf(f, "\"analise\":{");
    fprintf(f, "\"total\":%d,", a.total_pares);
    fprintf(f, "\"iguais\":%d,", a.pares_iguais);
    fprintf(f, "\"diferentes\":%d,", a.pares_diferentes);
    fprintf(f, "\"economiaMedia\":%.3f,", a.economia_media);
    fprintf(f, "\"economiaMaxima\":%.3f,", a.economia_maxima);
    fprintf(f, "\"parMaiorEconomia\":\"%s\"", a.par_maior_economia);
    fprintf(f, "}\n");

    fprintf(f, "}");
}

int export_gerar_json(Grafo *g, const Registro *registros, int total,
                      const char *produtos[], int num_produtos,
                      const char *caminho_saida) {
    FILE *f = fopen(caminho_saida, "w");
    if (!f) {
        fprintf(stderr, "[export] Nao foi possivel escrever '%s'.\n",
                caminho_saida);
        return 0;
    }

    /* Envolve o JSON em uma atribuicao JS para a pagina abrir direto do
       disco (file://) sem esbarrar no bloqueio de fetch dos navegadores. */
    fprintf(f, "window.DADOS = {\n");
    fprintf(f, "\"default\":\"%s\",\n", produtos[0]);
    fprintf(f, "\"produtos\":{\n");

    for (int p = 0; p < num_produtos; p++) {
        precos_calcular_por_estado(g, registros, total, produtos[p]);
        fprintf(f, "%s\"%s\":", p ? ",\n" : "", produtos[p]);
        escrever_dataset(f, g, produtos[p]);
    }

    fprintf(f, "\n}\n};\n");
    fclose(f);
    return 1;
}
