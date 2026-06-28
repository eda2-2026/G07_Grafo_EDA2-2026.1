#ifndef EXPORT_H
#define EXPORT_H

#include "grafo.h"

/*
 * Pre-calcula BFS (menor numero de estados) e DFS (rota mais barata) para
 * TODOS os pares origem/destino e grava tudo em um arquivo JSON consumido
 * pela interface web. Retorna 1 em caso de sucesso.
 */
int export_gerar_json(const Grafo *g, const char *produto,
                      const char *caminho_saida);

#endif
