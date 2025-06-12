#ifndef EXPRESSAO_H
#define EXPRESSAO_H

typedef struct {
    char posFixa[512];      // Expressão na forma pos-fixa, como 3 12 4+*
    char inFixa[512];       // Expressão na forma infixa, como 3* (12 + 4)
    float valor;            // Valor numérico da expressão
} Expressao;

// Retorna a forma inFixa de Str (posFixa)
char *getFormaInFixa(char *Str);

// Retorna a forma posFixa de Str (inFixa)
char *getFormaPosFixa(char *Str);

// Calcula o valor de Str (na forma posFixa)
float getValorPosFixa(char *StrPosFixa);

// Calcula o valor de Str (na forma inFixa)
float getValorInFixa(char *StrInFixa);

#endif