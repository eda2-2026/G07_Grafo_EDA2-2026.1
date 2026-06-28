#ifndef TIPOS_H
#define TIPOS_H

#define MAX_CAMPO 256
#define MAX_REGISTROS 60000

typedef struct {
    char regiao[8];
    char estado[4];
    char municipio[MAX_CAMPO];
    char revenda[MAX_CAMPO];
    char cnpj[32];
    char produto[32];
    char data[16];
    char valor_venda_texto[16];
    double valor_venda;
    char bandeira[64];
} Registro;

typedef struct {
    long long comparacoes;
    long long movimentacoes;
} MetricasOrdenacao;

typedef struct {
    long long comparacoes;
    long long rotacoes;
    long long insercoes;
    long long buscas;
    long long remocoes;
} MetricasArvore;

typedef int (*ComparadorRegistro)(const Registro *a, const Registro *b,
                                  MetricasOrdenacao *metricas);

typedef void (*AlgoritmoOrdenacao)(Registro *dados, int total,
                                   ComparadorRegistro comparar,
                                   MetricasOrdenacao *metricas);

typedef struct {
    const char *nome;
    const char *complexidade_media;
    const char *descricao;
    int limite_recomendado;
    AlgoritmoOrdenacao ordenar;
} MetodoOrdenacao;

#endif
