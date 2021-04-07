#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/resource.h>
#include "linkedList.c"
#define NCOLUNAS 2
#define NINT(a) ((a) >= 0.0 ? (int)((a)+0.5) : (int)((a)-0.5))
#define ALFA 0.3
double euclidianDistance();
int nlinhas;
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

    while(fgets(linha, sizeof(linha), fp) != NULL) {
        //printf("%s\n", linha);

        if(contador == 3){
            char* dimension1 = strtok(linha, " ");
            int dimension;
            while (dimension1 != NULL){
                dimension1 = strtok(NULL, " ");
                if (dimension1 != NULL){
                    dimension = atoi(dimension1);
                }
            }
            nlinhas = dimension;
            matriz = AlocaMatriz(nlinhas, NCOLUNAS);
        }
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

double verificaCusto(int * rota, float ** matrizDeCoordenadas){
    double custo = 0;
    for(int k=0; k < nlinhas; k++){
        if(k < nlinhas - 1){
            custo = custo + euclidianDistance(rota[k], rota[k+1], matrizDeCoordenadas);
        }
        else{
            custo = custo + euclidianDistance(rota[0], rota[k], matrizDeCoordenadas);
        }
    }
    printf("Custo real: %f\n", custo);
    return custo;
}

// Utilizacao de LinkedList para controle da lista de Nós (muitas operacoes de remocao de um elemento).
int * TSPVMP(float ** matrizDeCoordenadas, struct node *listOfNodes){
    int* rota = (int *)malloc(nlinhas * sizeof(int));
    int indexRota = 0;
    srand(time(NULL));
    int noAtual = ((rand() % nlinhas -1 )+1);
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

    int* rota = (int *)malloc(nlinhas * sizeof(int));
    int indexRota = 0;
    double custoTotal = 0;
    double custoCiclo, custoCiclo2;
    int posDoNovoNo, noAserInserido, copyElement, copyNoAtual;
    srand(time(NULL));
    int i,j,k, noAtual;
    //Escolhe os 3 nós iniciais da soluçao
    for (i=0; i<3;i++){
        noAtual = ((rand() % nlinhas -1 )+1);
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

int * TSPIMP_Randomizado(float ** matrizDeCoordenadas, struct node * listOfNodes){
    int* rota = (int *)malloc(nlinhas * sizeof(int));
    int indexRota = 0;
    double custoTotal = 0;
    double custoCiclo, custoCiclo2;
    int posDoNovoNo, noAserInserido, copyElement, copyNoAtual, posRandom;
    srand(time(NULL));
    int i,j,k,w,noAtual;
    //Escolhe os 3 nós iniciais da soluçao
    for (i=0; i<3;i++){
        noAtual = ((rand() % nlinhas -1 )+1);
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
        custoCiclo = 1000000000;
        struct node *listCopy = listOfNodes;
        posRandom = ((rand() % length(&listOfNodes)-1)+1);
        for(w = 0; w < posRandom; w++){
            listCopy = listCopy->next;
        }
        noAtual = listCopy->data;
        for(j=0; j<indexRota; j++){
            if(j < indexRota-1){
                custoCiclo2 = euclidianDistance(rota[j], noAtual, matrizDeCoordenadas) + euclidianDistance(noAtual, rota[j+1], matrizDeCoordenadas) - euclidianDistance(rota[j], rota[j+1], matrizDeCoordenadas);
            }
            else{
                custoCiclo2 = euclidianDistance(rota[j], noAtual, matrizDeCoordenadas) + euclidianDistance(noAtual, rota[0], matrizDeCoordenadas) - euclidianDistance(rota[j], rota[0], matrizDeCoordenadas);
            }
            if(custoCiclo2 < custoCiclo){
                posDoNovoNo = j+1;
                custoCiclo = custoCiclo2;
                noAserInserido = noAtual;
            }
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
    printf("O custo total via Inserção do Mais Barato Randomizado é de: %f\n", custoTotal);
    return rota;
}

struct node* rclSet(float **matrizDeCoordenadas, struct node *listOfNodes, int ultimoNoRota){
    double cmin=100000000, cmax=-1;
    struct node * rcl = NULL;
    struct node * copyOfNodes = listOfNodes;
    while(copyOfNodes != NULL){ // define o cmin e o cmax
        if(euclidianDistance(ultimoNoRota, copyOfNodes->data,matrizDeCoordenadas) < cmin){
            cmin = euclidianDistance(ultimoNoRota, copyOfNodes->data,matrizDeCoordenadas);
        }
        if(euclidianDistance(ultimoNoRota, copyOfNodes->data,matrizDeCoordenadas) > cmax){
            cmax = euclidianDistance(ultimoNoRota, copyOfNodes->data,matrizDeCoordenadas);
        }
        copyOfNodes = copyOfNodes->next;
    }
    //printf("Lista de nos com %d nos: \n", length(&listOfNodes));
    //printList(&listOfNodes);
    while (listOfNodes != NULL) {
        if((euclidianDistance(listOfNodes->data, ultimoNoRota, matrizDeCoordenadas) >= cmin) && (euclidianDistance(listOfNodes->data, ultimoNoRota, matrizDeCoordenadas) <= (cmin + (ALFA*(cmax-cmin))))){
            insertFirst(&rcl, listOfNodes->data);
        }
        listOfNodes = listOfNodes->next;

    }
    //printf("RCL com %d nos: ", length(&rcl));
    //printList(&rcl);
    //printf("\n");
    return rcl;
}

int * TSPSGM(float ** matrizDeCoordenadas, struct node * listOfAllNodes){
    int*rotaFinal;
    double custoTotal = 100000000;
    int contador=0;
    int n = 100;
    srand(time(NULL));
    while (contador < n){
        int* rota = (int *)malloc((nlinhas) * sizeof(int));
        //printf("=================== RODADA %d =====================\n", contador+1);
        struct node *nodeCopy = NULL;
        struct node *temporaryCopy = listOfAllNodes;
        for(int j=0; j<length(&listOfAllNodes);j++){
            insertFirst(&nodeCopy, temporaryCopy->data);
            temporaryCopy = temporaryCopy->next;
        }
        temporaryCopy = NULL;

        int indexRota = 0, posRandom;
        double custo = 0;

        struct node * rcl = NULL;
        //Adiciona o primeiro no de forma aleatoria;
        int noAtual = ((rand() % nlinhas -1 )+1);
        if(noAtual == 0){
            noAtual = 1;
        }
        rota[indexRota] = noAtual;
        indexRota++;
        delete(&nodeCopy, noAtual);
        while(length(&nodeCopy) != 0){
            //printf("noAtual: %d\n", noAtual);
            rcl = rclSet(matrizDeCoordenadas, nodeCopy, noAtual);
            posRandom = ((rand() % length(&rcl)-1)+1);
            for(int i = 0; i < posRandom; i++){
                rcl = rcl->next;
            }
            rota[indexRota] = rcl->data;
            custo = custo + euclidianDistance(rcl->data, rota[indexRota-1], matrizDeCoordenadas);
            delete(&nodeCopy, rcl->data);
            noAtual = rcl->data;
            indexRota++;
        }
        custo = custo + euclidianDistance(rota[0], rota[indexRota-1], matrizDeCoordenadas);
        //printf("O custo nesta rodada foi de: %f\n", custo);
        if (custo < custoTotal){
            rotaFinal = rota;
            custoTotal = custo;
        }
        contador++;
    }
    printf("O custo total via Semi-Greedy MultiStart com n = %d e alfa = %f é de: %f\n",n,ALFA, custoTotal);
    return rotaFinal;
}

int * TSPBestImproving(int * tour, double cost, float ** matrizDeCoordenadas){
    int improvement = 1;
    int size = nlinhas;
    int * newTour = (int *)malloc((nlinhas) * sizeof(int)); //construcao de cada S'
    int * tourCopy = (int *)malloc((nlinhas) * sizeof(int));// copia do tour original S
    int * bestTour = (int *)malloc((nlinhas) * sizeof(int)); // melhor S' de S
    double newCost;
    for(int i = 0; i < nlinhas; i++){
        tourCopy[i] = tour[i];
    }
    while(improvement != 0){
        improvement = 0;
        double bestCost = 100000000;
        //Movimento 2opt
        for (int i = 1; i < size - 1; i++) {
            for (int k = i + 1; k < size - 1; k++) {
                newCost = cost;
                for (int c = 0; c <= i - 1; c++) {
                    newTour[c] = tourCopy[c];
                }
                int dec = 0;
                for (int c = i; c <= k; c++) {
                    newTour[c] = tourCopy[k - dec];
                    if (c == i) {
                        newCost = newCost + euclidianDistance(tourCopy[c - 1], tourCopy[k],
                                                              matrizDeCoordenadas); // add aresta C-G
                        newCost = newCost - euclidianDistance(tourCopy[c - 1], tourCopy[c],
                                                              matrizDeCoordenadas); //tira aresta C-D
                    }
                    dec++;
                }

                for (int c = k + 1; c < size; c++) {
                    newTour[c] = tourCopy[c];
                    if (c == k + 1) {
                        newCost = newCost - euclidianDistance(tourCopy[c - 1], tourCopy[c],
                                                              matrizDeCoordenadas); //tira aresta G-H
                        newCost = newCost +
                                  euclidianDistance(newTour[k], tourCopy[c], matrizDeCoordenadas); // add aresta D-H
                    }
                }
                /*
                printf("TOUR S' i = %d, k = %d, Cost = %f e newCost = %f\n",i, k, cost, newCost);
                */

                if(newCost < bestCost){
                    for(int i = 0; i < nlinhas; i++){
                        bestTour[i] = newTour[i];
                    }
                    bestCost = newCost;
                }
            }
        }
        //printf(" ============== MELHOR CUSTO ENCONTRADO NOS S' = %f =====================\n", bestCost);
        if(bestCost < cost){
            improvement = 1;
            for(int i = 0; i < nlinhas; i++){
                tourCopy[i] = bestTour[i];
            }
            cost = bestCost;
        }
    }
    free(bestTour);
    free(newTour);
    printf("O novo custo, utilizando Busca Local - BestImproving é de: %f\n", cost);
    return tourCopy;
}

int * TSPFirstImproving(int * tour, double cost, float ** matrizDeCoordenadas){
    int improvement = 1;
    int size = nlinhas;
    int * newTour = (int *)malloc((nlinhas) * sizeof(int));
    int * testTour = (int *)malloc((nlinhas) * sizeof(int));
    double newCost, achei;
    for(int i = 0; i < nlinhas; i++){
        testTour[i] = tour[i];
    }
    while (improvement != 0){
        improvement = 0;
        for (int i = 1; i < size - 1; i++){
            for(int k = i + 1; k < size - 1; k++){
                achei = 0;
                newCost = cost;
                for (int c = 0; c <= i-1; c++){
                    newTour[c] = testTour[c];
                }
                int dec = 0;
                for (int c = i; c<=k; c++){
                    newTour[c] = testTour[k-dec];
                    if(c==i){
                        newCost = newCost + euclidianDistance(testTour[c-1], testTour[k], matrizDeCoordenadas); // add aresta C-G
                        newCost = newCost - euclidianDistance(testTour[c-1], testTour[c], matrizDeCoordenadas); //tira aresta C-D
                    }
                    dec++;
                }

                for(int c = k+1; c < size; c++){
                    newTour[c] = testTour[c];
                    if(c==k+1){
                        newCost = newCost - euclidianDistance(testTour[c-1], testTour[c], matrizDeCoordenadas); //tira aresta G-H
                        newCost = newCost + euclidianDistance(newTour[k], testTour[c], matrizDeCoordenadas); // add aresta D-H
                    }
                }
                /*
                printf("i = %d, k = %d, Cost = %f e newCost = %f\n",i, k, cost, newCost);
                verificaCusto(newTour, matrizDeCoordenadas);
                */
                if (newCost < cost){
                    for(int i = 0; i < nlinhas; i++){
                        testTour[i] = newTour[i];
                    }
                    cost = newCost;
                    improvement = 1;
                    achei = 1;
                    /*
                    printf("========== Tour atualizado ============ :");
                    printf("\n[ ");
                    for(int j=0; j<nlinhas; j++){
                        printf("(%d) ",testTour[j]);
                    }
                    printf("]\n");
                    */
                    break;
                }
            }
            if(achei){
                break;
            }
        }
    }
    printf("O novo custo, utilizando Busca Local - FirstImproving é de: %f\n", cost);

    return testTour;
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
    int* rotaIMP;
    int* rotaBL;
    double s_CPU_final;
    double s_CPU_inicial;
    double s_total_inicial;
    double s_total_final;

    float ** matrizDeCoordenadas;
    matrizDeCoordenadas = LeArquivo("/Users/gabrielduarte/CLionProjects/TSP/instanciasTSP/a280.tsp");

    struct node *listOfNodes = NULL;
    for(i=nlinhas; i > 0; i--){
        insertFirst(&listOfNodes, i);
    }

    rotaIMP = TSPIMP(matrizDeCoordenadas, listOfNodes);
    double custo = verificaCusto(rotaIMP, matrizDeCoordenadas);
    Tempo_CPU_Sistema(&s_CPU_inicial, &s_total_inicial);
    //rotaVMP = TSPVMP(matrizDeCoordenadas, listOfNodes);
    //rotaVMP = TSPIMP_Randomizado(matrizDeCoordenadas, listOfNodes);
    //rotaVMP = TSPSGM(matrizDeCoordenadas, listOfNodes);
    //rotaBL = TSPFirstImproving(rotaIMP, custo, matrizDeCoordenadas);
    rotaBL = TSPBestImproving(rotaIMP, custo, matrizDeCoordenadas);
    Tempo_CPU_Sistema(&s_CPU_final, &s_total_final);

    printf ("Tempo de CPU total = %f\n", s_CPU_final - s_CPU_inicial);
    printf("Rota Heuristica:");
    printf("\n[ ");
    for(j=0; j<nlinhas; j++){
        printf("(%d) ",rotaIMP[j]);
    }

    printf("]\n");
    printf("Rota TSP Local Search:");
    printf("\n[ ");
    for(j=0; j<nlinhas; j++){
        printf("(%d) ",rotaBL[j]);
    }
    printf("]\n");
    verificaCusto(rotaBL, matrizDeCoordenadas);
    LiberaMatriz(matrizDeCoordenadas, nlinhas);
    return 0;
}

/* Usado para calcular o número de arestras de um grafo dado o n de nós
int numeroArestas(int numNos){
    return (numNos* (numNos - 1)/2);
}
*/