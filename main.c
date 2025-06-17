// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "expressao.h"

int main(void) {
    char tipo[8];
    char linha[512];
    while (1) {
        printf("Digite o tipo de expressão ([i]nfixa, [p]osfixa ou vazio para sair): ");
        if (!fgets(tipo, sizeof tipo, stdin) || tipo[0]=='\n') break;
        // remove \n e lower
        tipo[strcspn(tipo, "\n")] = '\0';
        for (char *p = tipo; *p; ++p) *p = tolower((unsigned char)*p);

        if (tipo[0] == 'i') {
            printf("Digite expressão infixa: ");
            if (!fgets(linha, sizeof linha, stdin) || linha[0]=='\n') break;
            linha[strcspn(linha, "\n")] = '\0';
            char *pos = getFormaPosFixa(linha);
            if (pos == NULL) { // Adicionado tratamento de erro
                printf("Erro na conversão para posfixa.\n");
                continue;
            }
            char *inf = getFormaInFixa(pos); // Reconvertendo para infixa para teste de idempotência
            float vPos = getValorPosFixa(pos);
            float vInf = getValorInFixa(linha); // Avalia a infixa original
            printf("Posfixa convertida: %s\n", pos);
            //printf("Infixa reconstruída (para validação): %s\n", inf); // Opcional para depuração
            printf("Valor (infixa original): %.6f\n", vInf);
            printf("Valor (posfixa convertida): %.6f\n", vPos);
            free(pos); 
            if(inf) free(inf); // Libera inf se não for NULL
        } else if (tipo[0] == 'p') {
            printf("Digite expressão posfixa: ");
            if (!fgets(linha, sizeof linha, stdin) || linha[0]=='\n') break;
            linha[strcspn(linha, "\n")] = '\0';
            char *inf = getFormaInFixa(linha);
            if (inf == NULL) { // Adicionado tratamento de erro
                printf("Erro na conversão para infixa.\n");
                continue;
            }
            float vPos = getValorPosFixa(linha);
            float vInf = getValorInFixa(inf); // Avalia a infixa convertida
            char *posRecon = getFormaPosFixa(inf); // Reconstroi posfixa da infixa convertida
            printf("Infixa convertida: %s\n", inf);
            //printf("Posfixa (reconstruída): %s\n", posRecon); // Opcional para depuração
            printf("Valor (posfixa original): %.6f\n", vPos);
            printf("Valor (infixa convertida): %.6f\n", vInf);
            free(inf); 
            if(posRecon) free(posRecon); // Libera posRecon se não for NULL
        } else {
            printf("Tipo inválido: use 'i' ou 'p'.\n");
        }
        printf("\n");
    }
    return 0;
}