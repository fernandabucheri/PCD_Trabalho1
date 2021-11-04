#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAM 2048
#define NUM_GERACOES 2000
#define N_THREADS 8 // define o número de threads (1, 2, 4 e 8)

int **primeiraMatriz, **segundaMatriz;

void inicializar() { // inicializando as duas matrizes e preenchendo com 0
    int i, j;

    primeiraMatriz = malloc(TAM * sizeof(int *));
    segundaMatriz = malloc(TAM * sizeof(int *));

    for (i = 0; i < TAM; i++) {
        primeiraMatriz[i] = malloc(TAM * sizeof(int));
        segundaMatriz[i] = malloc(TAM * sizeof(int));

        for (j = 0; j < TAM; j++) {
            primeiraMatriz[i][j] = 0;
            segundaMatriz[i][j] = 0;
        }
    }
}

void preencher(int **matriz) { // definindo o que foi solicitado no trabalho
    //GLIDER
    int lin = 1, col = 1;
    matriz[lin][col + 1] = 1;
    matriz[lin + 1][col + 2] = 1;
    matriz[lin + 2][col] = 1;
    matriz[lin + 2][col + 1] = 1;
    matriz[lin + 2][col + 2] = 1;

    //R-pentomino
    lin = 10;
    col = 30;
    matriz[lin][col + 1] = 1;
    matriz[lin][col + 2] = 1;
    matriz[lin + 1][col] = 1;
    matriz[lin + 1][col + 1] = 1;
    matriz[lin + 2][col + 1] = 1;
}

int getNeighbors(int i, int j, int **matriz) {
    int linha_cima, linha_meio, linha_baixo;
    int coluna_esquerda, coluna_meio, coluna_direita;
    int vivos = 0;

    //como a matriz tem bordas infinitas, a fronteira superior liga-se com a fronteira inferior e a fronteira esquerda liga-se com a fronteira direita

    //linhas
    linha_meio = i;

    if (i == 0) // se a linha atual for a linha 0, a linha de cima é a TAM - 1
        linha_cima = TAM - 1;
    else // caso contrário, a linha de cima é i - 1
        linha_cima = i - 1;

    linha_baixo = (i + 1) % TAM; // se i+1 for inferior a TAM, o resto da divisão (%) é i+1; se i+1=TAM, o resto é 0, ou seja, volta para a linha 0;

    // colunas
    coluna_meio = j;

    if (j == 0) // se a coluna atual for a 0, a coluna da esquerda é a TAM-1
        coluna_esquerda = TAM - 1;
    else // caso contrário, a coluna da esquerda é j - 1
        coluna_esquerda = j - 1;

    coluna_direita = (j + 1) % TAM; // se j+1 for inferior a TAM, o resto da divisão (%) é j+1; se j+1=TAM, o resto é 0, ou seja, volta para a coluna 0;

    // verificando os vizinhos vivos
    vivos = vivos + matriz[linha_cima][coluna_esquerda];
    vivos = vivos + matriz[linha_meio][coluna_esquerda];
    vivos = vivos + matriz[linha_baixo][coluna_esquerda];
    vivos = vivos + matriz[linha_cima][coluna_meio];
    vivos = vivos + matriz[linha_baixo][coluna_meio];
    vivos = vivos + matriz[linha_cima][coluna_direita];
    vivos = vivos + matriz[linha_meio][coluna_direita];
    vivos = vivos + matriz[linha_baixo][coluna_direita];

    return vivos;
}

int totalVivos(int **matriz) { // verificando a quantidade de células vivas em uma matriz
    int i, j, vivos = 0;

// realizando a soma 
#pragma omp parallel for private(i, j) reduction(+:vivos) num_threads(N_THREADS)
    for (i = 0; i < TAM; i++)
        for (j = 0; j < TAM; j++)
            vivos = vivos + matriz[i][j];

    return vivos;
}

void geracoes(int **geracaoAtual, int **novaGeracao) { // criando uma nova geração
    int i, j, vizinhosVivos;

//Cria-se threads e particiona as iterações do for o mais igualmente possível entre as threads
#pragma omp parallel for shared(geracaoAtual, novaGeracao) private(i, j, vizinhosVivos) num_threads(N_THREADS)
    for (i = 0; i < TAM; i++) {
        for (j = 0; j < TAM; j++) {
            // Verificando os vizinhos vivos da célula na posição i, j
            vizinhosVivos = getNeighbors(i, j, geracaoAtual);

            // Qualquer célula viva com 2 (dois) ou 3 (três) vizinhos deve sobreviver;
            if (geracaoAtual[i][j] == 1 && (vizinhosVivos == 2 || vizinhosVivos == 3))
                novaGeracao[i][j] = 1;

                // Qualquer célula morta com 3 (três) ou 6 (seis) vizinhos torna-se viva;
            else if (geracaoAtual[i][j] == 0 && (vizinhosVivos == 3 || vizinhosVivos == 6))
                novaGeracao[i][j] = 1;

                // Qualquer outro caso, células vivas devem morrer e células já mortas devem continuar mortas.
            else
                novaGeracao[i][j] = 0;
        }
    }
}

void highLife() {
    int i;

    for (i = 1; i <= NUM_GERACOES; i++) {
        if (i % 2 != 0) //se i é impar
            geracoes(primeiraMatriz, segundaMatriz);
        else  //se i é par
            geracoes(segundaMatriz, primeiraMatriz);
    }
}

int main() {
    int vivos;
    double start, end;
     
    printf("** HighLife\n");

    inicializar();
    preencher(primeiraMatriz);

    printf("Condição inicial: %d\n", totalVivos(primeiraMatriz));

    start = omp_get_wtime(); // iniciando a medida do tempo
    highLife(); // executando o jogo
    end = omp_get_wtime(); //finalizando a medida do tempo

    // se o número de gerações for par, a última geração está na "primeiraMatriz", caso contrário está na "segundaMatriz"
    if (NUM_GERACOES % 2 == 0)
        vivos = totalVivos(primeiraMatriz);
    else
        vivos = totalVivos(segundaMatriz);

    printf("Última geração (%d iterações): %d células vivas\n", NUM_GERACOES, vivos);
    printf("\nTempo de execução apenas do trecho que envolve o laço que computa as gerações sucessivas: %f segundos.\n", end - start);

    return 0;
}
