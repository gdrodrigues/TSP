#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include "linkedList.c"

#define NLINHAS 280
#define NCOLUNAS 2
#define NINT(a) ((a) >= 0.0 ? (int)((a)+0.5) : (int)((a)-0.5))
double euclidianDistance();
int numeroArestas(int numNos);
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

double **AlocaMatriz(int m, int n){
    double **M;
    int i;

    M = (double **)malloc(m * sizeof(double *));
    if(M == NULL){
        printf("Memoria insuficiente.\n");
        exit(1);
    }
    for(i = 0; i < m; i++){
        M[i] = (double *)malloc(n * sizeof(double));
        if(M[i] == NULL){
            printf("Memoria insuficiente.\n");
            exit(1);
        }
    }
    return M;
}

void LiberaMatriz(double **M, int m){ // m linhas
    int i;
    for(i = 0; i < m; i++)
        free(M[i]);
    free(M);
}

double **LeArquivo(char filename[]){
    double **matriz;
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
            double no = atof(util);
            double coord1;
            double coord2;
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

double euclidianDistance (int no1, int no2, double ** matrizDeCoordenadas){
    double xd = matrizDeCoordenadas[no1-1][0] - matrizDeCoordenadas[no1-1][1];
    double yd = matrizDeCoordenadas[no2-1][0] - matrizDeCoordenadas[no2-1][1];
    double dij = NINT(sqrt(xd*xd + yd*yd));
    return dij;
}


int * TSPVMP(double ** matrizDeCoordenadas, struct node *listOfNodes){
    int* rota = (int *)malloc(NLINHAS * sizeof(int));
    int indexRota = 0;

    int noAtual = ((rand() % NLINHAS -1 )+1);
    int noMenorRota;
    double custoTotal = 0;

    rota[indexRota] = noAtual;
    indexRota = indexRota + 1;
    delete(&listOfNodes, noAtual);


    //printf("Tamanho da lista é: %d", length(&listOfNodes));
    while(length(&listOfNodes) != 0){ //Enquanto lista de nós não for vazia
        double menorCusto = 100000000;
        struct node *listCopy = listOfNodes;
        //printf("Lista:\n");
        //printList(&listOfNodes);
        while (listCopy != NULL){
            //printf("item da lista: %d\n", listOfNodes->data);
            if(euclidianDistance(noAtual, listCopy->data, matrizDeCoordenadas) < menorCusto){
                //printf("to calculando");
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
        //printList(&listOfNodes);
    }
    custoTotal = custoTotal + euclidianDistance(rota[0],noAtual,matrizDeCoordenadas);
    printf("O custo total foi de: %f\n", custoTotal);
    return rota;
    
}


int main() {
    int i,j;
    int* rotaVMP = (int *)malloc(NLINHAS * sizeof(int));

    double ** matrizDeCoordenadas;
    matrizDeCoordenadas = LeArquivo("/Users/gabrielduarte/CLionProjects/TSP/a280.tsp");

    struct node *listOfNodes = NULL;
    for(i=NLINHAS; i > 0; i--){
        insertFirst(&listOfNodes, i);
    }

    rotaVMP = TSPVMP(matrizDeCoordenadas, listOfNodes);
    printf("Rota TSP:");
    printf("\n[ ");
    for(j=0; j<NLINHAS; j++){
        printf("(%d) ",rotaVMP[j]);
    }
    printf("]\n");


    LiberaMatriz(matrizDeCoordenadas, NLINHAS);
    free(rotaVMP);
    return 0;
}

/* Usado para calcular o número de arestras de um grafo dado o n de nós
int numeroArestas(int numNos){
    return (numNos* (numNos - 1)/2);
}
*/