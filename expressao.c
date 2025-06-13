#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "expressao.h"

// --- Definição da Estrutura de Pilha e Funções ---

#define MAX_PILHA 100

// Pilha para operadores (chars) na conversão Infixa -> Posfixa
char pilhaOperadores[MAX_PILHA];
int topoOperadores = -1;

void empilharOperador(char op) {
    if (topoOperadores < MAX_PILHA - 1) pilhaOperadores[++topoOperadores] = op;
}
char desempilharOperador() {
    return (topoOperadores != -1) ? pilhaOperadores[topoOperadores--] : '\0';
}
char topoDaPilha() {
    return (topoOperadores != -1) ? pilhaOperadores[topoOperadores] : '\0';
}
int pilhaVazia() {
    return topoOperadores == -1;
}

// Pilha para valores (floats) na avaliação da expressão
float pilhaValores[MAX_PILHA];
int topoValores = -1;

void empilharValor(float val) {
    if (topoValores < MAX_PILHA - 1) pilhaValores[++topoValores] = val;
}
float desempilharValor() {
    return (topoValores != -1) ? pilhaValores[topoValores--] : 0.0f;
}

// Pilha para strings na conversão Posfixa -> Infixa
char* pilhaStrings[MAX_PILHA];
int topoStrings = -1;

void empilharString(char* str) {
    if (topoStrings < MAX_PILHA - 1) pilhaStrings[++topoStrings] = str;
}
char* desempilharString() {
    return (topoStrings != -1) ? pilhaStrings[topoStrings--] : NULL;
}


// --- Funções Auxiliares ---

// Define a precedência dos operadores para o algoritmo de conversão
int precedencia(char op) {
    switch (op) {
        case '+': case '-': return 1;
        case '*': case '/': case '%': return 2;
        case '^': return 3;
        case 'r': case 's': case 'c': case 't': case 'l': return 4; // Funções
    }
    return 0;
}

// Converte graus para radianos para as funções de math.h
double paraRadianos(double graus) {
    return graus * M_PI / 180.0;
}


// --- Implementação das Funções Principais ---

/**
 * Converte uma expressão Infixa para Pós-fixada.
 * Utiliza o algoritmo Shunting-yard.
 */
char *getFormaPosFixa(char *strInfixa) {
    static char posFixa[512];
    int i = 0, j = 0;
    char token;

    memset(posFixa, 0, sizeof(posFixa));
    topoOperadores = -1;

    while ((token = strInfixa[i++]) != '\0') {
        if (isspace(token)) continue;

        if (isdigit(token) || token == '.') {
            posFixa[j++] = token;
            if (!isdigit(strInfixa[i]) && strInfixa[i] != '.') {
                posFixa[j++] = ' ';
            }
        } else if (isalpha(token)) {
            char func[4] = {0};
            func[0] = tolower(token); func[1] = tolower(strInfixa[i]); func[2] = tolower(strInfixa[i+1]);
            
            if (strncmp(func, "rai", 3) == 0) { empilharOperador('r'); i += 3; }
            else if (strncmp(func, "sen", 3) == 0) { empilharOperador('s'); i += 2; }
            else if (strncmp(func, "cos", 3) == 0) { empilharOperador('c'); i += 2; }
            else if (strncmp(func, "tg", 2) == 0) { empilharOperador('t'); i += 1; }
            else if (strncmp(func, "log", 3) == 0) { empilharOperador('l'); i += 2; }

        } else if (token == '(') {
            empilharOperador(token);
        } else if (token == ')') {
            while (!pilhaVazia() && topoDaPilha() != '(') {
                posFixa[j++] = desempilharOperador();
                posFixa[j++] = ' ';
            }
            desempilharOperador(); // Descarta o '('
        } else { // É um operador
            while (!pilhaVazia() && precedencia(topoDaPilha()) >= precedencia(token)) {
                posFixa[j++] = desempilharOperador();
                posFixa[j++] = ' ';
            }
            empilharOperador(token);
        }
    }

    while (!pilhaVazia()) {
        posFixa[j++] = desempilharOperador();
        posFixa[j++] = ' ';
    }
    posFixa[j] = '\0';
    return posFixa;
}


/**
 * Calcula o valor de uma expressão na forma Pós-fixada.
 */
float getValorPosFixa(char *strPosFixa) {
    char *token;
    topoValores = -1;
    char posFixaCopia[512];
    strcpy(posFixaCopia, strPosFixa);

    token = strtok(posFixaCopia, " ");

    while (token != NULL) {
        if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1])) || strchr(token, '.')) {
            empilharValor(atof(token));
        } else {
            float op2, op1, resultado;
            if (strlen(token) == 1 && strchr("+-*/%^", token[0])) {
                op2 = desempilharValor(); op1 = desempilharValor();
                switch (token[0]) {
                    case '+': resultado = op1 + op2; break;
                    case '-': resultado = op1 - op2; break;
                    case '*': resultado = op1 * op2; break;
                    case '/': resultado = op1 / op2; break;
                    case '%': resultado = fmod(op1, op2); break;
                    case '^': resultado = pow(op1, op2); break;
                }
                empilharValor(resultado);
            } else { // Funções
                op1 = desempilharValor();
                switch (token[0]) {
                    case 'r': resultado = sqrt(op1); break;
                    case 's': resultado = sin(paraRadianos(op1)); break;
                    case 'c': resultado = cos(paraRadianos(op1)); break;
                    case 't': resultado = tan(paraRadianos(op1)); break;
                    case 'l': resultado = log10(op1); break;
                }
                empilharValor(resultado);
            }
        }
        token = strtok(NULL, " ");
    }
    return desempilharValor();
}


/**
 * Calcula o valor de uma expressão na forma Infixa.
 */
float getValorInFixa(char *strInfixa) {
    char *posFixa = getFormaPosFixa(strInfixa);
    return getValorPosFixa(posFixa);
}


/**
 * Converte uma expressão Pós-fixada para Infixa.
 */
char* getFormaInFixa(char *strPosFixa) {
    static char bufferPool[MAX_PILHA][256];
    int pool_idx = 0;
    static char infixResult[512];
    char posFixaCopia[512];
    strcpy(posFixaCopia, strPosFixa);

    topoStrings = -1;
    char *token = strtok(posFixaCopia, " ");

    while (token != NULL) {
        if (isdigit(token[0]) || strchr(token, '.')) {
            empilharString(token);
        } else {
            char* op2, *op1;
            char* novaExpr = bufferPool[pool_idx++];
            if (pool_idx >= MAX_PILHA) return "Erro: Expressao muito complexa.";

            if (strlen(token) == 1 && strchr("+-*/%^", token[0])) {
                op2 = desempilharString(); op1 = desempilharString();
                if (!op1 || !op2) return "Erro: Expressao Posfixa mal formada.";
                sprintf(novaExpr, "(%s %c %s)", op1, token[0], op2);
                empilharString(novaExpr);
            } else {
                op1 = desempilharString();
                if (!op1) return "Erro: Expressao Posfixa mal formada.";
                char funcName[5];
                switch(token[0]) {
                    case 'r': strcpy(funcName, "raiz"); break;
                    case 's': strcpy(funcName, "sen"); break;
                    case 'c': strcpy(funcName, "cos"); break;
                    case 't': strcpy(funcName, "tg"); break;
                    case 'l': strcpy(funcName, "log"); break;
                    default:  strcpy(funcName, "func"); break;
                }
                sprintf(novaExpr, "%s(%s)", funcName, op1);
                empilharString(novaExpr);
            }
        }
        token = strtok(NULL, " ");
    }

    char* finalExpr = desempilharString();
    if (finalExpr) strcpy(infixResult, finalExpr);
    else strcpy(infixResult, "Erro na conversao");
    
    return infixResult;
}