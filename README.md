# G07_Grafo_EDA2-2026.1

## Alunos
| Matricula | Aluno |
| -- | -- |
| 222021826 | Victor Leandro Rocha de Assis |
| 231036980 | Pedro Luiz Fonseca da Silva |

## Sobre
Projeto em linguagem C que modela um **grafo dos estados brasileiros** para
encontrar rotas de viagem usando a mesma base dos trabalhos anteriores,
`02-cados-abertos-preco-gasolina-etanol.csv`. O programa carrega o CSV, calcula
o preco medio da gasolina por estado, monta o grafo de fronteiras e pre-calcula
as rotas de todos os pares origem/destino, exportando o resultado para uma
interface web interativa.

## Modelagem do grafo
- **Vertices**: as 27 unidades federativas (UFs).
- **Arestas**: fronteiras geograficas entre estados (grafo nao direcionado,
  51 arestas). A topologia e fixa e conhecida, entao nao depende de dados
  externos de distancia.
- **Peso do vertice**: preco medio da gasolina no estado, calculado a partir
  dos registros do CSV.
- **Representacao**: lista de adjacencia (`Estado.vizinhos`).

## Algoritmos (apenas BFS, DFS e representacao, conforme a ementa)

### BFS — rota com menos estados
Busca em largura sobre o grafo sem peso. Devolve o caminho com o **menor numero
de estados atravessados** (caminho minimo em arestas) entre origem e destino.
Custo `O(V + E)`.

### DFS com poda — rota mais barata
Busca em profundidade que enumera caminhos simples e guarda o de **menor custo
total de gasolina** (soma do preco medio dos estados do caminho). Como todos os
precos sao positivos, qualquer ramo cujo custo parcial ja supere a melhor rota
encontrada e podado (estrategia *branch-and-bound*), o que mantem a busca
rapida mesmo enumerando caminhos.

### Observacao didatica
Comparar as duas rotas mostra que **a rota mais curta nem sempre e a mais
barata**: das 702 rotas possiveis, em 230 pares (32,8%) a rota com menos
estados passa por estados de gasolina cara e a rota mais barata compensa com um
pequeno desvio. Esse numero nao e mais "chutado": o modulo `analise.c` percorre
todos os pares e calcula automaticamente quantos diferem, a economia media e o
par de maior economia (impressos no terminal e embutidos no `web/rotas.js`).

### Representacao do grafo
Alem da **lista de adjacencia** usada nas buscas, o programa imprime a
**matriz de adjacencia** 27x27 no terminal (`grafo_imprimir_matriz`), cobrindo
as duas formas classicas de representacao de grafos vistas em aula.

## Interface web
A pasta `web/` contem a interface (HTML + biblioteca `vis-network`):
- seletor de **combustivel** (GASOLINA, ETANOL e GASOLINA ADITIVADA), que
  recolore o grafo e troca as rotas sem recarregar a pagina;
- dois seletores: estado de origem e de destino;
- botao que traça a rota e **anima** o caminho no grafo;
- nos coloridos por preco (verde = barato, vermelho = caro);
- painel com o caminho, custo total, fronteiras e metricas das buscas;
- resumo da analise (quantos pares têm rota mais barata diferente da mais curta).

Os dados consumidos pela pagina ficam em `web/rotas.js`
(`window.DADOS = {...}`), gerado pelo programa em C. O formato JS evita o
bloqueio de `fetch` em arquivos locais, entao **basta abrir `web/index.html`**
no navegador (requer internet apenas para carregar a biblioteca de grafos).

## Compilacao e execucao
Com GCC instalado, na raiz do projeto:

```bash
make run            # compila, gera web/rotas.js e imprime um resumo
```

Depois, abra `web/index.html` no navegador.

O `web/rotas.js` ja sai com os tres combustiveis (GASOLINA, ETANOL e GASOLINA
ADITIVADA) — basta trocar no seletor da pagina. O argumento de produto apenas
define qual aparece por padrao e qual e usado no resumo do terminal:

```bash
./grafo_combustivel caminho/do/arquivo.csv ETANOL
```

No Windows com MinGW:

```cmd
mingw32-make run
```

## Estrutura
```
src/
  tipos.h          estrutura de Registro (reaproveitada dos trabalhos anteriores)
  csv.h / csv.c    carregador do CSV (reaproveitado)
  grafo.h / grafo.c    grafo dos estados, BFS, DFS e matriz de adjacencia
  precos.h / precos.c  preco medio do combustivel por estado
  analise.h / analise.c  estatistica BFS x DFS em todos os pares
  export.h / export.c  geracao do web/rotas.js (varios combustiveis)
  main.c           orquestra o fluxo e imprime o resumo
web/
  index.html  style.css  app.js   interface
  rotas.js    (gerado)             dados pre-calculados pelo C
```

## Video de explicacao
https://youtu.be/ofN6ROfYnxA
