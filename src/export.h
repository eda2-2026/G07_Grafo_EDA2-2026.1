#ifndef EXPORT_H
#define EXPORT_H

#include "grafo.h"
#include "tipos.h"

/*
 * Para cada produto informado (ex.: "GASOLINA", "ETANOL"), recalcula o preco
 * por estado, pre-calcula BFS (menor numero de estados) e DFS (rota mais
 * barata) para TODOS os pares origem/destino e grava tudo num unico arquivo
 * JSON/JS consumido pela interface web. O primeiro produto da lista vira o
 * combustivel exibido por padrao. Retorna 1 em caso de sucesso.
 *
 * Observacao: `g` e modificado (os precos sao recalculados a cada produto).
 */
int export_gerar_json(Grafo *g, const Registro *registros, int total,
                      const char *produtos[], int num_produtos,
                      const char *caminho_saida);

#endif
