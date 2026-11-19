/*
 * ============================================================================
 * Universidade Presbiteriana Mackenzie - Faculdade de Computacao e Informatica
 * Disciplina: Teoria dos Grafos - Turma 6o D
 * Professor: Prof. Dr. Ivan Carlos Alcantara de Oliveira
 * Integrante: Gabriel Tortolio Fonseca - RA: 10416751
 * E-mail: 10416751@mackenzista.com.br
 *
 * Arquivo: grafo.h
 * Sintese:
 *   Declaracoes da estrutura de dados de Grafo (Matriz de Adjacencia) e dos
 *   prototipos das funcoes de manipulacao, analise estrutural (conexidade,
 *   graus, coloracao/biparticao, euleriano, hamiltoniano) e resolucao do
 *   problema de emparelhamento ponderado (1-para-1 e capacitado / b-matching).
 *
 * Historico de alteracoes:
 *   - 2026-09-20 | Gabriel Tortolio Fonseca | Criacao da estrutura base e operacoes da Parte 2.
 *   - 2026-11-19 | Gabriel Tortolio Fonseca | Inclusao dos algoritmos da Parte 3: emparelhamento
 *     ponderado (1-para-1 e b-matching via Fluxo Maximo de Custo Minimo), analise de graus,
 *     2-coloracao/Welsh-Powell, verificacao Euleriana e verificacao Hamiltoniana.
 * ============================================================================
 */

#ifndef GRAFO_H
#define GRAFO_H

#define MAX_VERTICES 200
#define MAX_ROTULO   32

typedef struct {
    int tipo;                              /* Tipo do grafo (2 = nao orientado com peso) */
    int num_vertices;                      /* Quantidade atual de vertices ativos */
    int num_arestas;                       /* Quantidade atual de arestas */
    int ativo[MAX_VERTICES];               /* 1 se o vertice existe, 0 caso removido */
    char rotulo[MAX_VERTICES][MAX_ROTULO]; /* Rotulo textual (ex: "V01", "A01") */
    int capacidade[MAX_VERTICES];          /* Capacidade semanal de atendimento (voluntarios) */
    int adj[MAX_VERTICES][MAX_VERTICES];   /* Matriz de adjacencia (0 = sem aresta; >0 = peso) */
} Grafo;

/* Inicializacao e E/S */
void inicializar_grafo(Grafo *g);
int ler_arquivo_grafo(Grafo *g, const char *nome_arquivo);
int carregar_capacidades_csv(Grafo *g, const char *nome_csv);
int gravar_arquivo_grafo(const Grafo *g, const char *nome_arquivo);
void mostrar_conteudo_arquivo(const char *nome_arquivo);
void mostrar_grafo(const Grafo *g);

/* Operacoes basicas (Parte 2) */
int inserir_vertice(Grafo *g, int id, const char *rotulo);
int inserir_aresta(Grafo *g, int u, int v, int peso);
int remover_vertice(Grafo *g, int id);
int remover_aresta(Grafo *g, int u, int v);
void analisar_conexidade(const Grafo *g);

/* Solucao do problema - Item 1 (Parte 3) */
void resolver_emparelhamento(const Grafo *g);

/* Tecnicas de analise estrutural - Item 2 (Parte 3) */
void analisar_graus(const Grafo *g);
void analisar_coloracao_biparticao(const Grafo *g);
void verificar_euleriano(const Grafo *g);
void verificar_hamiltoniano(const Grafo *g);

#endif /* GRAFO_H */
