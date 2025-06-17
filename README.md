Para atingir o objetivo, vamos seguir os seguintes passos:

Estrutura do Projeto: Conforme solicitado, o nosso código será dividido em três arquivos para garantir a modularidade e organização:

expressao.h: O arquivo de cabeçalho (header), que já nos foi fornecido e não pode ser alterado. Ele define a estrutura Expressao e os protótipos das funções que vamos implementar.
expressao.c: Onde vamos escrever a lógica principal do nosso programa. Aqui ficarão as implementações das funções de conversão e avaliação das expressões.
main.c: O arquivo principal que irá interagir com o utilizador, pedir a expressão, chamar as funções de expressao.c para processá-la e exibir os resultados.
Estrutura de Dados Fundamental: A pilha será a nossa principal ferramenta. É a estrutura de dados ideal para resolver este tipo de problema, pois a sua natureza de "último a entrar, primeiro a sair" (LIFO) é perfeita para lidar com a ordem das operações e conversões de notação.

Funcionalidades:

Conversão Infixa -> Pós-fixada: Implementaremos a função getFormaPosFixa.
Conversão Pós-fixada -> Infixa: Implementaremos a função getFormaInFixa.
Avaliação de Expressões: Criaremos a função getValorPosFixa para calcular o resultado de uma expressão pós-fixada. A avaliação da expressão infixa (getValorInFixa ) será feita convertendo-a primeiro para pós-fixada e depois calculando o valor.

Operadores Suportados:
Básicos: +, -, *, /, %.
Especiais (um operando): raiz (raiz quadrada), sen, cos, tg (seno, cosseno, tangente) e log (logaritmo na base 10). Os ângulos para as funções trigonométricas serão em graus.
