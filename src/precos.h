#ifndef PRECOS_H
#define PRECOS_H

#include "tipos.h"
#include "grafo.h"

/*
 * Percorre os registros do CSV e preenche, para cada estado do grafo, o preco
 * medio do combustivel informado em `produto` (ex.: "GASOLINA"). Estados sem
 * nenhuma amostra ficam com a media geral do pais, para nao quebrar a busca.
 */
void precos_calcular_por_estado(Grafo *g, const Registro *registros,
                                int total, const char *produto);

#endif
