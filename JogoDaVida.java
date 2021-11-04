public class JogoDaVida extends Thread {
    public int start;
    public int finish;
    public int[][] primeiraMatriz;
    public int[][] segundaMatriz;
    public int geracao;
    public int N;

    public JogoDaVida(int start, int finish, int[][] primeiraMatriz, int[][] segundaMatriz, int geracao, int N) {
        this.start = start;
        this.finish = finish;
        this.primeiraMatriz = primeiraMatriz;
        this.segundaMatriz = segundaMatriz;
        this.geracao = geracao;
        this.N = N;
    }

    public static void inicializa(int[][] matriz1, int[][] matriz2, int N) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                matriz1[i][j] = 0;
                matriz2[i][j] = 0;
            }
        }
    }

    public static void preencheMatriz(int[][] matriz) {
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

    private static int getNeighbors(int i, int j, int[][] matriz, int N) {
        int l1, l2, l3;
        int c1, c2, c3;
        int vivos = 0;

        if (i == 0) {
            l1 = N - 1;
        } else {
            l1 = i - 1;
        }

        l2 = i;
        l3 = (i + 1) % N;

        if (j == 0) {
            c1 = N - 1;
        } else {
            c1 = j - 1;
        }

        c2 = j;
        c3 = (j + 1) % N;

        vivos += matriz[l1][c1];
        vivos += matriz[l2][c1];
        vivos += matriz[l3][c1];
        vivos += matriz[l3][c2];
        vivos += matriz[l3][c3];
        vivos += matriz[l2][c3];
        vivos += matriz[l1][c3];
        vivos += matriz[l1][c2];

        return vivos;
    }

    public static int getVivos(int[][] matriz, int N) {
        int vivos = 0;

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                vivos += matriz[i][j];
            }
        }
        return vivos;
    }

    // Função auxiliar
    private static void imprime(int matriz[][]){
        int i, j;

        for(i=0; i<50; i++){
            for(j=0; j<50; j++){
                System.out.print(matriz[i][j] + " ");
            }
            System.out.println();
        }
    }

    public static void main(String[] args) throws InterruptedException {
        int maxThreads = 1; // 0 (versão serial), 1, 2, 4, 8
        int size = 2048;
        int generations = 2000;

        int[][] primeiraMatriz = new int[size][size];
        int[][] segundaMatriz = new int[size][size];
        inicializa(primeiraMatriz, segundaMatriz, size);
        preencheMatriz(primeiraMatriz);

        long startTime = System.currentTimeMillis();
        System.out.println("Condição inicial: " + getVivos(primeiraMatriz, size));

        for (int i = 1; i <= generations; i++) {
            if (maxThreads == 0) { // roda a versão serial
                JogoDaVida jogoDaVidaInstancia = new JogoDaVida(0, size, primeiraMatriz, segundaMatriz, i, size);
                if (jogoDaVidaInstancia.geracao % 2 == 1) {
                    jogoDaVidaInstancia.setNewGeneration(jogoDaVidaInstancia.primeiraMatriz, jogoDaVidaInstancia.segundaMatriz);
                    imprime(primeiraMatriz);
                    System.out.println("Geração "+ i +": " + getVivos(primeiraMatriz, size));
                }
                else {
                    jogoDaVidaInstancia.setNewGeneration(jogoDaVidaInstancia.segundaMatriz, jogoDaVidaInstancia.primeiraMatriz);
                    imprime(segundaMatriz);
                    System.out.println("Geração "+ i +": " + getVivos(primeiraMatriz, size));
                }

            } else { // roda a versão com threads
                JogoDaVida[] jogoDaVidaInstancia = new JogoDaVida[maxThreads];
                for (int j = 0; j < maxThreads; j++) {
                    jogoDaVidaInstancia[j] = new JogoDaVida((size / maxThreads) * j, (size / maxThreads) * (j + 1), primeiraMatriz, segundaMatriz, i, size);
                    jogoDaVidaInstancia[j].start();
                }
                for (int j = 0; j < maxThreads; j++) {
                    jogoDaVidaInstancia[j].join();
                }
            }
        }

        int vivos = getVivos(primeiraMatriz, size);
        System.out.println("Última geração (" + generations + " iterações): " + vivos + " células vivas");

        long total_time = (System.currentTimeMillis() - startTime) / 1000;
        System.out.println("Tempo decorrido: " + total_time + "s\n");
    }

    private void setNewGeneration(int[][] geracaoVelha, int[][] geracaoNova) {
        int vivos;

        for (int i = this.start; i < this.finish; i++) {
            for (int j = 0; j < this.N; j++) {
                vivos = getNeighbors(i, j, geracaoVelha, this.N);

                if (geracaoVelha[i][j] == 1 && (vivos == 2 || vivos == 3)) geracaoNova[i][j] = 1;
                else if (geracaoVelha[i][j] == 0 && vivos == 3) geracaoNova[i][j] = 1;
                else geracaoNova[i][j] = 0;
            }
        }
    }

    public void run() {
        if (this.geracao % 2 == 1)
            this.setNewGeneration(this.primeiraMatriz, this.segundaMatriz);
        else
            this.setNewGeneration(this.segundaMatriz, this.primeiraMatriz);
    }
}
