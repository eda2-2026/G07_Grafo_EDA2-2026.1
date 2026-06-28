#include "csv.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void trim_fim(char *texto) {
    int tamanho = (int)strlen(texto);
    while (tamanho > 0 &&
           (texto[tamanho - 1] == '\n' || texto[tamanho - 1] == '\r' ||
            texto[tamanho - 1] == ' ' || texto[tamanho - 1] == '\t')) {
        texto[--tamanho] = '\0';
    }
}

static void trim_inicio(char *texto) {
    int origem = 0;
    while (texto[origem] == ' ' || texto[origem] == '\t') origem++;
    if (origem > 0) {
        int destino = 0;
        while (texto[origem]) texto[destino++] = texto[origem++];
        texto[destino] = '\0';
    }
}

static void copiar_campo(char *destino, size_t tamanho, const char *origem) {
    if (tamanho == 0) return;
    strncpy(destino, origem, tamanho - 1);
    destino[tamanho - 1] = '\0';
}

static double converter_preco(const char *texto) {
    char buffer[32];
    int i = 0;
    while (*texto && i < (int)sizeof(buffer) - 1) {
        buffer[i++] = (*texto == ',') ? '.' : *texto;
        texto++;
    }
    buffer[i] = '\0';
    return atof(buffer);
}

static int quebrar_linha_csv(char *linha, char *campos[], int max_campos) {
    int quantidade = 0;
    char *atual = linha;
    int dentro_aspas = 0;

    while (quantidade < max_campos) {
        campos[quantidade++] = atual;
        while (*atual) {
            if (*atual == '"') {
                dentro_aspas = !dentro_aspas;
            } else if (*atual == ';' && !dentro_aspas) {
                break;
            }
            atual++;
        }
        if (*atual != ';') break;
        *atual = '\0';
        atual++;
        dentro_aspas = 0;
    }

    return quantidade;
}

static int parse_linha(char *linha, Registro *registro) {
    char *campos[16];
    int total_campos = quebrar_linha_csv(linha, campos, 16);
    if (total_campos < 16) return 0;

    for (int i = 0; i < total_campos; i++) {
        trim_fim(campos[i]);
        trim_inicio(campos[i]);
    }

    memset(registro, 0, sizeof(Registro));
    copiar_campo(registro->regiao, sizeof(registro->regiao), campos[0]);
    copiar_campo(registro->estado, sizeof(registro->estado), campos[1]);
    copiar_campo(registro->municipio, sizeof(registro->municipio), campos[2]);
    copiar_campo(registro->revenda, sizeof(registro->revenda), campos[3]);
    copiar_campo(registro->cnpj, sizeof(registro->cnpj), campos[4]);
    copiar_campo(registro->produto, sizeof(registro->produto), campos[10]);
    copiar_campo(registro->data, sizeof(registro->data), campos[11]);
    copiar_campo(registro->valor_venda_texto,
                 sizeof(registro->valor_venda_texto), campos[12]);
    copiar_campo(registro->bandeira, sizeof(registro->bandeira), campos[15]);
    registro->valor_venda = converter_preco(campos[12]);

    return registro->estado[0] != '\0' && registro->municipio[0] != '\0' &&
           registro->valor_venda > 0.0;
}

Registro *carregar_csv(const char *caminho, int *total) {
    FILE *arquivo = fopen(caminho, "r");
    if (!arquivo) {
        fprintf(stderr, "[csv] Nao foi possivel abrir '%s'.\n", caminho);
        return NULL;
    }

    Registro *registros = malloc(MAX_REGISTROS * sizeof(Registro));
    if (!registros) {
        fclose(arquivo);
        return NULL;
    }

    char linha[2048];
    int quantidade = 0;
    int primeira_linha = 1;

    while (fgets(linha, sizeof(linha), arquivo)) {
        if (primeira_linha) {
            primeira_linha = 0;
            continue;
        }

        if (quantidade >= MAX_REGISTROS) {
            fprintf(stderr, "[csv] Limite de %d registros atingido.\n",
                    MAX_REGISTROS);
            break;
        }

        if (parse_linha(linha, &registros[quantidade])) quantidade++;
    }

    fclose(arquivo);
    *total = quantidade;
    return registros;
}
