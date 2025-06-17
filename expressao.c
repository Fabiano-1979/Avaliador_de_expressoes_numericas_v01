#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_PI_2
#define M_PI_2 1.57079632679
#endif

#include "expressao.h"

#define MAX_TOKENS 512
#define MAX_TOKEN_LEN 64

static char *stackS[MAX_TOKENS];
static int topS;

static void pushS(char *s) {
    if (topS < MAX_TOKENS - 1) {
        stackS[++topS] = s;
    } else {
        fprintf(stderr, "Erro: Pilha de strings cheia (pushS).\n");
    }
}

static char *popS(void) {
    if (topS >= 0) {
        return stackS[topS--];
    }
    return NULL;
}

static char *peekS(void) {
    if (topS >= 0) {
        return stackS[topS];
    }
    return NULL;
}

static float stackF[MAX_TOKENS];
static int topF;

static void pushF(float v) {
    if (topF < MAX_TOKENS - 1) {
        stackF[++topF] = v;
    } else {
        fprintf(stderr, "Erro: Pilha de floats cheia (pushF).\n");
    }
}

static float popF(void) {
    if (topF >= 0) {
        return stackF[topF--];
    }
    return NAN;
}

static int prec(const char *op) {
    if (!strcmp(op, "raiz") || !strcmp(op, "sen") || !strcmp(op, "cos") || !strcmp(op, "tg") || !strcmp(op, "log")) return 5;
    if (strcmp(op, "^") == 0) return 4;
    if (!strcmp(op, "*") || !strcmp(op, "/") || !strcmp(op, "%")) return 3;
    if (!strcmp(op, "+") || !strcmp(op, "-")) return 2;
    return 0;
}

static int isOperator(const char *s) {
    return (!strcmp(s, "+") || !strcmp(s, "-") || !strcmp(s, "*") || !strcmp(s, "/") || !strcmp(s, "%") || !strcmp(s, "^"));
}

static int isFunction(const char *s) {
    return (!strcmp(s, "raiz") || !strcmp(s, "sen") || !strcmp(s, "cos") || !strcmp(s, "tg") || !strcmp(s, "log"));
}

static int tokenize(const char *expr, char tokens[][MAX_TOKEN_LEN]) {
    int i = 0, k = 0;
    while (expr[i] && k < MAX_TOKENS) {
        if (isspace((unsigned char)expr[i])) {
            i++;
            continue;
        }

        if (expr[i] == '(' || expr[i] == ')') {
            tokens[k][0] = expr[i];
            tokens[k][1] = '\0';
            k++;
            i++;
            continue;
        }

        int j = 0;
        // Lida com números negativos e decimais no início do token
        if ((expr[i] == '-' && (isdigit((unsigned char)expr[i+1]) || expr[i+1] == '.')) ||
            (expr[i] == '.' && isdigit((unsigned char)expr[i+1]))) {
            tokens[k][j++] = expr[i++];
        }

        // Lida com o restante do número ou palavra (função, operador multi-char)
        while (expr[i] && j < MAX_TOKEN_LEN - 1 &&
               (isalnum((unsigned char)expr[i]) || expr[i] == '.') &&
               !isspace((unsigned char)expr[i]) && expr[i] != '(' && expr[i] != ')') {
            tokens[k][j++] = expr[i++];
        }
        tokens[k][j] = '\0';
        k++;
    }
    return k;
}

char *getFormaPosFixa(char *Str) {
    char tokens[MAX_TOKENS][MAX_TOKEN_LEN];
    static char output[512]; 
    int n = tokenize(Str, tokens);
    output[0] = '\0';
    topS = -1;

    for (int i = 0; i < n; i++) {
        char *tok = tokens[i];

        if (isdigit((unsigned char)tok[0]) ||
            (tok[0] == '-' && strlen(tok) > 1 && isdigit((unsigned char)tok[1])) ||
            (tok[0] == '.' && strlen(tok) > 1 && isdigit((unsigned char)tok[1])) ) {
            strcat(output, tok);
            strcat(output, " ");
        }
        else if (isFunction(tok)) {
            pushS(strdup(tok));
        }
        else if (isOperator(tok)) {
            while (topS >= 0 && (isOperator(peekS()) || isFunction(peekS())) &&
                   ((prec(peekS()) > prec(tok)) || (prec(peekS()) == prec(tok) && strcmp(tok, "^") != 0))) {
                char *op = popS();
                if (op) {
                    strcat(output, op);
                    strcat(output, " ");
                    free(op);
                } else {
                    fprintf(stderr, "Erro: Pilha de operadores vazia inesperadamente em getFormaPosFixa.\n");
                    while (topS >= 0) { char *s = popS(); if (s) free(s); }
                    return NULL;
                }
            }
            pushS(strdup(tok));
        }
        else if (!strcmp(tok, "(")) {
            pushS(strdup(tok));
        }
        else if (!strcmp(tok, ")")) {
            while (topS >= 0 && strcmp(peekS(), "(") != 0) {
                char *op = popS();
                if (op) {
                    strcat(output, op);
                    strcat(output, " ");
                    free(op);
                } else {
                    fprintf(stderr, "Erro: Parêntese desbalanceado ou pilha vazia em getFormaPosFixa.\n");
                    return NULL;
                }
            }
            if (topS >= 0) {
                free(popS());
            } else {
                 fprintf(stderr, "Erro: Parêntese desbalanceado (faltando '(') em getFormaPosFixa.\n");
                 return NULL;
            }
            if (topS >= 0 && isFunction(peekS())) {
                char *fn = popS();
                if (fn) {
                    strcat(output, fn);
                    strcat(output, " ");
                    free(fn);
                } else {
                     fprintf(stderr, "Erro: Pilha vazia inesperadamente ao desempilhar função em getFormaPosFixa.\n");
                     return NULL;
                }
            }
        } else {
            fprintf(stderr, "Erro: Token inesperado '%s' na expressao infixa.\n", tok);
            while (topS >= 0) { char *s = popS(); if (s) free(s); }
            return NULL;
        }
    }
    while (topS >= 0) {
        char *op = popS();
        if (op && strcmp(op, "(") != 0) {
            strcat(output, op);
            strcat(output, " ");
            free(op);
        } else {
            fprintf(stderr, "Erro: Parêntese desbalanceado (sobra '(') ou operador inválido na pilha final de getFormaPosFixa.\n");
            if(op) free(op);
            return NULL;
        }
    }
    if (strlen(output) > 0 && output[strlen(output) - 1] == ' ') {
        output[strlen(output) - 1] = '\0';
    }
    return strdup(output);
}

typedef struct Node {
    char *expr;
    int prec;
} Node;

char *getFormaInFixa(char *Str) {
    char tokens[MAX_TOKENS][MAX_TOKEN_LEN];
    int n = tokenize(Str, tokens);
    int topN = -1;
    Node *stackN[MAX_TOKENS];

    for (int i = 0; i < n; i++) {
        char *tok = tokens[i];
        
        if (isdigit((unsigned char)tok[0]) ||
            (tok[0] == '-' && strlen(tok) > 1 && isdigit((unsigned char)tok[1])) ||
            (tok[0] == '.' && strlen(tok) > 1 && isdigit((unsigned char)tok[1])) ) {
            Node *node = malloc(sizeof(Node));
            if (!node) { fprintf(stderr, "Erro de alocacao para nodo de operando.\n"); goto cleanup_stackN_and_return_null; }
            node->expr = strdup(tok);
            if (!node->expr) { fprintf(stderr, "Erro de alocacao para expr do nodo de operando.\n"); free(node); goto cleanup_stackN_and_return_null; }
            node->prec = INT_MAX;
            stackN[++topN] = node;
        }
        else if (isOperator(tok)) {
            if (topN < 1) {
                fprintf(stderr, "ERRO: Poucos operandos (%d) para o operador '%s'. Expressao: %s\n", topN + 1, tok, Str);
                goto cleanup_stackN_and_return_null;
            }
            Node *b = stackN[topN--];
            Node *a = stackN[topN--];
            int p = prec(tok);

            char *astr = a->expr;
            char *bstr = b->expr;

            // Lógica de adição de parênteses para operadores binários
            // Operando Esquerdo (a)
            // Parentesiza se:
            // 1. A precedência de 'a' for MENOR que a do operador atual (p).
            // 2. OU A precedência de 'a' for IGUAL E o operador atual for '^' (associatividade à direita).
            if (a->prec < p || (a->prec == p && strcmp(tok, "^") == 0)) {
                int la = strlen(astr) + 3;
                char *t = malloc(la);
                if (!t) { fprintf(stderr, "Erro de alocacao para parentes em operando esquerdo.\n"); free(astr); free(bstr); free(a); free(b); goto cleanup_stackN_and_return_null; }
                snprintf(t, la, "(%s)", astr);
                astr = t;
            }

            // Operando Direito (b)
            // Parentesiza se:
            // 1. A precedência de 'b' for MENOR que a do operador atual (p).
            // 2. OU A precedência de 'b' for IGUAL E o operador atual NÃO for '^' (associatividade à esquerda).
            if (b->prec < p || (b->prec == p && strcmp(tok, "^") != 0)) {
                int lb = strlen(bstr) + 3;
                char *t = malloc(lb);
                if (!t) { fprintf(stderr, "Erro de alocacao para parentes em operando direito.\n"); free(astr); free(bstr); free(a); free(b); goto cleanup_stackN_and_return_null; }
                snprintf(t, lb, "(%s)", bstr);
                bstr = t;
            }
            
            int lbuff = strlen(astr) + strlen(tok) + strlen(bstr) + 4;
            char *buf = malloc(lbuff);
            if (!buf) { fprintf(stderr, "Erro de alocacao para expressao concatenada.\n"); free(astr); free(bstr); free(a); free(b); goto cleanup_stackN_and_return_null; }
            snprintf(buf, lbuff, "%s %s %s", astr, tok, bstr);

            Node *node = malloc(sizeof(Node));
            if (!node) { fprintf(stderr, "Erro de alocacao para nodo de operador.\n"); free(buf); free(astr); free(bstr); free(a); free(b); goto cleanup_stackN_and_return_null; }
            node->expr = buf;
            node->prec = p;

            free(a->expr); free(b->expr);
            free(a); free(b);
            if (astr != a->expr) free(astr);
            if (bstr != b->expr) free(bstr);

            stackN[++topN] = node;
        }
        else if (isFunction(tok)) {
            if (topN < 0) {
                fprintf(stderr, "ERRO: Poucos operandos (%d) para a funcao '%s'. Expressao: %s\n", topN + 1, tok, Str);
                goto cleanup_stackN_and_return_null;
            }
            Node *a = stackN[topN--];
            int p = prec(tok);
            char *astr = a->expr;

            // Parentesiza o argumento de funções se:
            // 1. A precedência do argumento for MENOR que a da função.
            // 2. OU Se a precedência for IGUAL e o argumento NÃO for um literal simples (número).
            if (a->prec < p || (a->prec == p && !(isdigit((unsigned char)astr[0]) || (astr[0] == '-' && strlen(astr) > 1 && isdigit((unsigned char)astr[1])) || (astr[0] == '.' && strlen(astr) > 1 && isdigit((unsigned char)astr[1])) ))) {
                 int la = strlen(astr) + 3;
                 char *t = malloc(la);
                 if (!t) { fprintf(stderr, "Erro de alocacao para parentes em argumento de funcao.\n"); free(astr); free(a); goto cleanup_stackN_and_return_null; }
                 snprintf(t, la, "(%s)", astr);
                 astr = t;
            }
            
            int lbuff = strlen(tok) + strlen(astr) + 3;
            char *buf = malloc(lbuff);
            if (!buf) { fprintf(stderr, "Erro de alocacao para expressao da funcao.\n"); free(astr); free(a); goto cleanup_stackN_and_return_null; }
            snprintf(buf, lbuff, "%s(%s)", tok, astr);

            Node *node = malloc(sizeof(Node));
            if (!node) { fprintf(stderr, "Erro de alocacao para nodo de funcao.\n"); free(buf); free(astr); free(a); goto cleanup_stackN_and_return_null; }
            node->expr = buf;
            node->prec = p;

            free(a->expr); free(a);
            if (astr != a->expr) free(astr);
            stackN[++topN] = node;
        }
        else {
            fprintf(stderr, "ERRO: Token inesperado ou inválido '%s' na expressao posfixa.\n", tok);
            goto cleanup_stackN_and_return_null;
        }
    }
    
    if (topN != 0) {
        fprintf(stderr, "ERRO: Expressao posfixa malformada (pilha final com %d itens, esperado 1). Expressao: %s\n", topN + 1, Str);
        goto cleanup_stackN_and_return_null;
    }

    char *res = strdup(stackN[topN]->expr);
    if (!res) { fprintf(stderr, "Erro de alocacao para resultado final.\n"); goto cleanup_stackN_and_return_null; }
    free(stackN[topN]->expr); free(stackN[topN]);

    int len = strlen(res);
    if (len > 1 && res[0] == '(' && res[len-1] == ')') {
        int balance = 0;
        int removable = 1;
        for (int k = 0; k < len; k++) {
            if (res[k] == '(') balance++;
            else if (res[k] == ')') balance--;
            if (balance == 0 && k < len - 1) {
                removable = 0;
                break;
            }
        }
        if (removable) {
            char *temp = malloc(len - 1);
            if (temp) {
                strncpy(temp, res + 1, len - 2);
                temp[len - 2] = '\0';
                free(res);
                res = temp;
            } else {
                 fprintf(stderr, "Erro de alocacao ao remover parentes externos (temp).\n");
            }
        }
    }
    return res;

cleanup_stackN_and_return_null:
    while(topN >= 0) {
        if(stackN[topN]->expr) free(stackN[topN]->expr);
        free(stackN[topN]);
        topN--;
    }
    return NULL;
}

float getValorPosFixa(char *StrPosFixa) {
    char tokens[MAX_TOKENS][MAX_TOKEN_LEN];
    int n = tokenize(StrPosFixa, tokens);
    topF = -1;

    for (int i = 0; i < n; i++) {
        char *tok = tokens[i];

        if (isdigit((unsigned char)tok[0]) ||
            (tok[0] == '-' && strlen(tok) > 1 && isdigit((unsigned char)tok[1])) ||
            (tok[0] == '.' && strlen(tok) > 1 && isdigit((unsigned char)tok[1])) ) {
            pushF(atof(tok));
        }
        else if (isOperator(tok)) {
            if (topF < 1) {
                fprintf(stderr, "Erro: Poucos operandos para o operador '%s' na avaliação posfixa.\n", tok);
                return NAN;
            }
            float b = popF();
            float a = popF();

            float r;
            if (!strcmp(tok, "+")) r = a + b;
            else if (!strcmp(tok, "-")) r = a - b;
            else if (!strcmp(tok, "*")) r = a * b;
            else if (!strcmp(tok, "/")) {
                if (fabsf(b) < 1e-9f) {
                    fprintf(stderr, "Erro: Divisão por zero.\n");
                    return INFINITY;
                }
                r = a / b;
            }
            else if (!strcmp(tok, "%")) r = fmodf(a, b);
            else if (!strcmp(tok, "^")) r = powf(a, b);
            else {
                fprintf(stderr, "Erro: Operador desconhecido '%s'.\n", tok);
                return NAN;
            }
            pushF(r);
        }
        else if (isFunction(tok)) {
            if (topF < 0) {
                fprintf(stderr, "Erro: Poucos operandos para a função '%s' na avaliação posfixa.\n", tok);
                return NAN;
            }
            float a = popF();
            float r;

            if (!strcmp(tok, "raiz")) {
                if (a < 0) {
                    fprintf(stderr, "Erro: Raiz quadrada de número negativo.\n");
                    return NAN;
                }
                r = sqrtf(a);
            }
            else if (!strcmp(tok, "sen")) r = sinf(a * M_PI / 180.0f);
            else if (!strcmp(tok, "cos")) r = cosf(a * M_PI / 180.0f);
            else if (!strcmp(tok, "tg")) {
                float angle_mod_90 = fmodf(fabsf(a), 90.0f);
                if (fabsf(angle_mod_90) < 1e-6f && fmodf(fabsf(a), 180.0f) > 1e-6f) {
                    fprintf(stderr, "Erro: Tangente indefinida para ângulo %.2f graus.\n", a);
                    return NAN;
                }
                r = tanf(a * M_PI / 180.0f);
            }
            else if (!strcmp(tok, "log")) {
                if (a <= 0) {
                    fprintf(stderr, "Erro: Logaritmo de número não positivo.\n");
                    return NAN;
                }
                r = log10f(a);
            }
            else {
                fprintf(stderr, "Erro: Função desconhecida '%s'.\n", tok);
                return NAN;
            }
            pushF(r);
        }
        else {
            fprintf(stderr, "Erro: Token inesperado ou inválido '%s' na expressao posfixa para calculo.\n", tok);
            return NAN;
        }
    }
    
    if (topF == 0) {
        return popF();
    } else {
        fprintf(stderr, "Erro: Expressão posfixa malformada (pilha não vazia ou vazia inesperadamente ao final da avaliacao).\n");
        while(topF >= 0) popF();
        return NAN;
    }
}

float getValorInFixa(char *StrInFixa) {
    char *p = getFormaPosFixa(StrInFixa);
    if (p == NULL) {
        fprintf(stderr, "Erro: Falha na conversão infixa para posfixa. Não é possível calcular o valor.\n");
        return NAN;
    }
    float v = getValorPosFixa(p);
    free(p);
    return v;
}