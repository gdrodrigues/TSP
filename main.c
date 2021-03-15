#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#define NLINHAS 280
#define NCOLUNAS 3
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
        printf("cheguei aqui no loop\n");
        printf("%s", linha);
        printf("contador: %d\n", contador);
        if(contador == 3){
            char* dimension1 = strtok(linha, " ");
            int dimension;
            while (dimension1 != NULL){
                dimension1 = strtok(NULL, " ");
                if (dimension1 != NULL){
                    dimension = atoi(dimension1);
                }
            }
            printf("dimensao: %d \n", dimension);
        }
        else if(contador > 5 && strcmp(linha, "EOF")!=0){

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
            printf("no %f coord1:%f e coord2:%f \n", no, coord1, coord2);
            matriz[nLinha][0] = no;
            matriz[nLinha][1] = coord1;
            matriz[nLinha][2] = coord2;
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

double euclidianDistance (){
    double xd = 288.0 - 270.0;
    double yd = 149.0 - 133.0;
    double dij = NINT(sqrt(xd*xd + yd*yd));
    return dij;
}

int numeroArestas(int numNos){
    return (numNos* (numNos - 1)/2);
}

int main() {

    double ** matriz;

    matriz = LeArquivo("/Users/gabrielduarte/CLionProjects/TSP/a280.tsp");

    /*
    double distance = euclidianDistance();
    printf("a distancia euclidiana entre 1 e 2 é: %f\n", distance);
    int arestas = numeroArestas(1748);
    printf("o numero de arestas de um grafo com x nós é: %d\n", arestas);
    */
    LiberaMatriz(matriz, NLINHAS);
    return 0;
}