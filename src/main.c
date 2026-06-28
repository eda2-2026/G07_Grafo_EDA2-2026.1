#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csv.h"
#include "grafo.h"
#include "precos.h"
#include "export.h"

#define CSV_PADRAO "02-cados-abertos-preco-gasolina-etanol.csv"
#define JSON_SAIDA "web/rotas.js"

int main(int argc, char *argv[]) {
    const char *caminho_csv = (argc > 1) ? argv[1] : CSV_PADRAO;
    const char *produto = (argc > 2) ? argv[2] : "GASOLINA";

    printf("=== Grafos: rotas de combustivel entre estados ===\n");
    printf("Base.....: %s\n", caminho_csv);
    printf("Produto..: %s\n\n", produto);

    int total = 0;
    Registro *registros = carregar_csv(caminho_csv, &total);
    if (!registros) return 1;
    printf("Registros carregados: %d\n", total);

    Grafo g;
    grafo_inicializar(&g);
    precos_calcular_por_estado(&g, registros, total, produto);

    /* Resumo no terminal (estilo dos trabalhos anteriores). */
    printf("\nPreco medio de %s por estado:\n", produto);
    printf("%-6s %-10s %-10s %s\n", "UF", "preco", "amostras", "fronteiras");
    for (int i = 0; i < g.total; i++) {
        const Estado *e = &g.estados[i];
        printf("%-6s R$ %-7.3f %-10ld %d\n",
               e->sigla, e->preco, e->amostras, e->grau);
    }

    /* Exemplo demonstrativo no terminal: DF -> AM. */
    int o = grafo_indice(&g, "DF");
    int d = grafo_indice(&g, "AM");
    if (o >= 0 && d >= 0) {
        Rota bfs = grafo_bfs(&g, o, d);
        Rota barato = grafo_dfs_mais_barato(&g, o, d);

        printf("\nExemplo DF -> AM:\n");
        printf("  BFS (menos estados): ");
        for (int i = 0; i < bfs.tamanho; i++)
            printf("%s%s", i ? " -> " : "", g.estados[bfs.caminho[i]].sigla);
        printf(" | %d saltos | custo R$ %.2f\n", bfs.tamanho - 1, bfs.custo);

        printf("  DFS (mais barata)..: ");
        for (int i = 0; i < barato.tamanho; i++)
            printf("%s%s", i ? " -> " : "", g.estados[barato.caminho[i]].sigla);
        printf(" | %d saltos | custo R$ %.2f | %ld caminhos avaliados\n",
               barato.tamanho - 1, barato.custo, barato.exploracoes);
    }

    if (export_gerar_json(&g, produto, JSON_SAIDA)) {
        printf("\nJSON gerado em: %s\n", JSON_SAIDA);
        printf("Abra web/index.html no navegador para usar a interface.\n");
    }

    free(registros);
    return 0;
}
