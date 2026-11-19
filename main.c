

#include <stdio.h>
#include <stdlib.h>
#include "grafo.h"

static void exibir_cabecalho_opcoes(void) {
    printf("====================================================================\n");
    printf("  Sistema de Emparelhamento de Voluntarios e Turmas em Ligas Academicas\n");
    printf("====================================================================\n");
    printf("  Opcoes executadas automaticamente:\n");
    printf("  (a) Ler dados do arquivo grafo.txt\n");
    printf("  (i) Apresentar a conexidade do grafo\n");
    printf("  (j) Resolver Emparelhamento Ponderado (1-para-1 vs. Capacitado)\n");
    printf("  (k) Analise de Grau dos Vertices (Tecnica 1)\n");
    printf("  (l) Coloracao de Vertices e Biparticao (Tecnica 2)\n");
    printf("  (m) Verificacao de Grafo e Percurso Euleriano (Tecnica 3)\n");
    printf("  (n) Verificacao de Ciclo Hamiltoniano (Tecnica 4)\n");
    printf("====================================================================\n");
}

int main(void) {
    Grafo g;

    inicializar_grafo(&g);
    exibir_cabecalho_opcoes();

    printf("\n>>> Executando opcao (a): Ler dados do arquivo grafo.txt\n");
    if (!ler_arquivo_grafo(&g, "grafo.txt")) {
        return 1;
    }


    printf("\n>>> Executando opcao (i): Apresentar a conexidade do grafo\n");
    analisar_conexidade(&g);

    printf("\n>>> Executando opcao (j): Resolver Emparelhamento Ponderado\n");
    resolver_emparelhamento(&g);

    printf("\n>>> Executando opcao (k): Analise de Grau dos Vertices\n");
    analisar_graus(&g);

    printf("\n>>> Executando opcao (l): Coloracao de Vertices e Biparticao\n");
    analisar_coloracao_biparticao(&g);

    printf("\n>>> Executando opcao (m): Verificacao de Grafo e Percurso Euleriano\n");
    verificar_euleriano(&g);

    printf("\n>>> Executando opcao (n): Verificacao de Ciclo Hamiltoniano\n");
    verificar_hamiltoniano(&g);

    printf("\n====================================================================\n");
    printf("  Execucao concluida com sucesso.\n");
    printf("====================================================================\n");

    return 0;
}
