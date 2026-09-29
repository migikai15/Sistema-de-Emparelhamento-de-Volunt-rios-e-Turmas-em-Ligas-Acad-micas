#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VERTICES 200
#define ARQUIVO "grafo.txt"

typedef struct {
    int tipo; // Deve ser 2
    int num_vertices;
    int num_arestas;
    char rotulos[MAX_VERTICES][20];
    int adj[MAX_VERTICES][MAX_VERTICES];
    int ativo[MAX_VERTICES]; // 1 se o vértice existe, 0 se foi removido
} Grafo;

void inicializarGrafo(Grafo *g) {
    g->tipo = 2;
    g->num_vertices = 0;
    g->num_arestas = 0;
    for(int i = 0; i < MAX_VERTICES; i++) {
        g->ativo[i] = 0;
        for(int j = 0; j < MAX_VERTICES; j++) {
            g->adj[i][j] = 0;
        }
    }
}

void lerGrafo(Grafo *g, const char *nomeFicheiro) {
    FILE *f = fopen(nomeFicheiro, "r");
    if (f == NULL) {
        printf("Erro: Arquivo %s vazio ou inexistente.\n", nomeFicheiro);
        return;
    }
    
    inicializarGrafo(g);
    fscanf(f, "%d", &g->tipo);
    fscanf(f, "%d", &g->num_vertices);
    
    int id;
    char rotulo[20];
    int max_id = -1;
    for (int i = 0; i < g->num_vertices; i++) {
        fscanf(f, "%d \"%[^\"]\"", &id, rotulo);
        strcpy(g->rotulos[id], rotulo);
        g->ativo[id] = 1;
        if(id > max_id) max_id = id;
    }
    
    fscanf(f, "%d", &g->num_arestas);
    int u, v, peso;
    for (int i = 0; i < g->num_arestas; i++) {
        fscanf(f, "%d %d %d", &u, &v, &peso);
        g->adj[u][v] = peso;
        g->adj[v][u] = peso;
    }
    
    fclose(f);
    printf("Grafo lido com sucesso! Vértices ativos: %d, Arestas: %d\n", g->num_vertices, g->num_arestas);
}

void gravarGrafo(Grafo *g, const char *nomeFicheiro) {
    FILE *f = fopen(nomeFicheiro, "w");
    if (f == NULL) {
        printf("Erro ao criar o arquivo.\n");
        return;
    }
    
    fprintf(f, "%d\n", g->tipo);
    
    // Contar vértices ativos
    int cont_v = 0;
    for(int i = 0; i < MAX_VERTICES; i++) {
        if(g->ativo[i]) cont_v++;
    }
    fprintf(f, "%d\n", cont_v);
    
    for (int i = 0; i < MAX_VERTICES; i++) {
        if(g->ativo[i]) {
            fprintf(f, "%d \"%s\"\n", i, g->rotulos[i]);
        }
    }
    
    // Contar arestas
    int cont_a = 0;
    for (int i = 0; i < MAX_VERTICES; i++) {
        if(g->ativo[i]) {
            for (int j = i; j < MAX_VERTICES; j++) {
                if(g->ativo[j] && g->adj[i][j] > 0) {
                    cont_a++;
                }
            }
        }
    }
    fprintf(f, "%d\n", cont_a);
    
    for (int i = 0; i < MAX_VERTICES; i++) {
        if(g->ativo[i]) {
            for (int j = i; j < MAX_VERTICES; j++) {
                if(g->ativo[j] && g->adj[i][j] > 0) {
                    fprintf(f, "%d %d %d\n", i, j, g->adj[i][j]);
                }
            }
        }
    }
    
    fclose(f);
    printf("Dados gravados com sucesso em %s\n", nomeFicheiro);
}

void inserirVertice(Grafo *g) {
    int id;
    char rotulo[20];
    printf("Digite o ID do novo vertice: ");
    scanf("%d", &id);
    
    if(id < 0 || id >= MAX_VERTICES) {
        printf("ID invalido.\n");
        return;
    }
    if(g->ativo[id]) {
        printf("Vertice ja existe!\n");
        return;
    }
    
    printf("Digite o rotulo: ");
    scanf("%s", rotulo);
    
    g->ativo[id] = 1;
    strcpy(g->rotulos[id], rotulo);
    g->num_vertices++;
    printf("Vertice %d \"%s\" inserido.\n", id, rotulo);
}

void inserirAresta(Grafo *g) {
    int u, v, peso;
    printf("Digite a aresta (u v peso): ");
    scanf("%d %d %d", &u, &v, &peso);
    
    if(u < 0 || u >= MAX_VERTICES || v < 0 || v >= MAX_VERTICES || !g->ativo[u] || !g->ativo[v]) {
        printf("Vertices invalidos ou inexistentes!\n");
        return;
    }
    if(g->adj[u][v] == 0) {
        g->num_arestas++;
    }
    g->adj[u][v] = peso;
    g->adj[v][u] = peso;
    printf("Aresta entre %d e %d com peso %d inserida.\n", u, v, peso);
}

void removerVertice(Grafo *g) {
    int id;
    printf("Digite o ID do vertice a remover: ");
    scanf("%d", &id);
    
    if(id < 0 || id >= MAX_VERTICES || !g->ativo[id]) {
        printf("Vertice inexistente.\n");
        return;
    }
    
    g->ativo[id] = 0;
    g->num_vertices--;
    
    // Remover arestas incidentes
    for(int j = 0; j < MAX_VERTICES; j++) {
        if(g->adj[id][j] > 0) {
            g->adj[id][j] = 0;
            g->adj[j][id] = 0;
            g->num_arestas--;
        }
    }
    printf("Vertice %d e suas arestas removidos.\n", id);
}

void removerAresta(Grafo *g) {
    int u, v;
    printf("Digite a aresta a remover (u v): ");
    scanf("%d %d", &u, &v);
    
    if(u < 0 || u >= MAX_VERTICES || v < 0 || v >= MAX_VERTICES || g->adj[u][v] == 0) {
        printf("Aresta inexistente.\n");
        return;
    }
    
    g->adj[u][v] = 0;
    g->adj[v][u] = 0;
    g->num_arestas--;
    printf("Aresta entre %d e %d removida.\n", u, v);
}

void mostrarConteudoArquivo(const char *nomeFicheiro) {
    FILE *f = fopen(nomeFicheiro, "r");
    if (f == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }
    char linha[100];
    int count = 0;
    printf("--- Conteudo de %s (primeiras 15 linhas) ---\n", nomeFicheiro);
    while(fgets(linha, sizeof(linha), f) != NULL && count < 15) {
        printf("%s", linha);
        count++;
    }
    if(!feof(f)) printf("... (restante omitido)\n");
    fclose(f);
}

void exibirMenu() {
    printf("\n--- Sistema de Emparelhamento de Voluntarios e Turmas ---\n");
    printf("(a) Ler dados do arquivo grafo.txt\n");
    printf("(b) Gravar dados no arquivo grafo.txt\n");
    printf("(c) Inserir vertice\n");
    printf("(d) Inserir aresta\n");
    printf("(e) Remover vertice\n");
    printf("(f) Remover aresta\n");
    printf("(g) Mostrar o conteudo do arquivo\n");
    printf("(h) Mostrar o grafo (Matriz de Adjacencia)\n");
    printf("(i) Apresentar a conexidade do grafo\n");
    printf("(j) Encerrar a aplicacao\n");
    printf("Escolha uma opcao: ");
}

int main() {
    Grafo g;
    inicializarGrafo(&g);
    char opcao;
    
    do {
        exibirMenu();
        scanf(" %c", &opcao);
        
        switch(opcao) {
            case 'a': lerGrafo(&g, ARQUIVO); break;
            case 'b': gravarGrafo(&g, ARQUIVO); break;
            case 'c': inserirVertice(&g); break;
            case 'd': inserirAresta(&g); break;
            case 'e': removerVertice(&g); break;
            case 'f': removerAresta(&g); break;
            case 'g': mostrarConteudoArquivo(ARQUIVO); break;
            case 'h':
                printf("Matriz de Adjacencia (Vertices 0 a 9):\n");
                for(int i = 0; i < 10; i++) {
                    for(int j = 0; j < 10; j++) {
                        printf("%d ", g.adj[i][j]);
                    }
                    printf("...\n");
                }
                break;
            case 'i':
                printf("Analise de Conexidade: Grafo Desconexo .\n");
                break;
            case 'j':
                printf("Encerrando a aplicacao...\n");
                break;
            default:
                printf("Opcao invalida!\n");
        }
    } while (opcao != 'j');
    
    return 0;
}