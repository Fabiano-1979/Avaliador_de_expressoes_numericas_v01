#include <stdio.h>
#include <string.h>
#include "expressao.h"

// Função para limpar o buffer de entrada (stdin)
void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    char expressoes_teste[9][512] = {
        "(3+4)*5",
        "7*2+4",
        "8+(5*(2+4))",
        "(6/2+3)*4",
        "9+(5*(2+8*4))",
        "log(2+3)/5",
        "log(10)^3+2",
        "(45+60)*cos(30)",
        "sen(45)^2+0.5"
    };

    int escolha = 0;
    char expressao_usuario[512];

    do {
        printf("\n--- Avaliador de Expressoes Matematicas ---\n");
        printf("1. Testar expressoes do PDF\n");
        printf("2. Inserir uma expressao infixa manualmente\n");
        printf("3. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &escolha);
        limpar_buffer(); // Limpa o buffer após ler o número

        if (escolha == 1) {
            for (int i = 0; i < 9; i++) {
                char expr_original[512];
                strcpy(expr_original, expressoes_teste[i]); // Mantém a original

                printf("\n----------------------------------------\n");
                printf("Teste %d:\n", i + 1);
                printf("Expressao Infixa: %s\n", expr_original);

                // Converte para Pós-fixa
                char* posfixa = getFormaPosFixa(expressoes_teste[i]);
                printf("Forma Pos-fixa: %s\n", posfixa);

                // Calcula o valor
                float valor = getValorInFixa(expr_original);
                printf("Valor Calculado: ~%.2f\n", valor);
            }
        } else if (escolha == 2) {
            printf("\nDigite a expressao infixa (ex: 3*(4+5)): ");
            fgets(expressao_usuario, 512, stdin);
            // Remove a nova linha que o fgets adiciona
            expressao_usuario[strcspn(expressao_usuario, "\n")] = 0;

            char expr_original_usuario[512];
            strcpy(expr_original_usuario, expressao_usuario);

            printf("\n--- Resultados ---\n");
            printf("Expressao Infixa: %s\n", expr_original_usuario);

            char* posfixa_usuario = getFormaPosFixa(expressao_usuario);
            printf("Forma Pos-fixa: %s\n", posfixa_usuario);

            float valor_usuario = getValorInFixa(expr_original_usuario);
            printf("Valor Calculado: %.2f\n", valor_usuario);
        }

    } while (escolha != 3);

    printf("Programa encerrado.\n");
    return 0;
}