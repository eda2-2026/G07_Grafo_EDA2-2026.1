#include "grafo.h"

#include <string.h>
#include <float.h>

/* As 27 unidades federativas (a ordem define o indice de cada estado). */
static const char *SIGLAS[MAX_ESTADOS] = {
    "AC", "AL", "AM", "AP", "BA", "CE", "DF", "ES", "GO",
    "MA", "MG", "MS", "MT", "PA", "PB", "PE", "PI", "PR",
    "RJ", "RN", "RO", "RR", "RS", "SC", "SE", "SP", "TO"
};

/*
 * Fronteiras entre estados (cada par listado uma vez). A funcao de
 * inicializacao insere a aresta nos dois sentidos, formando um grafo
 * nao direcionado. Fonte: divisas territoriais oficiais do Brasil.
 */
static const char *FRONTEIRAS[][2] = {
    {"AC", "AM"}, {"AC", "RO"},
    {"AM", "RO"}, {"AM", "RR"}, {"AM", "PA"}, {"AM", "MT"},
    {"RO", "MT"}, {"RR", "PA"}, {"AP", "PA"},
    {"PA", "MT"}, {"PA", "TO"}, {"PA", "MA"},
    {"MT", "TO"}, {"MT", "GO"}, {"MT", "MS"},
    {"TO", "MA"}, {"TO", "PI"}, {"TO", "BA"}, {"TO", "GO"},
    {"MA", "PI"},
    {"PI", "CE"}, {"PI", "PE"}, {"PI", "BA"},
    {"CE", "RN"}, {"CE", "PB"}, {"CE", "PE"},
    {"RN", "PB"}, {"RN", "PE"}, {"PB", "PE"},
    {"PE", "AL"}, {"PE", "BA"},
    {"AL", "SE"}, {"AL", "BA"}, {"SE", "BA"},
    {"BA", "GO"}, {"BA", "MG"}, {"BA", "ES"},
    {"GO", "DF"}, {"GO", "MG"}, {"GO", "MS"},
    {"MG", "ES"}, {"MG", "RJ"}, {"MG", "SP"}, {"MG", "MS"},
    {"ES", "RJ"}, {"RJ", "SP"},
    {"SP", "MS"}, {"SP", "PR"}, {"MS", "PR"},
    {"PR", "SC"}, {"SC", "RS"}
};

int grafo_indice(const Grafo *g, const char *sigla) {
    for (int i = 0; i < g->total; i++) {
        if (strcmp(g->estados[i].sigla, sigla) == 0) return i;
    }
    return -1;
}

static void adicionar_aresta(Grafo *g, int a, int b) {
    Estado *ea = &g->estados[a];
    Estado *eb = &g->estados[b];
    if (ea->grau < MAX_VIZINHOS) ea->vizinhos[ea->grau++] = b;
    if (eb->grau < MAX_VIZINHOS) eb->vizinhos[eb->grau++] = a;
}

void grafo_inicializar(Grafo *g) {
    g->total = MAX_ESTADOS;
    for (int i = 0; i < MAX_ESTADOS; i++) {
        strcpy(g->estados[i].sigla, SIGLAS[i]);
        g->estados[i].preco = 0.0;
        g->estados[i].amostras = 0;
        g->estados[i].grau = 0;
    }

    int n = (int)(sizeof(FRONTEIRAS) / sizeof(FRONTEIRAS[0]));
    for (int i = 0; i < n; i++) {
        int a = grafo_indice(g, FRONTEIRAS[i][0]);
        int b = grafo_indice(g, FRONTEIRAS[i][1]);
        if (a >= 0 && b >= 0) adicionar_aresta(g, a, b);
    }
}

/* ----------------------------------------------------------------------- */
/* BFS: menor numero de estados (caminho minimo em arestas)                */
/* ----------------------------------------------------------------------- */
Rota grafo_bfs(const Grafo *g, int origem, int destino) {
    Rota rota;
    memset(&rota, 0, sizeof(rota));

    int fila[MAX_ESTADOS];
    int inicio = 0, fim = 0;
    int visitado[MAX_ESTADOS] = {0};
    int anterior[MAX_ESTADOS];
    for (int i = 0; i < MAX_ESTADOS; i++) anterior[i] = -1;

    fila[fim++] = origem;
    visitado[origem] = 1;

    while (inicio < fim) {
        int atual = fila[inicio++];
        rota.exploracoes++;
        if (atual == destino) break;

        const Estado *e = &g->estados[atual];
        for (int k = 0; k < e->grau; k++) {
            int viz = e->vizinhos[k];
            if (!visitado[viz]) {
                visitado[viz] = 1;
                anterior[viz] = atual;
                fila[fim++] = viz;
            }
        }
    }

    if (!visitado[destino]) {
        rota.encontrou = 0;
        return rota;
    }

    /* reconstrucao do caminho de tras pra frente */
    int pilha[MAX_ESTADOS];
    int topo = 0;
    for (int v = destino; v != -1; v = anterior[v]) pilha[topo++] = v;

    rota.tamanho = topo;
    rota.custo = 0.0;
    for (int i = 0; i < topo; i++) {
        int estado = pilha[topo - 1 - i];
        rota.caminho[i] = estado;
        rota.custo += g->estados[estado].preco;
    }
    rota.encontrou = 1;
    return rota;
}

/* ----------------------------------------------------------------------- */
/* DFS com poda (branch-and-bound): rota de menor custo de combustivel      */
/* ----------------------------------------------------------------------- */
typedef struct {
    const Grafo *g;
    int destino;
    int visitado[MAX_ESTADOS];
    int caminho[MAX_ESTADOS];
    int profundidade;
    double melhor_custo;
    Rota melhor;
    long exploracoes;
} EstadoDFS;

static void dfs_rec(EstadoDFS *s, int atual, double custo_atual) {
    s->exploracoes++;

    /* poda: precos sao positivos, entao custo so cresce */
    if (custo_atual >= s->melhor_custo) return;

    if (atual == s->destino) {
        s->melhor_custo = custo_atual;
        s->melhor.tamanho = s->profundidade;
        s->melhor.custo = custo_atual;
        s->melhor.encontrou = 1;
        for (int i = 0; i < s->profundidade; i++) {
            s->melhor.caminho[i] = s->caminho[i];
        }
        return;
    }

    const Estado *e = &s->g->estados[atual];
    for (int k = 0; k < e->grau; k++) {
        int viz = e->vizinhos[k];
        if (s->visitado[viz]) continue;
        s->visitado[viz] = 1;
        s->caminho[s->profundidade++] = viz;
        dfs_rec(s, viz, custo_atual + s->g->estados[viz].preco);
        s->profundidade--;
        s->visitado[viz] = 0;
    }
}

Rota grafo_dfs_mais_barato(const Grafo *g, int origem, int destino) {
    EstadoDFS s;
    memset(&s, 0, sizeof(s));
    s.g = g;
    s.destino = destino;
    s.melhor_custo = DBL_MAX;
    s.melhor.encontrou = 0;

    s.visitado[origem] = 1;
    s.caminho[s.profundidade++] = origem;
    dfs_rec(&s, origem, g->estados[origem].preco);

    s.melhor.exploracoes = s.exploracoes;
    return s.melhor;
}
