
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "grafo.h"

#define INF 1000000000

typedef struct {
    int to;
    int cap;
    int cost;
    int rev;
} FluxoAresta;

static int eh_voluntario(const Grafo *g, int u) {
    return g->ativo[u] && (g->rotulo[u][0] == 'V' || g->rotulo[u][0] == 'v');
}

static int eh_aluno(const Grafo *g, int u) {
    return g->ativo[u] && (g->rotulo[u][0] == 'A' || g->rotulo[u][0] == 'a');
}

void inicializar_grafo(Grafo *g) {
    int i, j;
    g->tipo = 2;
    g->num_vertices = 0;
    g->num_arestas = 0;
    for (i = 0; i < MAX_VERTICES; i++) {
        g->ativo[i] = 0;
        g->rotulo[i][0] = '\0';
        g->capacidade[i] = 1;
        for (j = 0; j < MAX_VERTICES; j++) {
            g->adj[i][j] = 0;
        }
    }
}

int ler_arquivo_grafo(Grafo *g, const char *nome_arquivo) {
    FILE *fp = fopen(nome_arquivo, "r");
    int tipo, n, m, i, id, u, v, peso;
    char rot[MAX_ROTULO];

    if (!fp) {
        printf("Erro: Arquivo %s vazio ou inexistente.\n", nome_arquivo);
        return 0;
    }

    inicializar_grafo(g);

    if (fscanf(fp, "%d", &tipo) != 1 || fscanf(fp, "%d", &n) != 1) {
        printf("Erro: Formato invalido no cabecalho de %s.\n", nome_arquivo);
        fclose(fp);
        return 0;
    }
    g->tipo = tipo;

    for (i = 0; i < n; i++) {
        if (fscanf(fp, "%d \"%31[^\"]\"", &id, rot) != 2) {
            printf("Erro ao ler vertice %d em %s.\n", i, nome_arquivo);
            fclose(fp);
            return 0;
        }
        if (id >= 0 && id < MAX_VERTICES) {
            g->ativo[id] = 1;
            snprintf(g->rotulo[id], MAX_ROTULO, "%s", rot);
            g->capacidade[id] = (rot[0] == 'V' || rot[0] == 'v') ? 2 : 1;
            g->num_vertices++;
        }
    }

    if (fscanf(fp, "%d", &m) != 1) {
        printf("Erro ao ler quantidade de arestas.\n");
        fclose(fp);
        return 0;
    }

    for (i = 0; i < m; i++) {
        if (fscanf(fp, "%d %d %d", &u, &v, &peso) != 3) {
            break;
        }
        if (u >= 0 && u < MAX_VERTICES && v >= 0 && v < MAX_VERTICES &&
            g->ativo[u] && g->ativo[v] && u != v && g->adj[u][v] == 0) {
            g->adj[u][v] = peso;
            g->adj[v][u] = peso;
            g->num_arestas++;
        }
    }

    fclose(fp);
    carregar_capacidades_csv(g, "dados_voluntarios.csv");
    printf("Grafo lido com sucesso! Vertices: %d, Arestas: %d\n",
           g->num_vertices, g->num_arestas);
    return 1;
}

int carregar_capacidades_csv(Grafo *g, const char *nome_csv) {
    FILE *fp = fopen(nome_csv, "r");
    char linha[512];
    int carregados = 0;

    if (!fp) {
        return 0;
    }

    /* Descarta cabecalho CSV */
    if (!fgets(linha, sizeof(linha), fp)) {
        fclose(fp);
        return 0;
    }

    while (fgets(linha, sizeof(linha), fp)) {
        int id, cap;
        char codigo[32], perfil[128];
        if (sscanf(linha, "%d,%31[^,],%127[^,],%d", &id, codigo, perfil, &cap) == 4) {
            if (id >= 0 && id < MAX_VERTICES && g->ativo[id] && cap > 0) {
                g->capacidade[id] = cap;
                carregados++;
            }
        }
    }

    fclose(fp);
    return carregados;
}

int gravar_arquivo_grafo(const Grafo *g, const char *nome_arquivo) {
    FILE *fp = fopen(nome_arquivo, "w");
    int i, j;

    if (!fp) {
        printf("Erro ao abrir %s para gravacao.\n", nome_arquivo);
        return 0;
    }

    fprintf(fp, "%d\n", g->tipo);
    fprintf(fp, "%d\n", g->num_vertices);
    for (i = 0; i < MAX_VERTICES; i++) {
        if (g->ativo[i]) {
            fprintf(fp, "%d \"%s\"\n", i, g->rotulo[i]);
        }
    }

    fprintf(fp, "%d\n", g->num_arestas);
    for (i = 0; i < MAX_VERTICES; i++) {
        if (!g->ativo[i]) continue;
        for (j = i + 1; j < MAX_VERTICES; j++) {
            if (g->ativo[j] && g->adj[i][j] > 0) {
                fprintf(fp, "%d %d %d\n", i, j, g->adj[i][j]);
            }
        }
    }

    fclose(fp);
    printf("Dados gravados com sucesso em %s\n", nome_arquivo);
    return 1;
}

void mostrar_conteudo_arquivo(const char *nome_arquivo) {
    FILE *fp = fopen(nome_arquivo, "r");
    char linha[256];
    int count = 0;

    if (!fp) {
        printf("Erro: Nao foi possivel abrir %s.\n", nome_arquivo);
        return;
    }

    printf("\nConteudo de %s (primeiras 20 linhas):\n", nome_arquivo);
    while (fgets(linha, sizeof(linha), fp) && count < 20) {
        printf("%s", linha);
        count++;
    }
    if (!feof(fp)) {
        printf("... (restante omitido)\n");
    }
    fclose(fp);
}

void mostrar_grafo(const Grafo *g) {
    int i, j, exibidos = 0;
    int ids[10];

    printf("\nResumo do Grafo: %d vertices, %d arestas (Tipo %d)\n",
           g->num_vertices, g->num_arestas, g->tipo);

    for (i = 0; i < MAX_VERTICES && exibidos < 10; i++) {
        if (g->ativo[i]) {
            ids[exibidos++] = i;
        }
    }

    printf("Submatriz de Adjacencia (primeiros %d vertices ativos):\n     ", exibidos);
    for (i = 0; i < exibidos; i++) {
        printf("%4s ", g->rotulo[ids[i]]);
    }
    printf("\n");
    for (i = 0; i < exibidos; i++) {
        printf("%4s ", g->rotulo[ids[i]]);
        for (j = 0; j < exibidos; j++) {
            printf("%4d ", g->adj[ids[i]][ids[j]]);
        }
        printf("\n");
    }

    printf("\nLista de Adjacencia (amostra dos 5 primeiros voluntarios):\n");
    exibidos = 0;
    for (i = 0; i < MAX_VERTICES && exibidos < 5; i++) {
        if (eh_voluntario(g, i)) {
            int k = 0;
            printf("  %s (cap=%d): ", g->rotulo[i], g->capacidade[i]);
            for (j = 0; j < MAX_VERTICES; j++) {
                if (g->ativo[j] && g->adj[i][j] > 0) {
                    printf("%s(w=%d) ", g->rotulo[j], g->adj[i][j]);
                    k++;
                }
            }
            if (k == 0) printf("(isolado)");
            printf("\n");
            exibidos++;
        }
    }
}

int inserir_vertice(Grafo *g, int id, const char *rotulo) {
    if (id < 0 || id >= MAX_VERTICES) {
        printf("Erro: ID fora do intervalo valido (0 a %d).\n", MAX_VERTICES - 1);
        return 0;
    }
    if (g->ativo[id]) {
        printf("Erro: Vertice %d ja existe (%s).\n", id, g->rotulo[id]);
        return 0;
    }
    g->ativo[id] = 1;
    snprintf(g->rotulo[id], MAX_ROTULO, "%s", rotulo);
    g->capacidade[id] = (rotulo[0] == 'V' || rotulo[0] == 'v') ? 2 : 1;
    g->num_vertices++;
    printf("Vertice %d \"%s\" inserido.\n", id, g->rotulo[id]);
    return 1;
}

int inserir_aresta(Grafo *g, int u, int v, int peso) {
    if (u < 0 || u >= MAX_VERTICES || v < 0 || v >= MAX_VERTICES ||
        !g->ativo[u] || !g->ativo[v] || u == v || peso <= 0) {
        printf("Vertices ou peso invalidos!\n");
        return 0;
    }
    if (g->adj[u][v] == 0) {
        g->num_arestas++;
    }
    g->adj[u][v] = peso;
    g->adj[v][u] = peso;
    printf("Aresta entre %d (%s) e %d (%s) com peso %d inserida.\n",
           u, g->rotulo[u], v, g->rotulo[v], peso);
    return 1;
}

int remover_vertice(Grafo *g, int id) {
    int j;
    if (id < 0 || id >= MAX_VERTICES || !g->ativo[id]) {
        printf("Vertice inexistente.\n");
        return 0;
    }
    for (j = 0; j < MAX_VERTICES; j++) {
        if (g->adj[id][j] > 0) {
            g->adj[id][j] = 0;
            g->adj[j][id] = 0;
            g->num_arestas--;
        }
    }
    g->ativo[id] = 0;
    g->rotulo[id][0] = '\0';
    g->num_vertices--;
    printf("Vertice %d e suas arestas removidos.\n", id);
    return 1;
}

int remover_aresta(Grafo *g, int u, int v) {
    if (u < 0 || u >= MAX_VERTICES || v < 0 || v >= MAX_VERTICES ||
        !g->ativo[u] || !g->ativo[v] || g->adj[u][v] == 0) {
        printf("Aresta inexistente.\n");
        return 0;
    }
    g->adj[u][v] = 0;
    g->adj[v][u] = 0;
    g->num_arestas--;
    printf("Aresta entre %d e %d removida.\n", u, v);
    return 1;
}

void analisar_conexidade(const Grafo *g) {
    int visitado[MAX_VERTICES] = {0};
    int fila[MAX_VERTICES];
    int componentes = 0;
    int i, j;

    if (g->num_vertices == 0) {
        printf("Grafo vazio.\n");
        return;
    }

    printf("\n--- Analise de Conexidade (BFS) ---\n");
    for (i = 0; i < MAX_VERTICES; i++) {
        if (g->ativo[i] && !visitado[i]) {
            int ini = 0, fim = 0, tam = 0;
            componentes++;
            visitado[i] = 1;
            fila[fim++] = i;

            while (ini < fim) {
                int u = fila[ini++];
                tam++;
                for (j = 0; j < MAX_VERTICES; j++) {
                    if (g->ativo[j] && g->adj[u][j] > 0 && !visitado[j]) {
                        visitado[j] = 1;
                        fila[fim++] = j;
                    }
                }
            }
            if (tam == 1) {
                printf("  Componente %d: 1 vertice isolado (%s, ID %d)\n",
                       componentes, g->rotulo[i], i);
            } else {
                printf("  Componente %d: %d vertices\n", componentes, tam);
            }
        }
    }

    if (componentes == 1) {
        printf("Resultado: Grafo Conexo (1 componente conexa).\n");
    } else {
        printf("Resultado: Grafo Desconexo (%d componentes conexas).\n", componentes);
    }
}

static void executar_mcmf(const Grafo *g, int usar_capacidade_real, int exibir_pares) {
    int mapa[MAX_VERTICES];
    int inv_mapa[MAX_VERTICES];
    int total_nos = 0;
    int i, j, u, v;
    int total_vol = 0, total_alunos = 0, cap_total = 0;

    for (i = 0; i < MAX_VERTICES; i++) {
        mapa[i] = -1;
        if (g->ativo[i]) {
            mapa[i] = total_nos;
            inv_mapa[total_nos] = i;
            total_nos++;
            if (eh_voluntario(g, i)) {
                total_vol++;
                cap_total += usar_capacidade_real ? g->capacidade[i] : 1;
            } else if (eh_aluno(g, i)) {
                total_alunos++;
            }
        }
    }

    int S = total_nos;
    int T = total_nos + 1;
    int N = total_nos + 2;

    FluxoAresta *rede[MAX_VERTICES + 2];
    int deg[MAX_VERTICES + 2] = {0};
    int cap_alloc[MAX_VERTICES + 2] = {0};

    for (i = 0; i < N; i++) {
        cap_alloc[i] = 16;
        rede[i] = (FluxoAresta *)malloc(cap_alloc[i] * sizeof(FluxoAresta));
    }

    #define ADD_EDGE(from, to_node, capacity, edge_cost) do { \
        if (deg[from] >= cap_alloc[from]) { \
            cap_alloc[from] *= 2; \
            rede[from] = (FluxoAresta *)realloc(rede[from], cap_alloc[from] * sizeof(FluxoAresta)); \
        } \
        if (deg[to_node] >= cap_alloc[to_node]) { \
            cap_alloc[to_node] *= 2; \
            rede[to_node] = (FluxoAresta *)realloc(rede[to_node], cap_alloc[to_node] * sizeof(FluxoAresta)); \
        } \
        int idx_f = deg[from]; \
        int idx_r = deg[to_node]; \
        rede[from][idx_f].to = to_node; \
        rede[from][idx_f].cap = capacity; \
        rede[from][idx_f].cost = edge_cost; \
        rede[from][idx_f].rev = idx_r; \
        deg[from]++; \
        rede[to_node][idx_r].to = from; \
        rede[to_node][idx_r].cap = 0; \
        rede[to_node][idx_r].cost = -(edge_cost); \
        rede[to_node][idx_r].rev = idx_f; \
        deg[to_node]++; \
    } while (0)

    for (i = 0; i < MAX_VERTICES; i++) {
        if (!g->ativo[i]) continue;
        if (eh_voluntario(g, i)) {
            int cap_v = usar_capacidade_real ? g->capacidade[i] : 1;
            ADD_EDGE(S, mapa[i], cap_v, 0);
        } else if (eh_aluno(g, i)) {
            ADD_EDGE(mapa[i], T, 1, 0);
        }
    }

    for (i = 0; i < MAX_VERTICES; i++) {
        if (!eh_voluntario(g, i)) continue;
        for (j = 0; j < MAX_VERTICES; j++) {
            if (eh_aluno(g, j) && g->adj[i][j] > 0) {
                /* Maximizar peso = minimizar (-peso) */
                ADD_EDGE(mapa[i], mapa[j], 1, -g->adj[i][j]);
            }
        }
    }
    #undef ADD_EDGE

    int fluxo = 0;
    int custo_total = 0;
    int dist[MAX_VERTICES + 2];
    int pai_v[MAX_VERTICES + 2];
    int pai_e[MAX_VERTICES + 2];
    int na_fila[MAX_VERTICES + 2];
    int fila[MAX_VERTICES * 10];

    while (1) {
        int ini = 0, fim = 0;
        for (i = 0; i < N; i++) {
            dist[i] = INF;
            pai_v[i] = -1;
            pai_e[i] = -1;
            na_fila[i] = 0;
        }
        dist[S] = 0;
        fila[fim++] = S;
        na_fila[S] = 1;

        while (ini < fim) {
            u = fila[ini++];
            na_fila[u] = 0;
            for (i = 0; i < deg[u]; i++) {
                v = rede[u][i].to;
                if (rede[u][i].cap > 0 && dist[u] + rede[u][i].cost < dist[v]) {
                    dist[v] = dist[u] + rede[u][i].cost;
                    pai_v[v] = u;
                    pai_e[v] = i;
                    if (!na_fila[v]) {
                        fila[fim++] = v;
                        na_fila[v] = 1;
                    }
                }
            }
        }

        if (dist[T] == INF) break;

        int cur = T;
        while (cur != S) {
            int pv = pai_v[cur];
            int pe = pai_e[cur];
            int rev = rede[pv][pe].rev;
            rede[pv][pe].cap -= 1;
            rede[cur][rev].cap += 1;
            cur = pv;
        }
        fluxo++;
        custo_total += dist[T];
    }

    int score_total = -custo_total;
    double cobertura = (total_alunos > 0) ? (100.0 * fluxo / total_alunos) : 0.0;
    double score_medio = (fluxo > 0) ? ((double)score_total / fluxo) : 0.0;

    printf("\n[%s]\n", usar_capacidade_real
        ? "MODELO 2: EMPARELHAMENTO CAPACITADO (b-matching)"
        : "MODELO 1: EMPARELHAMENTO SIMPLES (1-para-1)");
    printf("  Voluntarios: %d | Vagas totais ofertadas: %d | Alunos: %d\n",
           total_vol, cap_total, total_alunos);
    printf("  Alunos atendidos: %d / %d (%.1f%%)\n", fluxo, total_alunos, cobertura);
    printf("  Score total de compatibilidade: %d (Media: %.2f)\n", score_total, score_medio);

    if (exibir_pares) {
        int atendido[MAX_VERTICES] = {0};
        printf("  Alocacoes encontradas:\n");
        for (i = 0; i < MAX_VERTICES; i++) {
            if (!eh_voluntario(g, i)) continue;
            int idx_u = mapa[i];
            int count_u = 0;
            for (j = 0; j < deg[idx_u]; j++) {
                int idx_v = rede[idx_u][j].to;
                if (idx_v < total_nos && rede[idx_u][j].cap == 0 && rede[idx_u][j].cost < 0) {
                    int orig_v = inv_mapa[idx_v];
                    if (count_u == 0) {
                        printf("    %s (cap=%d) -> ", g->rotulo[i], g->capacidade[i]);
                    } else {
                        printf(", ");
                    }
                    printf("%s (peso=%d)", g->rotulo[orig_v], -rede[idx_u][j].cost);
                    atendido[orig_v] = 1;
                    count_u++;
                }
            }
            if (count_u > 0) printf("\n");
        }

        printf("  Alunos nao atendidos: ");
        int nao_atendidos = 0;
        for (i = 0; i < MAX_VERTICES; i++) {
            if (eh_aluno(g, i) && !atendido[i]) {
                printf("%s ", g->rotulo[i]);
                nao_atendidos++;
            }
        }
        if (nao_atendidos == 0) printf("Nenhum!");
        printf(" (Total: %d)\n", nao_atendidos);
    }

    for (i = 0; i < N; i++) {
        free(rede[i]);
    }
}

void resolver_emparelhamento(const Grafo *g) {
    if (g->num_vertices == 0) {
        printf("Grafo vazio. Carregue o arquivo grafo.txt primeiro.\n");
        return;
    }
    printf("\n====================================================================\n");
    printf(" SOLUCAO DO PROBLEMA: EMPARELHAMENTO MAXIMO PONDERADO (MCMF / SPFA)\n");
    printf("====================================================================\n");
    executar_mcmf(g, 0, 0);
    executar_mcmf(g, 1, 1);
    printf("--------------------------------------------------------------------\n");
    printf("Interpretacao: O emparelhamento simples (1-para-1) limita cada\n");
    printf("voluntario a apenas 1 aluno, desperdicando horarios livres distintos.\n");
    printf("Ao modelar a capacidade semanal c(u) via b-matching, a cobertura\n");
    printf("sobe drasticamente, restando apenas alunos estruturalmente isolados.\n");
}

void analisar_graus(const Grafo *g) {
    int i, j;
    int min_v = INF, max_v = -1, soma_v = 0, qtd_v = 0, id_max_v = -1;
    int min_a = INF, max_a = -1, soma_a = 0, qtd_a = 0, id_max_a = -1;
    int isolados = 0, criticos = 0;

    if (g->num_vertices == 0) {
        printf("Grafo vazio.\n");
        return;
    }

    printf("\n--- Tecnica 1: Analise de Grau dos Vertices ---\n");
    for (i = 0; i < MAX_VERTICES; i++) {
        if (!g->ativo[i]) continue;
        int grau = 0;
        for (j = 0; j < MAX_VERTICES; j++) {
            if (g->ativo[j] && g->adj[i][j] > 0) grau++;
        }

        if (grau == 0) isolados++;
        if (eh_voluntario(g, i)) {
            qtd_v++;
            soma_v += grau;
            if (grau < min_v) min_v = grau;
            if (grau > max_v) { max_v = grau; id_max_v = i; }
        } else if (eh_aluno(g, i)) {
            qtd_a++;
            soma_a += grau;
            if (grau < min_a) min_a = grau;
            if (grau > max_a) { max_a = grau; id_max_a = i; }
            if (grau <= 1) criticos++;
        }
    }

    if (qtd_v > 0) {
        printf("Particao U (Voluntarios, n=%d): grau min=%d, max=%d (%s), media=%.2f\n",
               qtd_v, min_v, max_v, g->rotulo[id_max_v], (double)soma_v / qtd_v);
    }
    if (qtd_a > 0) {
        printf("Particao V (Alunos, n=%d):      grau min=%d, max=%d (%s), media=%.2f\n",
               qtd_a, min_a, max_a, g->rotulo[id_max_a], (double)soma_a / qtd_a);
    }
    printf("Grau medio global: %.2f | Vertices isolados (grau 0): %d | Alunos criticos (grau <= 1): %d\n",
           (double)(soma_v + soma_a) / g->num_vertices, isolados, criticos);

    printf("Alunos isolados (sem nenhum voluntario compativel): ");
    int encontrou = 0;
    for (i = 0; i < MAX_VERTICES; i++) {
        if (eh_aluno(g, i)) {
            int grau = 0;
            for (j = 0; j < MAX_VERTICES; j++) {
                if (g->ativo[j] && g->adj[i][j] > 0) grau++;
            }
            if (grau == 0) {
                printf("%s ", g->rotulo[i]);
                encontrou = 1;
            }
        }
    }
    if (!encontrou) printf("Nenhum");
    printf("\n");
}
void analisar_coloracao_biparticao(const Grafo *g) {
    int cor[MAX_VERTICES];
    int fila[MAX_VERTICES];
    int bipartido = 1;
    int i, j;

    if (g->num_vertices == 0) {
        printf("Grafo vazio.\n");
        return;
    }

    for (i = 0; i < MAX_VERTICES; i++) cor[i] = -1;

    for (i = 0; i < MAX_VERTICES; i++) {
        if (g->ativo[i] && cor[i] == -1) {
            int ini = 0, fim = 0;
            cor[i] = 0;
            fila[fim++] = i;
            while (ini < fim) {
                int u = fila[ini++];
                for (j = 0; j < MAX_VERTICES; j++) {
                    if (g->ativo[j] && g->adj[u][j] > 0) {
                        if (cor[j] == -1) {
                            cor[j] = 1 - cor[u];
                            fila[fim++] = j;
                        } else if (cor[j] == cor[u]) {
                            bipartido = 0;
                        }
                    }
                }
            }
        }
    }

    int c0 = 0, c1 = 0;
    for (i = 0; i < MAX_VERTICES; i++) {
        if (g->ativo[i]) {
            if (cor[i] == 0) c0++;
            else if (cor[i] == 1) c1++;
        }
    }

    printf("\n--- Tecnica 2: Coloracao de Vertices e Verificacao de Biparticao ---\n");
    if (bipartido) {
        printf("Resultado BFS 2-Coloracao: GRAFO BIPARTIDO (Numero cromatico chi(G) = 2).\n");
        printf("Particoes encontradas: Cor 0 = %d vertices | Cor 1 = %d vertices.\n", c0, c1);
        printf("Interpretacao: Nao ha ciclos impares nem conflitos dentro do mesmo grupo,\n");
        printf("validando matematicamente a separacao entre Voluntarios e Alunos.\n");
    } else {
        int ordem[MAX_VERTICES], graus[MAX_VERTICES], n = 0;
        for (i = 0; i < MAX_VERTICES; i++) {
            if (g->ativo[i]) {
                int d = 0;
                for (j = 0; j < MAX_VERTICES; j++) if (g->ativo[j] && g->adj[i][j] > 0) d++;
                ordem[n] = i;
                graus[i] = d;
                cor[i] = -1;
                n++;
            }
        }
        for (i = 0; i < n; i++) {
            for (j = i + 1; j < n; j++) {
                if (graus[ordem[j]] > graus[ordem[i]]) {
                    int tmp = ordem[i]; ordem[i] = ordem[j]; ordem[j] = tmp;
                }
            }
        }
        int num_cores = 0;
        for (i = 0; i < n; i++) {
            int u = ordem[i];
            int usada[MAX_VERTICES] = {0};
            for (j = 0; j < MAX_VERTICES; j++) {
                if (g->ativo[j] && g->adj[u][j] > 0 && cor[j] != -1) {
                    usada[cor[j]] = 1;
                }
            }
            int c = 0;
            while (usada[c]) c++;
            cor[u] = c;
            if (c + 1 > num_cores) num_cores = c + 1;
        }
        printf("Resultado: GRAFO NAO BIPARTIDO (possui ciclo impar).\n");
        printf("Coloracao Gulosa (Welsh-Powell) utilizou %d cores.\n", num_cores);
    }
}
void verificar_euleriano(const Grafo *g) {
    int i, j, impares = 0;

    if (g->num_vertices == 0) {
        printf("Grafo vazio.\n");
        return;
    }

    for (i = 0; i < MAX_VERTICES; i++) {
        if (!g->ativo[i]) continue;
        int grau = 0;
        for (j = 0; j < MAX_VERTICES; j++) {
            if (g->ativo[j] && g->adj[i][j] > 0) grau++;
        }
        if (grau % 2 != 0) impares++;
    }

    printf("\n--- Tecnica 3: Verificacao de Grafo Euleriano e Percurso Euleriano ---\n");
    printf("Quantidade de vertices com grau impar: %d\n", impares);
    if (impares == 0 && g->num_arestas > 0) {
        printf("Resultado: Todos os vertices possuem grau par (candidato a Grafo Euleriano,\n");
        printf("           desde que todas as arestas estejam na mesma componente conexa).\n");
    } else if (impares == 2) {
        printf("Resultado: NAO e Grafo Euleriano, mas admite Percurso Euleriano (exatamente 2 vertices de grau impar).\n");
    } else {
        printf("Resultado: NAO e Grafo Euleriano e NAO admite Percurso Euleriano\n");
        printf("           (Teorema de Euler exige 0 ou 2 vertices de grau impar).\n");
        printf("Interpretacao: A heterogeneidade natural das grades horarias e disciplinas\n");
        printf("gera graus impares em dezenas de voluntarios e alunos, inviabilizando um\n");
        printf("percurso que percorra cada compatibilidade exatamente uma vez.\n");
    }
}


void verificar_hamiltoniano(const Grafo *g) {
    int i, j;
    int qtd_u = 0, qtd_v = 0, min_grau = INF;
    int dirac_ok = 1, ore_ok = 1;

    if (g->num_vertices < 3) {
        printf("\n--- Tecnica 4: Verificacao de Ciclo Hamiltoniano ---\n");
        printf("Resultado: NAO admite ciclo hamiltoniano (menos de 3 vertices).\n");
        return;
    }

    int graus[MAX_VERTICES] = {0};
    for (i = 0; i < MAX_VERTICES; i++) {
        if (!g->ativo[i]) continue;
        for (j = 0; j < MAX_VERTICES; j++) {
            if (g->ativo[j] && g->adj[i][j] > 0) graus[i]++;
        }
        if (graus[i] < min_grau) min_grau = graus[i];
        if (eh_voluntario(g, i)) qtd_u++;
        else if (eh_aluno(g, i)) qtd_v++;
    }

    if (min_grau < (g->num_vertices + 1) / 2) dirac_ok = 0;

    for (i = 0; i < MAX_VERTICES && ore_ok; i++) {
        if (!g->ativo[i]) continue;
        for (j = i + 1; j < MAX_VERTICES; j++) {
            if (g->ativo[j] && g->adj[i][j] == 0) {
                if (graus[i] + graus[j] < g->num_vertices) {
                    ore_ok = 0;
                    break;
                }
            }
        }
    }

    printf("\n--- Tecnica 4: Verificacao de Condicoes para Ciclo Hamiltoniano ---\n");
    printf("Condicao necessaria para grafos bipartidos (|U| == |V|): |U|=%d, |V|=%d -> %s\n",
           qtd_u, qtd_v, (qtd_u == qtd_v) ? "SATISFEITA" : "VIOLADA");
    printf("Grau minimo delta(G) = %d (necessario >= 2 para ciclo hamiltoniano)\n", min_grau);
    printf("Teorema de Dirac (delta(G) >= n/2 = %.1f): %s\n",
           g->num_vertices / 2.0, dirac_ok ? "SATISFEITO" : "NAO SATISFEITO");
    printf("Teorema de Ore (d(u)+d(v) >= n = %d para nao adjacentes): %s\n",
           g->num_vertices, ore_ok ? "SATISFEITO" : "NAO SATISFEITO");

    if (qtd_u != qtd_v || min_grau < 2) {
        printf("Conclusao: O grafo COMPROVADAMENTE NAO admite ciclo hamiltoniano.\n");
        printf("Interpretacao: Como ha muito mais alunos (%d) do que voluntarios (%d)\n", qtd_v, qtd_u);
        printf("e existem alunos isolados/pouco conectados, e impossivel alternar 1-a-1\n");
        printf("passando por todos os participantes exatamente uma vez.\n");
    } else if (dirac_ok || ore_ok) {
        printf("Conclusao: O grafo ADMITE ciclo hamiltoniano (por Dirac/Ore).\n");
    } else {
        printf("Conclusao: Nao atende as condicoes suficientes de Dirac/Ore.\n");
    }
}
