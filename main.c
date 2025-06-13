#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "expressao.h"

void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    char escolha;
    char expressao_usuario[512];

    do {
        printf("\n--- Avaliador de Expressoes Matematicas ---\n");
        printf("Escolha o tipo de expressao que deseja inserir:\n");
        printf("  'I' para Inserir uma expressao Infixa manualmente (ex: 5 * (2+3))\n");
        printf("  'P' para Inserir uma expressao Posfixa manualmente (ex: 3 4 + 5 *)\n");
        printf("  'S' para Sair\n");
        printf("Opcao: ");

        scanf(" %c", &escolha);
        limpar_buffer();

        escolha = tolower(escolha);

        if (escolha == 'i') {
            printf("\nDigite a expressao infixa: ");
            fgets(expressao_usuario, 512, stdin);
            expressao_usuario[strcspn(expressao_usuario, "\n")] = 0;

            char expr_original_usuario[512];
            strcpy(expr_original_usuario, expressao_usuario);

            printf("\n--- Resultados ---\n");
            printf("Expressao Infixa Original: %s\n", expr_original_usuario);

            char* posfixa_usuario = getFormaPosFixa(expressao_usuario);
            printf("Forma Pos-fixa Convertida: %s\n", posfixa_usuario);

            float valor_usuario = getValorInFixa(expr_original_usuario);
            printf("Valor Calculado: %.2f\n", valor_usuario);

        } else if (escolha == 'p') {
            // --- BLOCO MODIFICADO ---
            printf("\nDigite a expressao posfixa (operandos e operadores separados por espaco): ");
            fgets(expressao_usuario, 512, stdin);
            expressao_usuario[strcspn(expressao_usuario, "\n")] = 0;

            // Criamos cópias para as diferentes funções
            char posfixa_para_calculo[512];
            strcpy(posfixa_para_calculo, expressao_usuario);

            char posfixa_para_conversao[512];
            strcpy(posfixa_para_conversao, expressao_usuario);

            printf("\n--- Resultados ---\n");
            printf("Expressao Posfixa Original: %s\n", expressao_usuario);

            // 1. Converte de volta para Infixa para exibição
            char* infixa_convertida = getFormaInFixa(posfixa_para_conversao);
            printf("Forma In-fixa Convertida: %s\n", infixa_convertida);

            // 2. Calcula o valor
            float valor_posfixa = getValorPosFixa(posfixa_para_calculo);
            printf("Valor Calculado: %.2f\n", valor_posfixa);

        } else if (escolha == 's') {
            continue;
        } else {
            printf("\n!!! Alerta: Identificacao de inconsistencias nos dados de entradas.\n");
            printf("!!! Por favor, escolha 'I', 'P' ou 'S'.\n");
        }

    } while (escolha != 's');

    printf("\nPrograma encerrado.\n");
    return 0;
}