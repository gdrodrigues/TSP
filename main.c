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
#define ALFA 0.5
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
    matrizDeCoordenadas = LeArquivo("/Users/gabrielduarte/CLionProjects/TSP/instanciasTSP/lin105.tsp");

    struct node *listOfNodes = NULL;
    for(i=nlinhas; i > 0; i--){
        insertFirst(&listOfNodes, i);
    }

    Tempo_CPU_Sistema(&s_CPU_inicial, &s_total_inicial);
    //rotaVMP = TSPVMP(matrizDeCoordenadas, listOfNodes);
    //rotaVMP = TSPIMP(matrizDeCoordenadas, listOfNodes);
    //rotaVMP = TSPIMP_Randomizado(matrizDeCoordenadas, listOfNodes);
    rotaVMP = TSPSGM(matrizDeCoordenadas, listOfNodes);
    Tempo_CPU_Sistema(&s_CPU_final, &s_total_final);

    printf ("Tempo de CPU total = %f\n", s_CPU_final - s_CPU_inicial);

    printf("Rota TSP:");
    printf("\n[ ");
    for(j=0; j<nlinhas; j++){
        printf("(%d) ",rotaVMP[j]);
    }
    printf("]\n");
/*
    printf("Tira prova: \n");
    int custo = 0;
    for(int k=0; k < nlinhas; k++){
        if(k < nlinhas - 1){
            custo = custo + euclidianDistance(rotaVMP[k], rotaVMP[k+1], matrizDeCoordenadas);
        }
        else{
            custo = custo + euclidianDistance(rotaVMP[0], rotaVMP[k], matrizDeCoordenadas);
        }
    }
    printf("Custo real: %d", custo);
*/
    LiberaMatriz(matrizDeCoordenadas, nlinhas);
    return 0;
}

/* Usado para calcular o número de arestras de um grafo dado o n de nós
int numeroArestas(int numNos){
    return (numNos* (numNos - 1)/2);
}
*/