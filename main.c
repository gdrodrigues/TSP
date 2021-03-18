#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/resource.h>
#include "linkedList.c"
#define NLINHAS 2152
#define NCOLUNAS 2
#define NINT(a) ((a) >= 0.0 ? (int)((a)+0.5) : (int)((a)-0.5))
double euclidianDistance();
//int numeroArestas(int numNos);
// O número de arestas em um grafo completo é n(n-1)/2.

char *strstrip(char *s)
{
    size_t size;
    char *end;

    size = strlen(s);

    if (!size)
        return s;

    end = s + size - 1;
    while (end >= s && isspace(*end))
        end--;
    *(end + 1) = '\0';

    while (*s && isspace(*s))
        s++;

    return s;
}

float **AlocaMatriz(int m, int n){
    float **M;
    int i;

    M = (float **)malloc(m * sizeof(float *));
    if(M == NULL){
        printf("Memoria insuficiente.\n");
        exit(1);
    }
    for(i = 0; i < m; i++){
        M[i] = (float *)malloc(n * sizeof(float));
        if(M[i] == NULL){
            printf("Memoria insuficiente.\n");
            exit(1);
        }
    }
    return M;
}

void LiberaMatriz(float **M, int m){ // m linhas
    int i;
    for(i = 0; i < m; i++)
        free(M[i]);
    free(M);
}

float **LeArquivo(char filename[]){
    float **matriz;
    FILE *fp;
    int nLinha = 0;
    int contador = 0;
    char linha[1024];

    fp = fopen(filename, "r"); // read mode
    if (fp == NULL) {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    matriz = AlocaMatriz(NLINHAS, NCOLUNAS);
    while(fgets(linha, sizeof(linha), fp) != NULL) {
        //printf("%s\n", linha);
        /*
        if(contador == 3){
            char* dimension1 = strtok(linha, " ");
            int dimension;
            while (dimension1 != NULL){
                dimension1 = strtok(NULL, " ");
                if (dimension1 != NULL){
                    dimension = atoi(dimension1);
                }
            }
        }
        */
        if(contador > 5 && strcmp(linha, "EOF")!=0){

            char* util = strstrip(strtok(linha, " "));
            float no = atof(util);
            float coord1;
            float coord2;
            int cont=1;
            while(util != NULL){
                if(cont==1){
                    coord1 = atof(strtok(NULL, " "));
                }
                else{
                    coord2 = atof(strtok(NULL, " "));
                }
                cont = cont + 1;
                if(cont>2){
                    break;
                }
            }
            //printf("coord 1: %f e coord 2: %f\n", coord1, coord2);
            matriz[nLinha][0] = coord1;
            matriz[nLinha][1] = coord2;
            nLinha = nLinha + 1;

        }
        else{
            contador = contador + 1;
            continue;
        }
        contador = contador + 1;
    }
    fclose(fp);
    return matriz;

}

double euclidianDistance (int no1, int no2, float ** matrizDeCoordenadas){
    double xd = matrizDeCoordenadas[no1-1][0] - matrizDeCoordenadas[no1-1][1];
    double yd = matrizDeCoordenadas[no2-1][0] - matrizDeCoordenadas[no2-1][1];
    double dij = NINT(sqrt(xd*xd + yd*yd));
    return dij;
}

// Utilizacao de LinkedList para controle da lista de Nós (muitas operacoes de remocao de um elemento).
int * TSPVMP(float ** matrizDeCoordenadas, struct node *listOfNodes){
    int* rota = (int *)malloc(NLINHAS * sizeof(int));
    int indexRota = 0;
    srand(time(NULL));
    int noAtual = ((rand() % NLINHAS -1 )+1);
    int noMenorRota;
    double custoTotal = 0;

    rota[indexRota] = noAtual;
    indexRota = indexRota + 1;
    delete(&listOfNodes, noAtual);

    while(length(&listOfNodes) != 0){
        double menorCusto = 100000000;
        struct node *listCopy = listOfNodes;
        while (listCopy != NULL){
            if(euclidianDistance(noAtual, listCopy->data, matrizDeCoordenadas) < menorCusto){
                menorCusto = euclidianDistance(noAtual, listCopy->data, matrizDeCoordenadas);
                noMenorRota = listCopy->data;
            }
            listCopy = listCopy->next;
        }
        custoTotal = custoTotal + menorCusto;
        rota[indexRota] = noMenorRota;
        indexRota = indexRota + 1;
        noAtual = noMenorRota;
        delete(&listOfNodes, noAtual);
    }
    custoTotal = custoTotal + euclidianDistance(rota[0],noAtual,matrizDeCoordenadas);
    printf("O custo total pelo Vizinho + proximo foi de: %f\n", custoTotal);
    return rota;
    
}

int * TSPIMP(float  ** matrizDeCoordenadas, struct node *listOfNodes){

    int* rota = (int *)malloc(NLINHAS * sizeof(int));
    int indexRota = 0;
    double custoTotal = 0;
    double custoCiclo, custoCiclo2;
    int posDoNovoNo, noAserInserido, copyElement, copyNoAtual;
    srand(time(NULL));
    int i,j,k, noAtual;
    //Escolhe os 3 nós iniciais da soluçao
    for (i=0; i<3;i++){
        noAtual = ((rand() % NLINHAS -1 )+1);
        if(noAtual ==0){
            noAtual = 1;
        }
        rota[indexRota] = noAtual;
        indexRota = indexRota + 1;
        delete(&listOfNodes, noAtual);
        if(i==1){
            custoTotal = euclidianDistance(rota[0], rota[1], matrizDeCoordenadas);
        }
        else if(i==2){
            custoTotal = custoTotal + euclidianDistance(rota[1], rota[2], matrizDeCoordenadas);
            custoTotal = custoTotal + euclidianDistance(rota[2], rota[0], matrizDeCoordenadas);
        }
        else{
            continue;
        }

    }
    while(length(&listOfNodes) != 0){
        struct node *listCopy = listOfNodes;
        custoCiclo = 1000000000;
        while(listCopy != NULL){
            for(j=0; j<indexRota; j++){
                if(j < indexRota-1){
                    custoCiclo2 = euclidianDistance(rota[j], listCopy->data, matrizDeCoordenadas) + euclidianDistance(listCopy->data, rota[j+1], matrizDeCoordenadas) - euclidianDistance(rota[j], rota[j+1], matrizDeCoordenadas);
                }
                else{
                    custoCiclo2 = euclidianDistance(rota[j], listCopy->data, matrizDeCoordenadas) + euclidianDistance(listCopy->data, rota[0], matrizDeCoordenadas) - euclidianDistance(rota[j], rota[0], matrizDeCoordenadas);
                }
                if(custoCiclo2 < custoCiclo){
                    posDoNovoNo = j+1;
                    custoCiclo = custoCiclo2;
                    noAserInserido = listCopy->data;
                }
            }
            listCopy = listCopy->next;
        }
        custoTotal = custoTotal+ custoCiclo;
        copyNoAtual = noAserInserido;
        for(k=0; k<indexRota+1; k++){
            if(k >= posDoNovoNo){
                copyElement = rota[k];
                rota[k] = copyNoAtual;
                copyNoAtual = copyElement;
                if(k==indexRota){
                    rota[k+1] = copyNoAtual;
                }
            }
        }
        indexRota = indexRota + 1;
        delete(&listOfNodes, noAserInserido);
    }
    printf("O custo total via Inserção do Mais Barato é de: %f\n", custoTotal);
    return rota;
}

void Tempo_CPU_Sistema(double *seg_CPU_total, double *seg_sistema_total)
{
    long seg_CPU, seg_sistema, mseg_CPU, mseg_sistema;
    struct rusage ptempo;

    getrusage(0,&ptempo);

    seg_CPU = ptempo.ru_utime.tv_sec;
    mseg_CPU = ptempo.ru_utime.tv_usec;
    seg_sistema = ptempo.ru_stime.tv_sec;
    mseg_sistema = ptempo.ru_stime.tv_usec;

    *seg_CPU_total     = (seg_CPU + 0.000001 * mseg_CPU);
    *seg_sistema_total = (seg_sistema + 0.000001 * mseg_sistema);
}


int main() {
    int i,j;
    int* rotaVMP;
    double s_CPU_final;
    double s_CPU_inicial;
    double s_total_inicial;
    double s_total_final;

    float ** matrizDeCoordenadas;
    matrizDeCoordenadas = LeArquivo("/Users/gabrielduarte/CLionProjects/TSP/instanciasTSP/u2152.tsp");

    struct node *listOfNodes = NULL;
    for(i=NLINHAS; i > 0; i--){
        insertFirst(&listOfNodes, i);
    }

    Tempo_CPU_Sistema(&s_CPU_inicial, &s_total_inicial);
    //rotaVMP = TSPVMP(matrizDeCoordenadas, listOfNodes);
    rotaVMP = TSPIMP(matrizDeCoordenadas, listOfNodes);
    Tempo_CPU_Sistema(&s_CPU_final, &s_total_final);

    printf ("Tempo de CPU total = %f\n", s_CPU_final - s_CPU_inicial);

    printf("Rota TSP:");
    printf("\n[ ");
    for(j=0; j<NLINHAS; j++){
        printf("(%d) ",rotaVMP[j]);
    }
    printf("]\n");


    LiberaMatriz(matrizDeCoordenadas, NLINHAS);
    return 0;
}

/* Usado para calcular o número de arestras de um grafo dado o n de nós
int numeroArestas(int numNos){
    return (numNos* (numNos - 1)/2);
}
*/